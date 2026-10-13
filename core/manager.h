#pragma once
#include <QObject>
#include <QFileInfo>
#include "attachementchip.h"
#include "conversationrow.h"
#include "row.h"
#include <vector>
#include "utils.h"
#include "client.h"
#include "pendingrow.h"
#include "outgoingrow.h"
#include "friendrow.h"
// Client facade: RPC, UI rows and local media cache only. No database connection.
class Manager : public QObject {
    Q_OBJECT
public:
    explicit Manager(QObject *parent=nullptr);
    static Manager *get();
    void setClient(Client *client);
    Client *getClient();
    bool authenticate(const QString &username,const QString &password);
    bool connectServer();
    QString lastError() const;
    bool createUser(QString username,QString password,QString nickname);
    bool checkUsernameAvailibity(QString username);
    bool checkNicknameAvailibity(QString nickname);
    bool updateUserAvatar(QString path,QString nickname);
    QString avatarPathForUser(int userId,bool reuseCached=false);
    void resolveDataMedia(Data &data);
    QString checkFriendship(QString nickname);
    std::vector<std::unique_ptr<Row>> fetchRequests(int id,RequestType type);
    QHash<Data,int>& fetchFriends(int userId,QHash<Data,int>& friends);
    std::vector<std::unique_ptr<Row>> fetchPendingRequests(int id);
    std::vector<std::unique_ptr<Row>> fetchOutgoingRequests(int id);
    bool acceptRequest(Data data);
    bool declineRequest(int id);
    bool cancelRequest(int id);
    int checkConversation(int ownId,int peerId);
    bool sendChatMessage(const QString &body,int conversationId,int peerId,const QVector<Attachement>& files,Data &sent);
    StringContainer fetchMessages(int conversationId);
    std::vector<std::unique_ptr<ConversationRow>> fetchConversations(Data &data);
private:
    Client *client=nullptr;
    QHash<int,QString> avatarPaths;
    QJsonObject call(const QString &op,const QJsonObject &args={});
    Data profile(const QJsonObject &object);
    QString cacheMedia(const QByteArray&,const QString&,qint64,const QString&) const;
    QString avatarPathFromValues(int,const QByteArray&,const QString&);
    QString attachmentPathForId(qint64 id);
};
