#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <QHash>
#include <QMap>
#include <QTcpServer>
#include <functional>
#include <QObject>
#include <QHash>
#include <QSet>
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <functional>
class ChatServer : public QObject {
    Q_OBJECT
public:
    ChatServer(QObject *parent = nullptr);
    ~ChatServer();
    void updateRequest(QJsonObject &obj, QTcpSocket* socket);
    void init(QJsonObject &obj, QTcpSocket* socket);
    void checkUserStatus(QJsonObject &obj, QTcpSocket* socket);
    void sendMessage(QJsonObject &obj, QTcpSocket* socket);
signals:
    void messageReceived();
    void userStatus(int id, bool online);
public slots:
    void onNewConnection();
    
    void readClient();
    
    void clientDisconnected();
private:
    QTcpServer *tcpServer;
    QHash<QTcpSocket*, int> clients;
    QHash<int, QTcpSocket*> clientsR;
    QHash<QString, std::function<void(QJsonObject &, QTcpSocket*)>> handlers;
    QSet<QString> updateFriendshipTypes = {
        "update_pending",
        "update_accept",
        "add_request",
        "add_row"
    };
};


#endif
