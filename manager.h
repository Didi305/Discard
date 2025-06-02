#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <qobject.h>
#include <qsqldatabase.h>
struct DBConfig{
    QString host;
    int port;
    QString database;
    QString user;
    QString password;
};

class Manager : public QObject {
    Q_OBJECT
public:
    Manager(QObject *parent = nullptr);
    static Manager* get();
    QSqlDatabase& db();
    bool init(QJsonObject &DBConfig, QSqlDatabase &db);
    bool createUser(QString username, QString password, QString nickname);
    QString checkUser(QString username, QString password);
    bool checkUsernameAvailibity(QString username);
    bool checkNicknameAvailibity(QString nickname);
    void updateUserAvatar(QString path, QString nickname);
    void findNickname(QString username);
    void postMessage(QString message);
private:
    Manager() = default;
    ~Manager() = default;
    QSqlDatabase database;
};

#endif // DATABASEMANAGER_H
