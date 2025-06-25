#ifndef CHATSERVER_H
#define CHATSERVER_H
#include <QHash>
#include <QMap>
#include <QTcpServer>
#include <functional>
class ChatServer : public QObject {
    Q_OBJECT
public:
    ChatServer(QObject *parent = nullptr);
    ~ChatServer();
    void updateRequest(QJsonObject &obj, QTcpSocket* socket);
    void updateFriends(QJsonObject &obj, QTcpSocket* socket);
signals:
    void messageReceived();
    
public slots:
    void onNewConnection();
    
    void readClient();
    
    void clientDisconnected();
private:
    QTcpServer *tcpServer;
    QHash<QTcpSocket*, int> clients;
    QHash<int, QTcpSocket*> clientsR;
    QHash<QString, std::function<void(QJsonObject &, QTcpSocket*)>> handlers;
};


#endif
