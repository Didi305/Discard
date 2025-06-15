#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <qobject.h>
#include "requestrow.h"
#include <vector>
#include <utility>
#include <qsqldatabase.h>
#include "client.h"
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
    bool init(QJsonObject &DBConfig, QSqlDatabase &db);
    bool createUser(QString username, QString password, QString nickname);
    QString checkUser(const QString& username, const QString& password = QString());
    bool checkUsernameAvailibity(QString username);
    bool checkNicknameAvailibity(QString nickname);
    QString checkFriendship(QString nickname);
    void updateUserAvatar(QString path, QString nickname);
    void findNickname(QString username);
    void postMessage(QString message);
    std::vector<std::unique_ptr<RequestRow>> fetchRequests(int id, requestType type);
private:
    Manager() = default;
    ~Manager() = default;
    QSqlDatabase database;
    Client *client;
};

#endif // DATABASEMANAGER_H
