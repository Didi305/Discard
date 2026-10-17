#pragma once
#include <QElapsedTimer>
#include <QHash>
#include <QJsonObject>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Manager;

class ChatServer : public QObject {
    Q_OBJECT
public:
    explicit ChatServer(Manager &serverManager, QObject *parent = nullptr);
    bool listen(const QHostAddress &address, quint16 port);

private:
    struct Session {
        int userId = 0;
        QByteArray buffer;
        int authAttempts = 0;
        qint64 authWindow = 0;
    };

    Manager &manager;
    QTcpServer server;
    QHash<QTcpSocket *, Session> sessions;
    QElapsedTimer clock;

    void read(QTcpSocket *socket);
    void dispatch(QTcpSocket *socket, const QJsonObject &request);
    void send(QTcpSocket *socket, const QJsonObject &jsonObject);
    void notify(int userId, const QJsonObject &event);
    void presence(int userId, bool isOnline);
    bool online(int userId) const;
};
