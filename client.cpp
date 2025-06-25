#include "client.h"
#include "manager.h"
#include <QJsonObject>
#include <QJsonArray>
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
    int clientID = getClientID();
    QString id = QString::number(clientID);
    socket->write(id.toUtf8());
    qDebug() << "Connected to server";
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
        {"nickname" , d.displayName}
    };
    qDebug() << "ID of the user that needs to update his shit is: " << dataInfo["id"].toStdString();
    QString jsonString = buildJson(dataInfo);
    socket->write(jsonString.toUtf8());

}

void Client::checkFriendsStatus(QHash<Data, int> friends){
    QJsonObject root;
    QJsonArray arr;
    root["type"] = "friends_status";
    for(auto it = friends.constBegin(); it != friends.constEnd(); ++it){
        arr.append(it.key().user_id);
    }
    root["friends"] = arr;
    QJsonDocument doc(root);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    socket->write(jsonString.toUtf8());
}
void Client::onReadyRead(){
    QByteArray data = socket->readAll();
    if(data == "mre7ba"){
        setFriends(m_manager->fetchFriends(getClientID(), getFriends()));
        return;
    }
    QJsonObject obj = parseJson(data);
    QString dataType = obj["status"].toString();
    //initiate the data with a method maybe?
    Data d;
    d.avatar = obj["avatar"].toString();
    d.displayName = obj["nickname"].toString();
    d.user_id = obj["id"].toInt();
    //maybe in the future QHash to handle multipe types of signals
    if(dataType == "update_accept" || dataType == "add_request"){
        emit updateAcceptRequests(d, dataType);
    }
    else if(dataType == "update_pending"){
        emit updatePendingRequests(d.user_id);
    }
    else if(dataType == "friends_status"){
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
        emit connected();
    }
}


