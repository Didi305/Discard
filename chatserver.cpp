#include "chatserver.h"
#include "helpers.h"
#include "qtcpsocket.h"

#include <qjsonarray.h>
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
            clientsR.insert(data.toInt(), clientSocket);
            clientSocket->write("mre7ba");
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
        QTcpSocket* foundKey = nullptr;
        for (auto it = clients.constBegin(); it != clients.constEnd(); ++it) {
            if (it.value() == receiver_id) {
                foundKey = it.key();
                break;
            }
        }
        if (foundKey != nullptr) {
            obj["id"] = clients[socket];
            QJsonDocument doc(obj);
            QString jsonString = doc.toJson(QJsonDocument::Compact);
            foundKey->write(jsonString.toUtf8());
        }
    }
    void ChatServer::updateFriends(QJsonObject &obj, QTcpSocket* socket){
        auto friends = obj["friends"].toArray();
        for(auto f : friends){
            int friend_ID = f.toInt();
            if(clientsR.contains(friend_ID)){
                f = 1;
            }
            else {
                f = 0;
            }
        }
        //Probably needs optimization
        obj["status"] = "friends_status";
        obj["friends"] = friends;
        QJsonDocument doc(obj);
        QString qjsonString = doc.toJson(QJsonDocument::Compact);
        socket->write(qjsonString.toUtf8());
    }

    void ChatServer::clientDisconnected(){
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        clients.remove(clientSocket);
        clientSocket->deleteLater();
    };




