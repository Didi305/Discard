#include "chatserver.h"
#include "qtcpsocket.h"

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
            clients.append(clientSocket);
        }
    };

    void ChatServer::readClient(){
        qDebug() << "jerebti hadchi?";
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        qDebug() << "chmen client hada?" << clientSocket;
        if(clientSocket){
            QByteArray data = clientSocket->readAll();
            qDebug() << data;
            QString message = data;
            for(QTcpSocket *c : clients){
                c->write(message.toUtf8());
            }
        }
    };

    void ChatServer::clientDisconnected(){
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        clientSocket->deleteLater();
    };




