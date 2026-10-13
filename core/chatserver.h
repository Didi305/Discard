#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHash>
#include <QJsonObject>
#include <QElapsedTimer>
class Manager;
class ChatServer : public QObject {
    Q_OBJECT
public:
    explicit ChatServer(Manager &manager, QObject *parent = nullptr);
    bool listen(const QHostAddress &address, quint16 port);
private:
    struct Session { int userId=0; QByteArray buffer; int authAttempts=0; qint64 authWindow=0; };
    Manager &manager;
    QTcpServer server;
    QHash<QTcpSocket*,Session> sessions;
    QElapsedTimer clock;
    void read(QTcpSocket *socket);
    void dispatch(QTcpSocket *socket, const QJsonObject &request);
    void send(QTcpSocket *socket, const QJsonObject &object);
    void notify(int userId, const QJsonObject &event);
    void presence(int userId, bool online);
    bool online(int userId) const;
};
