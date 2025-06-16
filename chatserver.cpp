#include "chatserver.h"
#include "qtcpsocket.h"
#include "client.h"
ChatServer::ChatServer(QObject *parent)
    : QObject(parent)
{
    tcpServer = new QTcpServer(this);

    connect(tcpServer, &QTcpServer::newConnection, this, &ChatServer::onNewConnection);
    tcpServer->listen(QHostAddress::Any, 1234);
}
ChatServer::~ChatServer() {
    delete tcpServer;
}

    void ChatServer::onNewConnection() {
        while(tcpServer->hasPendingConnections()){
            QTcpSocket *clientSocket = tcpServer->nextPendingConnection();
            connect(clientSocket, &QTcpSocket::readyRead, this, &ChatServer::readClient);
            connect(clientSocket, &QTcpSocket::disconnected, this, &ChatServer::clientDisconnected);
            clients.insert(clientSocket, "");
        }
    };

    void ChatServer::readClient(){
        qDebug() << "jerebti hadchi?";
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        QByteArray data = clientSocket->readAll();
        qDebug() << data;
        qDebug() << "chmen client hada?" << clientSocket;
        QString nickname = clients.value(clientSocket);
        if(nickname.isEmpty()){
            QString userNickname = data;
            clients[clientSocket] = userNickname;
            QString announcement = userNickname + " joined the server";
            for(auto *c : clients.keys()){
                c->write(announcement.toUtf8());
            }
        }
        else{
            QString message = data;
            for(auto *c : clients.keys()){
                c->write(message.toUtf8());
            }
        }
    };

    void ChatServer::clientDisconnected(){
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        qDebug() << "deleting Socket: " << clientSocket;
        clients.remove(clientSocket);
        clientSocket->deleteLater();
    };




