#include "chatserver.h"
#include "../server/manager.h"
#include "../utils/inputlimits.h"
#include "serverprotocol.h"
#include <QJsonArray>
#include <QJsonParseError>
#include <QTimer>
#include <stdexcept>

namespace {
constexpr qsizetype MaxFrame = 16 * 1024 * 1024;
}

ChatServer::ChatServer(Manager &serverManager, QObject *parent)
    : QObject(parent),
      manager(serverManager) {
    clock.start();

    auto *expiryTimer = new QTimer(this);
    connect(expiryTimer, &QTimer::timeout, this, [this] {
        const qint64 elapsedMilliseconds = clock.elapsed();
        for (auto &session : sessions) {
            if (session.pendingUserId > 0 && elapsedMilliseconds >= session.loginExpires) {
                session.pendingUserId = 0;
                session.loginExpires = 0;
                session.factorAttempts = 0;
            }
            if (!session.pendingTotpKey.isEmpty() && elapsedMilliseconds >= session.setupExpires) {
                session.pendingTotpKey.clear();
                session.setupExpires = 0;
                session.setupAttempts = 0;
            }
        }
    });
    expiryTimer->start(1000);

    connect(&manager, &Manager::notifyUser, this, &ChatServer::notify);
    connect(&server, &QTcpServer::newConnection, this, [this] {
        while (server.hasPendingConnections()) {
            auto *socket = server.nextPendingConnection();
            if (sessions.size() >= 64) {
                socket->disconnectFromHost();
                socket->deleteLater();
                continue;
            }

            socket->setReadBufferSize(MaxFrame + 1);
            sessions.insert(socket, {});
            connect(socket, &QTcpSocket::readyRead, this, [this, socket] {
                read(socket);
            });
            connect(socket, &QTcpSocket::disconnected, this, [this, socket] {
                const int userId = sessions.value(socket).userId;
                sessions.remove(socket);
                if (userId > 0 && !online(userId)) {
                    presence(userId, false);
                }
                socket->deleteLater();
            });
        }
    });
}

bool ChatServer::listen(const QHostAddress &address, quint16 port) {
    return server.listen(address, port);
}

bool ChatServer::online(int userId) const {
    for (const auto &session : sessions) {
        if (session.userId == userId) {
            return true;
        }
    }
    return false;
}

void ChatServer::send(QTcpSocket *socket, const QJsonObject &jsonObject) {
    const auto frame = ServerProtocol::jsonObjectToData(jsonObject);
    if (frame.size() > MaxFrame || socket->bytesToWrite() + frame.size() > 2 * MaxFrame) {
        socket->abort();
        return;
    }
    socket->write(frame);
}

void ChatServer::notify(int userId, const QJsonObject &event) {
    const auto sockets = sessions.keys();
    for (auto *socket : sockets) {
        if (sessions.value(socket).userId == userId) {
            send(socket, event);
        }
    }
}

void ChatServer::presence(int userId, bool isOnline) {
    try {
        const auto friendUserIds = manager.friendIds(userId);
        const QJsonObject event{
            {"type", "friend_update"},
            {"id", userId},
            {"bool", isOnline},
        };

        for (int friendUserId : friendUserIds) {
            notify(friendUserId, event);
        }
    } catch (...) {
        // Database errors must not escape a Qt signal handler.
    }
}

void ChatServer::read(QTcpSocket *socket) {
    if (!sessions.contains(socket)) {
        return;
    }

    sessions[socket].buffer += socket->readAll();
    while (sessions.contains(socket)) {
        auto &buffer = sessions[socket].buffer;
        const auto newlineIndex = buffer.indexOf('\n');
        if (newlineIndex < 0) {
            if (buffer.size() > MaxFrame) {
                socket->abort();
            }
            return;
        }
        if (newlineIndex > MaxFrame) {
            socket->abort();
            return;
        }

        const auto jsonLine = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);
        QJsonParseError parseError;
        const auto jsonDocument = QJsonDocument::fromJson(jsonLine, &parseError);
        if (parseError.error != QJsonParseError::NoError || !jsonDocument.isObject()) {
            socket->abort();
            return;
        }
        dispatch(socket, jsonDocument.object());
    }
}

void ChatServer::limitPasswordAttempts(const QString &identity) {
    const qint64 elapsedMilliseconds = clock.elapsed();
    for (auto windowIterator = passwordWindows.begin(); windowIterator != passwordWindows.end();) {
        if (elapsedMilliseconds - windowIterator->started >= 60000) {
            windowIterator = passwordWindows.erase(windowIterator);
        } else {
            ++windowIterator;
        }
    }

    // Bound the map as well as individual usernames, so random account names
    // cannot grow server memory without limit.
    if (!passwordWindows.contains(identity) && passwordWindows.size() >= 1024) {
        throw std::runtime_error("rate_limited");
    }

    auto &passwordWindow = passwordWindows[identity];
    if (passwordWindow.attempts == 0) {
        passwordWindow.started = elapsedMilliseconds;
    }
    ++passwordWindow.attempts;
    if (passwordWindow.attempts > 30) {
        throw std::runtime_error("rate_limited");
    }
}

QJsonObject ChatServer::startLogin(Session &session, const QJsonObject &arguments) {
    session.pendingUserId = 0;
    session.loginExpires = 0;
    session.factorAttempts = 0;

    const auto usernameValue = arguments.value("username");
    const QString username = usernameValue.toString();
    if (!usernameValue.isString() || username.size() > InputLimits::MaxUsernameLength ||
        username.trimmed().isEmpty() || username.contains(QChar::Null)) {
        throw std::runtime_error("invalid_request");
    }

    limitPasswordAttempts("username:" + username);
    const int userId = manager.verifyLoginPassword(arguments);
    if (manager.mfaEnabled(userId)) {
        session.pendingUserId = userId;
        session.loginExpires = clock.elapsed() + 120000;
        return {{"mfa_required", true}};
    }

    const auto result = manager.finishLogin(userId);
    session.userId = userId;
    return result;
}

QJsonObject ChatServer::verifyLoginCode(Session &session, const QJsonObject &arguments) {
    if (session.pendingUserId == 0 || clock.elapsed() >= session.loginExpires) {
        session.pendingUserId = 0;
        session.loginExpires = 0;
        session.factorAttempts = 0;
        throw std::runtime_error("authentication_expired");
    }

    ++session.factorAttempts;
    QJsonObject result;
    try {
        result = manager.verifyLoginFactor(session.pendingUserId, arguments);
    } catch (...) {
        if (session.factorAttempts >= 5) {
            session.pendingUserId = 0;
            session.loginExpires = 0;
            session.factorAttempts = 0;
        }
        throw;
    }

    session.userId = session.pendingUserId;
    session.pendingUserId = 0;
    session.loginExpires = 0;
    session.factorAttempts = 0;
    return result;
}

QJsonObject ChatServer::beginTotpSetup(Session &session, const QJsonObject &arguments) {
    limitPasswordAttempts("user:" + QString::number(session.userId));
    const auto result = manager.beginMfaSetup(session.userId, arguments);
    session.pendingTotpKey = result["secret"].toString();
    session.setupExpires = clock.elapsed() + 300000;
    session.setupAttempts = 0;
    return result;
}

QJsonObject ChatServer::confirmTotpSetup(Session &session, const QJsonObject &arguments) {
    if (session.pendingTotpKey.isEmpty() || clock.elapsed() >= session.setupExpires) {
        session.pendingTotpKey.clear();
        session.setupExpires = 0;
        session.setupAttempts = 0;
        throw std::runtime_error("mfa_setup_expired");
    }

    ++session.setupAttempts;
    QJsonObject result;
    try {
        result = manager.confirmMfaSetup(session.userId, session.pendingTotpKey, arguments);
    } catch (...) {
        if (session.setupAttempts >= 5) {
            session.pendingTotpKey.clear();
            session.setupExpires = 0;
            session.setupAttempts = 0;
        }
        throw;
    }

    session.pendingTotpKey.clear();
    session.setupExpires = 0;
    session.setupAttempts = 0;
    return result;
}

void ChatServer::dispatch(QTcpSocket *socket, const QJsonObject &request) {
    const auto requestId = request["request_id"].toString();
    QJsonObject reply{{"type", "response"}, {"request_id", requestId}, {"ok", false}};
    try {
        if (requestId.isEmpty() || requestId.size() > 64 || !request["args"].isObject()) {
            throw std::runtime_error("invalid_request");
        }

        const auto operation = request["type"].toString();
        const auto arguments = request["args"].toObject();
        // These checks precede special authentication operations too. No
        // request may pick its own actor or pending second-factor identity.
        for (const auto &key : {"user_id", "sender_id", "messageSender", "actor_id"}) {
            if (arguments.contains(key)) {
                throw std::runtime_error("invalid_request");
            }
        }

        auto &session = sessions[socket];
        if (operation == "login" || operation == "register" || operation == "available" ||
            operation == "login_verify") {
            if (session.userId > 0) {
                throw std::runtime_error("already_authenticated");
            }
            if (clock.elapsed() - session.authWindow > 60000) {
                session.authAttempts = 0;
                session.authWindow = clock.elapsed();
            }
            ++session.authAttempts;
            if (session.authAttempts > 30) {
                throw std::runtime_error("rate_limited");
            }
        } else if (session.userId == 0) {
            throw std::runtime_error("authentication_required");
        }

        QJsonObject result;
        bool completedLogin = false;
        if (operation == "login") {
            result = startLogin(session, arguments);
            completedLogin = session.userId > 0;
        } else if (operation == "login_verify") {
            result = verifyLoginCode(session, arguments);
            completedLogin = true;
        } else if (operation == "mfa_setup_begin") {
            result = beginTotpSetup(session, arguments);
        } else if (operation == "mfa_setup_confirm") {
            result = confirmTotpSetup(session, arguments);
        } else if (operation == "mfa_setup_cancel") {
            session.pendingTotpKey.clear();
            session.setupExpires = 0;
            session.setupAttempts = 0;
        } else {
            if (operation == "mfa_disable") {
                limitPasswordAttempts("user:" + QString::number(session.userId));
            }
            result = manager.dispatch(session.userId, operation, arguments);
        }

        if (completedLogin) {
            const int userId = session.userId;
            send(socket,
                 {{"type", "response"},
                  {"request_id", requestId},
                  {"ok", true},
                  {"result", result}});
            presence(userId, true);
            return;
        }
        if (operation == "friends") {
            auto users = result["users"].toArray();
            for (int userIndex = 0; userIndex < users.size(); ++userIndex) {
                auto user = users[userIndex].toObject();
                user["online"] = online(user["id"].toInt());
                users[userIndex] = user;
            }
            result["users"] = users;
        }
        reply["ok"] = true;
        reply["result"] = result;
    } catch (const std::exception &error) {
        reply["error"] = QString::fromLatin1(error.what());
    }
    send(socket, reply);
}
