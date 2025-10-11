#include "client.h"
#include "manager.h"
#include <QJsonObject>
#include <QJsonArray>

Client::Client(QObject *parent)
    : QObject(parent)
    , m_manager(Manager::get())
    , server(new ChatServer)
{

    socket = new QTcpSocket(this);
    handlers["message"] = [this](QJsonObject &obj){
        Data d = Utils::buildData(obj);
        Message msg = d.message;
        emit newMessage(d); emit updateConversations(msg, d);
    };
    handlers["friends_status"]  = [this](QJsonObject& obj){checkFriendsStatus(obj);};
    handlers["user_status"] = [this](QJsonObject& obj){
        Data d = {obj["id"].toInt(), obj["name"].toString(), obj["avatar"].toString(), Utils::statusFromString(obj["status"].toString())};
        friends[d] = obj["bool"].toInt();
        emit addNewFriendRow(d);
    };
    handlers["friend_update"]  = [this](QJsonObject& obj){emit updateFriends(obj["id"].toInt(), obj["bool"].toBool());};
    handlers["update_pending"] = [this](QJsonObject& obj){emit updatePendingRequests(obj["id"].toInt());};
    handlers["update_accept"]  = [this](QJsonObject& obj){updateAcceptRequests(obj);};
    handlers["add_row"] = [this](QJsonObject& obj){
        Data d = {obj["id"].toInt(), obj["name"].toString(), obj["avatar"].toString(), Utils::statusFromString(obj["status"].toString())};
        qCritical() << "status of the friend to be added is: " << Utils::stringFromStatus(d.status);
        friends.emplace(d, 1);
        emit addNewFriendRow(d);};

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

Status Client::getClientStatus(){
    return status;
}

Data& Client::getClientData(){
    return d;
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

void Client::setClientStatus(Status stat){
    this->status = stat;
}
void Client::setClientID(int id){
    this->id = id;
}
void Client::setFriends(QHash<Data , int> &friends){
    this->friends = friends;
}
void Client::setClientData(Data& d){
    this->d = d;
}
void Client::connectToServer(const QString &host, int port){
    socket->connectToHost(host, port);
    if(socket->state() == QAbstractSocket::ConnectedState){
        socket->write("shitype");
    }
}

void Client::disconnectFromServer(){
    friends.clear();
    socket->disconnectFromHost();
}

void Client::onConnected(){
    emit connected();
    QJsonObject root;
    QJsonArray arr;
    root["type"] = "init";
    root["user_id"] = getClientID();
    auto amigos = friends.keys();
    for(auto &f : amigos){
        arr.append(f.user_id);
    }
    root["friends"] = arr;
    QJsonDocument doc(root);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    jsonString.append('\n');
    qDebug() << "Connected to server";
    socket->write(jsonString.toUtf8());
}

void Client::onDisconnected(){
    emit disconnected();
}

void Client::sendMessage(Data &d, QString typ) {
    QJsonObject obj = Utils::buildJson(d);
    obj["type"] = typ;
    QJsonDocument doc(obj);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    jsonString.append('\n');
    socket->write(jsonString.toUtf8());

}

void Client::onReadyRead(){
    QByteArray buffer;
    buffer.append(socket->readAll());
    while(true){
        int index = buffer.indexOf('\n');
        if(index < 0) break;
        QByteArray msg = buffer.left(index);
        buffer.remove(0, index+1);
        QJsonObject obj = Utils::parseJson(msg);
        QString whatwedoing = obj["type"].toString();
        if(whatwedoing == "add_request") whatwedoing = "update_accept";
        auto f = handlers.value(whatwedoing, nullptr);
        if(f) f(obj);
        }
    }



void Client::checkFriendsStatus(QJsonObject& obj){
    auto& friends = getFriends();
    auto friendsStatus = obj["friends"].toArray();
    int i = 0;
    for(auto it = friends.begin(); it != friends.end(); ++it){
        int friendStatus = friendsStatus.at(i).toInt();
        it.value() = friendStatus;
        i++;
    }
    setFriends(friends);
    emit friendsReady(friends);
}

void Client::updateAcceptRequests(QJsonObject& obj){
    QString type = obj["type"].toString();
    emit updateAcceptRequest(obj, type);
}



