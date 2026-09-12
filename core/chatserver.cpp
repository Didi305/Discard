#include "chatserver.h"
#include "qtcpsocket.h"
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include "serverprotocol.h"
ChatServer::ChatServer(QObject *parent)
    : QObject(parent)
{
    tcpServer = new QTcpServer(this);
    handlers["friend_request"] = [this](QJsonObject &obj, QTcpSocket* socket){ updateRequest(obj, socket);};
    handlers["init"] = [this](QJsonObject &obj, QTcpSocket* socket){ init(obj, socket);};
    handlers["user_status"] = [this](QJsonObject &obj, QTcpSocket* socket){ checkUserStatus(obj, socket);};
    handlers["message"] = [this](QJsonObject &obj, QTcpSocket *socket){sendMessage(obj, socket);};
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
            qDebug() << "Users already connected: " << clientsR;
        }
    };
    void ChatServer::clientDisconnected(){
        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        if (!clientSocket) return; // A socket being destroyed may no longer cast to QTcpSocket.
        for(auto it = clients.constBegin(); it != clients.constEnd(); ++it){
            it.key()->write(ServerProtocol::updateStatus(clients[clientSocket], false));
        }
        qDebug() << "Client disconnected: " << clientSocket;
        int client_ID = clients[clientSocket];
        clients.remove(clientSocket);
        clientsR.remove(client_ID);
        clientSocket->deleteLater();
    };



    void ChatServer::readClient(){

        QTcpSocket* clientSocket = qobject_cast<QTcpSocket*>(sender());
        while(clientSocket->canReadLine()){
            QByteArray data = clientSocket->readLine().trimmed();
            QJsonObject obj = ServerProtocol::parseJson(data);
            qDebug() << "message li ja l server: " << obj["messageBody"].toString();
            QString dataType = obj["type"].toString();
            if(updateFriendshipTypes.contains(dataType)) dataType = "friend_request";
            for(auto it = handlers.constBegin(); it != handlers.constEnd(); ++it){
                if(it.key() == dataType){
                    it.value()(obj, clientSocket);
                }
            }
        }
    };


    void ChatServer::init(QJsonObject &obj, QTcpSocket* socket){
        for(auto it = clients.constBegin(); it != clients.constEnd(); ++it){
            auto payload = ServerProtocol::updateStatus(obj["user_id"].toInt(), true);
            it.key()->write(payload);
        }
        clients[socket] = obj["user_id"].toInt();
        clientsR.insert(obj["user_id"].toInt(), socket);
        QJsonArray status;
        for(auto fID : obj["friends"].toArray()){
            const auto friendSocket = clientsR.value(fID.toInt(), nullptr);
            if(friendSocket && friendSocket->state() == QAbstractSocket::ConnectedState){
                status.append(1);
            }
            else {
                status.append(0);
            }
        }
        QJsonObject reply;
        reply["type"]  = "friends_status";
        reply["friends"] = status;
        QJsonDocument doc(reply);
        QByteArray jsonString = doc.toJson(QJsonDocument::Compact);
        jsonString.append('\n');
        socket->write(jsonString);
    }

    void ChatServer::updateRequest(QJsonObject &obj, QTcpSocket* socket){
        int receiver_id = obj["id"].toInt();
        QString type = obj["type"].toString();
        QTcpSocket* foundKey = nullptr;
        if(clientsR.contains(receiver_id)){
            foundKey = clientsR[receiver_id];
        }
        if (foundKey != nullptr) {
            obj["id"] = clients[socket];
            QJsonDocument doc(obj);
            QString jsonString = doc.toJson(QJsonDocument::Compact);
            jsonString.append('\n');
            foundKey->write(jsonString.toUtf8());
        }
    }

    void ChatServer::checkUserStatus(QJsonObject &obj, QTcpSocket* socket){
        int requestID = obj["id"].toInt();
        const auto friendSocket = clientsR.value(requestID, nullptr);
        obj["bool"] = friendSocket && friendSocket->state() == QAbstractSocket::ConnectedState ? 1 : 0;
        QJsonDocument doc(obj);
        QString jsonString = doc.toJson(QJsonDocument::Compact);
        jsonString.append('\n');
        socket->write(jsonString.toUtf8());
    }

    void ChatServer::sendMessage(QJsonObject &obj, QTcpSocket* socket){
        int receiverID = obj["id"].toInt();
        // operator[] would insert an empty entry for an offline recipient,
        // making later presence checks mistake that user for a connected one.
        auto receiverSocket = clientsR.value(receiverID, nullptr);
        if (!receiverSocket || receiverSocket->state() != QAbstractSocket::ConnectedState) return;
        obj["id"] = clients[socket];
        receiverSocket->write(ServerProtocol::jsonObjectToData(obj));
    }




