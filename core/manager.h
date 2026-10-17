#pragma once
#include "accountauthentication.h"
#include "client.h"
#include "utils.h"
#include "utils/conversationsummary.h"
#include <QObject>
#include <vector>
// Client facade: server requests and application data. Pages create UI rows.
class Manager : public QObject {
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);
    static Manager *get();
    void setClient(Client *client);
    Client *getClient();
    bool authenticate(const QString &username, const QString &password);
    bool secondFactorRequired() const;
    bool verifySecondFactor(const QString &code);
    void cancelAuthentication();
    QJsonObject securityRequest(const QString &operation, const QJsonObject &arguments = {});
    bool connectServer();
    QString lastError() const;
    bool checkUsernameLength(const QString &username);
    bool createUser(QString username, QString password, QString nickname);
    bool checkUsernameAvailibity(QString username);
    bool checkNicknameAvailibity(QString nickname);
    bool updateUserAvatar(QString path, QString nickname);
    QString avatarPathForUser(int userId, bool reuseCached = false);
    void resolveDataMedia(Data &data);
    QString checkFriendship(QString nickname);
    std::vector<Data> fetchRequests(int userId, RequestType requestType);
    QHash<Data, int> &fetchFriends(int userId, QHash<Data, int> &friends);
    std::vector<Data> fetchPendingRequests(int userId);
    std::vector<Data> fetchOutgoingRequests(int userId);
    bool acceptRequest(Data requester);
    bool declineRequest(int peerId);
    bool cancelRequest(int peerId);
    int checkConversation(int ownUserId, int peerId);
    bool sendChatMessage(const QString &body,
                         int conversationId,
                         int peerId,
                         const QVector<Attachement> &files,
                         Data &sentData);
    StringContainer fetchMessages(int conversationId);
    std::vector<ConversationSummary> fetchConversations();

private:
    Client *client = nullptr;
    AccountAuthentication accountAuthentication;
    QHash<int, QString> avatarPaths;
    QString requestError;
    QJsonObject call(const QString &operation, const QJsonObject &arguments = {});
    bool finishAuthentication(const QJsonObject &userObject);
    Data profile(const QJsonObject &userObject);
    QString
    avatarPathFromValues(int userId, const QByteArray &avatarBytes, const QString &filename);
    QString attachmentPathForId(qint64 attachmentId);
};
