#include "chatserver.h"
#include "../server/manager.h"
#include "serverprotocol.h"
#include <QJsonArray>
#include <QJsonParseError>
#include <stdexcept>

namespace {
constexpr qsizetype MaxFrame = 16 * 1024 * 1024;
}

ChatServer::ChatServer(Manager &serverManager, QObject *parent)
    : QObject(parent),
      manager(serverManager) {
    clock.start();

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

void ChatServer::dispatch(QTcpSocket *socket, const QJsonObject &request) {
    const auto requestId = request["request_id"].toString();
    QJsonObject reply{{"type", "response"}, {"request_id", requestId}, {"ok", false}};
    try {
        if (requestId.isEmpty() || requestId.size() > 64 || !request["args"].isObject()) {
            throw std::runtime_error("invalid_request");
        }

        const auto operation = request["type"].toString();
        const auto arguments = request["args"].toObject();
        auto &session = sessions[socket];
        if (operation == "login" || operation == "register" || operation == "available") {
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

        auto result = manager.dispatch(session.userId, operation, arguments);
        if (operation == "login") {
            session.userId = result["user"].toObject()["id"].toInt();
            send(socket,
                 {{"type", "response"},
                  {"request_id", requestId},
                  {"ok", true},
                  {"result", result}});
            presence(session.userId, true);
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
