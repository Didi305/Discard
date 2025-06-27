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
    handlers["friends_status"]  = [this](QJsonObject& obj){ qDebug() << "check Friends Status is being called" ; checkFriendsStatus(obj);};
    handlers["friend_update"]  = [this](QJsonObject& obj){emit updateFriends(obj["id"].toInt(), obj["bool"].toBool());};
    handlers["update_pending"] = [this](QJsonObject& obj){emit updatePendingRequests(obj["id"].toInt());};
    handlers["update_accept"]  = [this](QJsonObject& obj){updateAcceptRequests(obj);};

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

Data Client::getClientData(){
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
void Client::setClientID(int id){
    this->id = id;
}
void Client::setFriends(QHash<Data , int> &friends){
    this->friends = friends;
}
void Client::setClientData(int id, QString name, QString avatar, QHash<Data, int> friends){
    this->d.user_id = id;
    this->d.avatar = avatar;
    this->d.displayName = name;
    this->d.friends = friends;
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
    root["type"] = "init";
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

void Client::sendMessage(Data &d, QString typ) {
    QMap<QString, QString> dataInfo = {
        {"type" , typ},
        {"id" , QString::number(d.user_id)},
        {"avatar" , d.avatar},
        {"name", d.displayName}
    };
    qDebug() << "ID of the user that needs to update his shit is: " << dataInfo["id"].toStdString();
    QString jsonString = Utils::buildJson(dataInfo);
    socket->write(jsonString.toUtf8());

}

void Client::onReadyRead(){
    QByteArray buffer;
    buffer.append(socket->readAll());
    qDebug() << "Ha buffer chno fih before processing: " << buffer;
    while(true){
        qDebug() << "getting the next available message";
        int index = buffer.indexOf('\n');
        if(index < 0) break;
        QByteArray msg = buffer.left(index);
        qDebug() << "message to be processed: " << msg;
        buffer.remove(0, index+1);
        QJsonObject obj = Utils::parseJson(msg);
        QString whatwedoing = obj["type"].toString();
        if(whatwedoing == "add_request") whatwedoing = "update_accept";
        qDebug() << "what we doing? " << whatwedoing;
        auto f = handlers.value(whatwedoing, nullptr);
        if(f) f(obj);
        }
    }



void Client::checkFriendsStatus(QJsonObject& obj){
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

void Client::updateAcceptRequests(QJsonObject& obj){
    QString type = obj["type"].toString();
    emit updateAcceptRequest(obj, type);
}



