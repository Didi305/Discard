#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <qobject.h>
#include "row.h"
#include <vector>
#include "utils.h"
#include <qsqldatabase.h>
#include "client.h"
#include "pendingrow.h"
#include "outgoingrow.h"
#include "friendrow.h"
struct DBConfig{
    QString host;
    int port;
    QString database;
    QString user;
    QString password;
};
enum class requestType {
    amigo,
    user
};
class Manager : public QObject {
    Q_OBJECT
public:
    Manager(QObject *parent = nullptr);
    static Manager* get();
    void setClient(Client *client);
    Client* getClient();
    QSqlDatabase& db();
    /*---------------General-----------------*/
    bool init(QJsonObject &DBConfig, QSqlDatabase &db);
    bool createUser(QString username, QString password, QString nickname);
    QString checkUser(const QString& username, const QString& password = QString());
    bool checkUsernameAvailibity(QString username);
    bool checkNicknameAvailibity(QString nickname);
    void updateUserAvatar(QString path, QString nickname);
    void findNickname(QString username);
    /*--------------Friends------------------*/
    QString checkFriendship(QString nickname);
    std::vector<std::unique_ptr<Row>> fetchRequests(int id, RequestType type);
    QHash<Data , int>& fetchFriends(int user_id, QHash<Data , int>& friends);
    std::vector<std::unique_ptr<Row>> fetchPendingRequests(int userId);
    std::vector<std::unique_ptr<Row>> fetchOutgoingRequests(int userId);
    void acceptRequest(Data d);
    void declineRequest(int user_id);
    void cancelRequest(int user_id);
    /*---------------DM's--------------------*/
    int createConversation(int user1_ID, int user2_ID);
    int checkConversation(int user1_ID, int user2_ID);
    void postMessage(QString message, int conversation_ID, int sender_ID);
    StringContainer fetchMessages(int conversationID);
private:
    Manager() = default;
    ~Manager() = default;
    QSqlDatabase database;
    Client *client;
};

#endif // DATABASEMANAGER_H
