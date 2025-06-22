#include "client.h"
#include <qjsonobject.h>
#include "helpers.h"
Client::Client(QObject *parent) : QObject(parent){
    socket = new QTcpSocket(this);

    connect(socket, &QTcpSocket::connected, this, &Client::onConnected);
    connect(socket, &QTcpSocket::disconnected, this, &Client::onDisconnected);
    connect(socket, &QTcpSocket::readyRead, this, &Client::onReadyRead);
}
Client::~Client() {
    delete socket;
}

QString Client::getClientName(){
    return name;
}
QString Client::getClientAvatar(){
    return avatar;
}
int Client::getClientID(){
    return id;
}
void Client::setClientName(QString name){
    this->name = name;
}
void Client::setClientAvatar(QString avatar){
    this->avatar = avatar;
}
void Client::setClientID(int id){
    this->id = id;
}
void Client::connectToServer(const QString &host, int port){
    socket->connectToHost(host, port);
    if(socket->state() == QAbstractSocket::ConnectedState){
        socket->write("shitype");
    }
}

void Client::disconnectFromServer(){
    socket->disconnectFromHost();
}

void Client::sendMessage(const QString &message, QString typ, QString status) {
    QMap<QString, QString> dataInfo = {
        {"type" , typ},
        {"content" , message},
        {"status", status}
    };
    qDebug() << "client send message was called and the reception id is: " << message;
    QString jsonString = buildJson(dataInfo);
    socket->write(jsonString.toUtf8());

}
void Client::onConnected(){
    emit connected();
    int clientID = getClientID();
    QString id = QString::number(clientID);
    socket->write(id.toUtf8());
    qDebug() << "Connected to server";
}

void Client::onDisconnected(){
    emit disconnected();
}

void Client::onReadyRead(){
    QByteArray data = socket->readAll();
    QJsonObject obj = parseJson(data);
    QString dataType = obj["status"].toString();
    QString friend_id = obj["content"].toString();
    //maybe in the future QMap to handle multipe types of signals
    if(dataType == "update_accept"){
        emit updateAcceptRequests(friend_id);
    }
    else if(dataType == "update_pending"){
        emit updatePendingRequests(friend_id);
    }
}
