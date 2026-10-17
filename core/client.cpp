#include "client.h"
#include "manager.h"
#include <QElapsedTimer>
#include <QJsonArray>
#include <QTimer>

namespace {
constexpr qsizetype MaxFrameBytes = 16 * 1024 * 1024;
constexpr int ConnectionTimeoutMs = 5000;
constexpr int ResponseTimeoutMs = 10000;
constexpr qsizetype MaxQueuedEvents = 1000;
} // namespace

Client::Client(QObject *parent)
    : QObject(parent),
      socket(new QTcpSocket(this)),
      m_manager(Manager::get()) {
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
    connect(socket, &QTcpSocket::connected, this, &Client::connected);
    connect(socket, &QTcpSocket::disconnected, this, [this] {
        id = 0;
        friends.clear();
        receiveBuffer.clear();
        events.clear();
        emit disconnected();
    });
}

QString Client::getClientName() {
    return name;
}

QString Client::getClientAvatar() {
    return avatar;
}

Status Client::getClientStatus() {
    return status;
}

int Client::getClientID() {
    return id;
}

Data &Client::getClientData() {
    return clientData;
}

QHash<Data, int> &Client::getFriends() {
    return friends;
}

void Client::setFriends(QHash<Data, int> &updatedFriends) {
    friends = updatedFriends;
}

void Client::setClientName(QString displayName) {
    name = displayName;
}

void Client::setClientAvatar(QString avatarPath) {
    avatar = avatarPath;
}

void Client::setClientStatus(Status userStatus) {
    status = userStatus;
}

void Client::setClientID(int userId) {
    id = userId;
}

void Client::setClientData(Data &userData) {
    clientData = userData;
}

void Client::setEndpoint(const QString &hostname, int serverPort) {
    host = hostname;
    port = serverPort;
}

bool Client::ensureConnected() {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        return true;
    }

    socket->abort();
    socket->connectToHost(host, quint16(port));
    if (socket->waitForConnected(ConnectionTimeoutMs)) {
        return true;
    }

    lastError = "Cannot connect to ChatServer.";
    return false;
}

void Client::disconnectFromServer() {
    socket->abort();
    id = 0;
    friends.clear();
    responses.clear();
    events.clear();
}

QJsonObject Client::request(const QString &operation, const QJsonObject &arguments) {
    if (requesting) {
        lastError = "A request is already in progress.";
        return {};
    }
    if (!ensureConnected()) {
        return {};
    }

    requesting = true;
    ++requestCounter;
    const QString requestId = QString::number(requestCounter);
    const QJsonObject requestObject{
        {"type", operation},
        {"request_id", requestId},
        {"args", arguments},
    };
    const QByteArray payload = QJsonDocument(requestObject).toJson(QJsonDocument::Compact) + '\n';
    if (payload.size() > MaxFrameBytes) {
        requesting = false;
        lastError = "Request is too large.";
        return {};
    }

    socket->write(payload);
    socket->flush();

    QElapsedTimer elapsed;
    elapsed.start();
    while (!responses.contains(requestId) && socket->state() == QAbstractSocket::ConnectedState &&
           elapsed.elapsed() < ResponseTimeoutMs) {
        socket->waitForReadyRead(int(ResponseTimeoutMs - elapsed.elapsed()));
    }
    requesting = false;

    if (!responses.contains(requestId)) {
        socket->abort();
        lastError = "Server did not reply. Reconnect and check whether the operation completed.";
        return {};
    }

    const QJsonObject response = responses.take(requestId);
    if (!response["ok"].toBool()) {
        lastError = response["error"].toString("Request failed.");
        return response;
    }

    lastError.clear();
    // Process notifications that were queued while this request was waiting.
    QTimer::singleShot(0, this, &Client::dispatchEvents);
    return response;
}

void Client::onReadyRead() {
    receiveBuffer += socket->readAll();
    while (true) {
        const qsizetype newlinePosition = receiveBuffer.indexOf('\n');
        if (newlinePosition < 0) {
            if (receiveBuffer.size() > MaxFrameBytes) {
                socket->abort();
            }
            break;
        }
        if (newlinePosition > MaxFrameBytes) {
            socket->abort();
            break;
        }

        const QByteArray line = receiveBuffer.left(newlinePosition);
        receiveBuffer.remove(0, newlinePosition + 1);

        QJsonParseError parseError;
        const QJsonDocument document = QJsonDocument::fromJson(line, &parseError);
        if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
            socket->abort();
            break;
        }

        const QJsonObject receivedObject = document.object();
        if (receivedObject["type"] == "response") {
            const QString responseId = receivedObject["request_id"].toString();
            if (requesting && responseId == QString::number(requestCounter)) {
                responses.insert(responseId, receivedObject);
            }
        } else {
            events.append(receivedObject);
            if (events.size() > MaxQueuedEvents) {
                socket->abort();
                break;
            }
        }
    }

    if (!requesting) {
        QTimer::singleShot(0, this, &Client::dispatchEvents);
    }
}

void Client::dispatchEvents() {
    if (requesting || dispatching) {
        return;
    }

    dispatching = true;
    while (!events.isEmpty() && !requesting && id > 0) {
        const QJsonObject event = events.takeFirst();
        const QString eventType = event["type"].toString();

        if (eventType == "message") {
            Data messageData = Utils::buildData(event);
            m_manager->resolveDataMedia(messageData);
            Message message = messageData.message;
            emit newMessage(messageData);
            emit updateConversations(message, messageData);
        } else if (eventType == "friend_update") {
            const int friendUserId = event["id"].toInt();
            const bool isOnline = event["bool"].toBool();
            emit updateFriends(friendUserId, isOnline);
        } else if (eventType == "relationships_changed") {
            emit relationshipsChanged();
        }
    }
    dispatching = false;
}
