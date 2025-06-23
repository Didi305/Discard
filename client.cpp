#include "client.h"
#include "manager.h"
#include <qjsonobject.h>
#include "helpers.h"
Client::Client(QObject *parent)
    : QObject(parent)
    , m_manager(Manager::get())
{

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

std::vector<Data> Client::getFriends(){
    return friends;
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

void Client::setFriends(std::vector<Data> friends){
    this->friends = friends;
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

void Client::sendMessage(Data &d, QString typ, QString status) {
    QMap<QString, QString> dataInfo = {
        {"type" , typ},
        {"status", status},
        {"id" , QString::number(d.user_id)},
        {"avatar" , d.avatar},
        {"nickname" , d.displayName}
    };
    qDebug() << "ID of the user that needs to update his shit is: " << dataInfo["id"].toStdString();
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
    //initiate the data with a method maybe?
    Data d;
    d.avatar = obj["avatar"].toString();
    d.displayName = obj["nickname"].toString();
    d.user_id = obj["id"].toInt();
    qDebug() << "Client ID: " << getClientID() << " needs to update the request of client ID: " << d.user_id;
    qDebug() << "update type: " << dataType;
    //maybe in the future QMap to handle multipe types of signals
    if(dataType == "update_accept" || dataType == "add_request"){
        emit updateAcceptRequests(d, dataType);
    }
    else if(dataType == "update_pending"){
        emit updatePendingRequests(d.user_id);
    }
}
