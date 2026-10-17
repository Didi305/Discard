#include "manager.h"
#include "utils/attachmentpayload.h"
#include "utils/errormessages.h"
#include "utils/mediacache.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QTimer>

Manager::Manager(QObject *parent)
    : QObject(parent) {
}

Manager *Manager::get() {
    static Manager instance;
    return &instance;
}

void Manager::setClient(Client *client) {
    this->client = client;
}

Client *Manager::getClient() {
    return client;
}

bool Manager::connectServer() {
    requestError.clear();
    return client && client->ensureConnected();
}

QString Manager::lastError() const {
    if (!requestError.isEmpty()) {
        return requestError;
    }
    if (client) {
        return client->errorString();
    }
    return "Client is not initialized.";
}

QJsonObject Manager::call(const QString &operation, const QJsonObject &arguments) {
    requestError.clear();
    if (!client) {
        requestError = tr("Client is not initialized.");
        return {};
    }
    const auto reply = client->request(operation, arguments);
    if (reply["ok"].toBool()) {
        return reply;
    }
    requestError =
        ErrorMessages::message(operation, reply["error"].toString(), client->errorString());
    return reply;
}

QString
Manager::avatarPathFromValues(int userId, const QByteArray &avatarBytes, const QString &filename) {
    QString avatarPath;
    if (avatarBytes.isEmpty()) {
        avatarPath = ":/avatars/default.png";
    } else {
        avatarPath = MediaCache::save(avatarBytes, "avatars", userId, filename);
    }
    if (avatarPath.isEmpty()) {
        avatarPath = ":/avatars/default.png";
    }
    avatarPaths[userId] = avatarPath;
    return avatarPath;
}

Data Manager::profile(const QJsonObject &userObject) {
    Data profileData{};
    profileData.user_id = userObject["id"].toInt();
    profileData.displayName = userObject["name"].toString();
    profileData.status = Utils::statusFromString(userObject["status"].toString());
    profileData.avatar = avatarPathForUser(profileData.user_id, true);
    return profileData;
}

bool Manager::authenticate(const QString &username, const QString &password) {
    const auto reply = call("login", {{"username", username}, {"password", password}});
    if (!reply["ok"].toBool()) {
        return false;
    }
    const auto userObject = reply["result"].toObject()["user"].toObject();
    const int userId = userObject["id"].toInt();
    if (userId <= 0) {
        requestError = tr("The server returned an incomplete sign-in response.");
        return false;
    }
    avatarPaths.clear();
    auto profileData = profile(userObject);
    client->setClientID(profileData.user_id);
    client->setClientName(profileData.displayName);
    client->setClientStatus(profileData.status);
    client->setClientAvatar(profileData.avatar);
    client->setClientData(profileData);
    fetchFriends(profileData.user_id, client->getFriends());
    if (client->getClientID() != profileData.user_id) {
        requestError = tr("The connection was lost during sign-in. Cancel and sign in again.");
        return false;
    }
    return true;
}

bool Manager::createUser(QString username, QString password, QString nickname) {
    const auto reply =
        call("register", {{"username", username}, {"password", password}, {"nickname", nickname}});
    return reply["ok"].toBool();
}

bool Manager::checkUsernameAvailibity(QString username) {
    const auto reply = call("available", {{"field", "username"}, {"value", username}});
    return reply["ok"].toBool() && reply["result"].toObject()["available"].toBool();
}

bool Manager::checkNicknameAvailibity(QString nickname) {
    const auto reply = call("available", {{"field", "nickname"}, {"value", nickname}});
    return reply["ok"].toBool() && reply["result"].toObject()["available"].toBool();
}

bool Manager::updateUserAvatar(QString path, QString nickname) {
    Q_UNUSED(nickname)
    QFile avatarFile(path);
    if (!avatarFile.open(QIODevice::ReadOnly) || avatarFile.size() > 2 * 1024 * 1024) {
        return false;
    }
    const auto reply = call("set_avatar",
                            {{"filename", QFileInfo(path).fileName()},
                             {"bytes", QString::fromLatin1(avatarFile.readAll().toBase64())}});
    if (reply["ok"].toBool()) {
        avatarPaths.remove(client->getClientID());
    }
    return reply["ok"].toBool();
}

QString Manager::avatarPathForUser(int userId, bool reuseCached) {
    if (reuseCached && avatarPaths.contains(userId) && QFileInfo::exists(avatarPaths[userId])) {
        return avatarPaths[userId];
    }
    const auto reply = call("avatar", {{"peer_id", userId}});
    if (!reply["ok"].toBool()) {
        return ":/avatars/default.png";
    }
    const auto avatarObject = reply["result"].toObject();
    return avatarPathFromValues(userId,
                                QByteArray::fromBase64(avatarObject["bytes"].toString().toLatin1()),
                                avatarObject["filename"].toString());
}

QString Manager::checkFriendship(QString nickname) {
    const auto reply = call("friend_request", {{"nickname", nickname}});
    if (reply["ok"].toBool()) {
        return reply["result"].toObject()["status"].toString();
    }
    return {};
}

std::vector<Data> Manager::fetchRequests(int userId, RequestType requestType) {
    Q_UNUSED(userId)
    const auto reply = call(
        "requests", {{"direction", requestType == RequestType::Pending ? "incoming" : "outgoing"}});
    std::vector<Data> users;
    for (const auto &userValue : reply["result"].toObject()["users"].toArray()) {
        users.push_back(profile(userValue.toObject()));
    }
    return users;
}

std::vector<Data> Manager::fetchPendingRequests(int userId) {
    return fetchRequests(userId, RequestType::Pending);
}

std::vector<Data> Manager::fetchOutgoingRequests(int userId) {
    return fetchRequests(userId, RequestType::Outgoing);
}

QHash<Data, int> &Manager::fetchFriends(int userId, QHash<Data, int> &friends) {
    Q_UNUSED(userId)
    const auto reply = call("friends");
    if (!reply["ok"].toBool()) {
        return friends;
    }
    friends.clear();
    for (const auto &userValue : reply["result"].toObject()["users"].toArray()) {
        const auto userObject = userValue.toObject();
        friends.insert(profile(userObject), userObject["online"].toBool() ? 1 : 0);
    }
    return friends;
}

bool Manager::acceptRequest(Data requester) {
    const auto reply = call("friend_accept", {{"peer_id", requester.user_id}});
    if (reply["ok"].toBool()) {
        QTimer::singleShot(0, client, &Client::relationshipsChanged);
    }
    return reply["ok"].toBool();
}

bool Manager::declineRequest(int peerId) {
    const auto reply = call("friend_decline", {{"peer_id", peerId}});
    return reply["ok"].toBool();
}

bool Manager::cancelRequest(int peerId) {
    const auto reply = call("friend_cancel", {{"peer_id", peerId}});
    return reply["ok"].toBool();
}

int Manager::checkConversation(int ownUserId, int peerId) {
    Q_UNUSED(ownUserId)
    const auto reply = call("conversation", {{"peer_id", peerId}});
    return reply["result"].toObject()["conversation_id"].toInt(-1);
}

bool Manager::sendChatMessage(const QString &body,
                              int conversationId,
                              int peerId,
                              const QVector<Attachement> &files,
                              Data &sentData) {
    const auto preparedAttachments = AttachmentPayload::prepare(files);
    if (!preparedAttachments.ok) {
        requestError = ErrorMessages::message("send_message", preparedAttachments.errorCode, {});
        return false;
    }
    const auto reply = call("send_message",
                            {{"peer_id", peerId},
                             {"conversation_id", conversationId},
                             {"body", body},
                             {"attachments", preparedAttachments.attachments}});
    if (!reply["ok"].toBool()) {
        return false;
    }
    auto messageObject = reply["result"].toObject();
    messageObject["id"] = client->getClientID();
    messageObject["name"] = client->getClientName();
    messageObject["status"] = Utils::stringFromStatus(client->getClientStatus());
    sentData = Utils::buildData(messageObject);
    resolveDataMedia(sentData);
    return true;
}

QString Manager::attachmentPathForId(qint64 attachmentId) {
    const auto reply = call("attachment", {{"attachment_id", attachmentId}});
    if (!reply["ok"].toBool()) {
        return {};
    }
    const auto attachmentObject = reply["result"].toObject();
    return MediaCache::save(QByteArray::fromBase64(attachmentObject["bytes"].toString().toLatin1()),
                            "attachments",
                            attachmentId,
                            attachmentObject["filename"].toString());
}

void Manager::resolveDataMedia(Data &data) {
    data.avatar = avatarPathForUser(data.user_id, true);
    for (auto &attachment : data.message.attachs) {
        const QString prefix = "db://attachment/";
        if (attachment.path.startsWith(prefix)) {
            attachment.path = attachmentPathForId(attachment.path.mid(prefix.size()).toLongLong());
        } else {
            attachment.path.clear(); // Never trust a remote path to a local file.
        }
    }
}

StringContainer Manager::fetchMessages(int conversationId) {
    StringContainer messages;
    if (conversationId <= 0) {
        return messages;
    }
    int lastMessageId = 0;
    while (true) {
        const auto reply =
            call("messages", {{"conversation_id", conversationId}, {"after_id", lastMessageId}});
        if (!reply["ok"].toBool()) {
            break;
        }
        const auto messageRows = reply["result"].toObject()["messages"].toArray();
        for (const auto &messageValue : messageRows) {
            auto messageObject = messageValue.toObject();
            messageObject["id"] = messageObject["messageSender"];
            auto messageData = Utils::buildData(messageObject);
            resolveDataMedia(messageData);
            messages.push_back(messageData.message);
            lastMessageId = messageObject["message_id"].toInt();
        }
        if (messageRows.size() < 100) {
            break;
        }
    }
    return messages;
}

std::vector<ConversationSummary> Manager::fetchConversations() {
    std::vector<ConversationSummary> conversations;
    const auto reply = call("conversations");
    for (const auto &conversationValue : reply["result"].toObject()["conversations"].toArray()) {
        const auto conversationObject = conversationValue.toObject();
        ConversationSummary summary;
        summary.conversationId = conversationObject["conversation_id"].toInt();
        summary.peer = profile(conversationObject["user"].toObject());
        summary.lastMessage.body = conversationObject["body"].toString();
        summary.lastMessage.senderID = conversationObject["sender"].toInt();
        summary.lastMessage.sentIn = summary.conversationId;
        conversations.push_back(summary);
    }
    return conversations;
}
