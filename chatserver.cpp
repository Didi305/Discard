#include "chatserver.h"
#include "helpers.h"
#include "qtcpsocket.h"

#include <qjsondocument.h>
#include <qjsonobject.h>
ChatServer::ChatServer(QObject *parent)
    : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    handlers["friend_request"] = [this](QJsonObject &obj, QTcpSocket* socket){ updateRequest(obj, socket);};
    handlers["friends_status"] = [this](QJsonObject &obj, QTcpSocket* socket){ updateFriends(obj, socket);};
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
            clients.insert(clientSocket, 0);
            qDebug() << "New User just connected: " << clientSocket;
        }
    };



    void ChatServer::readClient(){
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        QByteArray data = clientSocket->readAll();
        QJsonObject obj = parseJson(data);

        QString dataType = obj["type"].toString();
        if(dataType.isEmpty()){
            clients[clientSocket] = data.toInt();
            qDebug() << "client ID received by the server" << clients[clientSocket];
            return;
        }
        for(auto it = handlers.constBegin(); it != handlers.constEnd(); ++it){
            if(it.key() == dataType){
                it.value()(obj, clientSocket);
            }
        }
    };

    void ChatServer::updateRequest(QJsonObject &obj, QTcpSocket* socket){
        int receiver_id = obj["id"].toString().toInt();
        qDebug() << "the guy that needs to update his shit from server POV: " << receiver_id;
        QTcpSocket* foundKey = nullptr;
        for (auto it = clients.constBegin(); it != clients.constEnd(); ++it) {
            if (it.value() == receiver_id) {
                foundKey = it.key();
                break;
            }
        }
        if (foundKey != nullptr) {
            obj["id"] = clients[socket];
            qDebug() << "and he has to update the request of the client ID1: " << clients[socket];;
            qDebug() << "and he has to update the request of the client ID2: " << obj["id"].toInt();
            QJsonDocument doc(obj);
            QString jsonString = doc.toJson(QJsonDocument::Compact);
            foundKey->write(jsonString.toUtf8());
        }
    }
    void ChatServer::updateFriends(QJsonObject &obj, QTcpSocket* socket){

    }

    void ChatServer::clientDisconnected(){
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        clients.remove(clientSocket);
        clientSocket->deleteLater();
    };




