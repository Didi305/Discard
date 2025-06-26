#include "client.h"
#include "manager.h"
#include <QJsonObject>
#include <QJsonArray>

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

QHash<Data, int>& Client::getFriends(){
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

void Client::setFriends(QHash<Data , int> &friends){
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

void Client::onConnected(){
    QJsonObject root;
    QJsonArray arr;
    root["type"] = "";
    root["user_id"] = getClientID();
    auto amigos = friends.keys();
    for(auto &f : amigos){
        arr.append(f.user_id);
    }
    root["friends"] = arr;
    QJsonDocument doc(root);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    qDebug() << "Connected to server";
    socket->write(jsonString.toUtf8());
}

void Client::onDisconnected(){
    emit disconnected();
}

void Client::sendMessage(Data &d, QString typ, QString status) {
    QMap<QString, QString> dataInfo = {
        {"type" , typ},
        {"status", status},
        {"id" , QString::number(d.user_id)},
        {"avatar" , d.avatar},
    };
    qDebug() << "ID of the user that needs to update his shit is: " << dataInfo["id"].toStdString();
    QString jsonString = Utils::buildJson(dataInfo);
    socket->write(jsonString.toUtf8());

}

void Client::checkFriendsStatus(QHash<Data, int> friends){

}
void Client::onReadyRead(){
    QByteArray data = socket->readAll();
    QJsonObject obj = Utils::parseJson(data);
    QString dataType = obj["status"].toString();
    if(dataType == "friends_status"){
        auto& friends = getFriends();
        auto friendsStatus = obj["friends"].toArray();
        qDebug() << "aysewer meni ha zeb" <<friendsStatus;
        int i = 0;
        for(auto it = friends.begin(); it != friends.end(); ++it){
            int friendStatus = friendsStatus.at(i).toInt();
            it.value() = friendStatus;
            i++;
        }
        setFriends(friends);
        qDebug() << "friends yalah jaw";
        emit friendsReady(friends);
    }
    //initiate the data with a method maybe?
    Data d;
    d.avatar = obj["avatar"].toString();
    d.displayName = obj["nickname"].toString();
    d.user_id = obj["id"].toInt();
    //maybe in the future QHash to handle multipe types of signals
    if(dataType == "update_pending"){
        emit updatePendingRequests(d.user_id);
    }
    else if(dataType == "update_accept" || dataType == "add_request"){
        emit updateAcceptRequests(d, dataType);
    }
}


