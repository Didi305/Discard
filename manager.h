#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <qobject.h>
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
    bool init(QJsonObject &DBConfig);
    void createUser(QString username, QString password);
    void checkUser(QString username, QString password);
    void postMessage(QString message);
private:
    Manager() = default;
    ~Manager() = default;
};

#endif // DATABASEMANAGER_H
