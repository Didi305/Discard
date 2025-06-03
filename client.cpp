#include "client.h"
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
}

void Client::disconnectFromServer(){
    socket->disconnectFromHost();
}

void Client::sendMessage(const QString &message) {
    if (socket->state() == QAbstractSocket::ConnectedState) {
        socket->write(message.toUtf8());
    }
}
void Client::onConnected(){
    emit connected();
    QString clientName = getClientName();
    socket->write(clientName.toUtf8());
    qDebug() << "Connected to server";
}

void Client::onDisconnected(){
    emit disconnected();
}

void Client::onReadyRead(){
    QString message = socket->readAll();
    emit messageReceived(message);
}
