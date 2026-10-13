#include "client.h"
#include "manager.h"
#include <QJsonArray>
#include <QElapsedTimer>
#include <QTimer>
Client::Client(QObject *parent):QObject(parent),socket(new QTcpSocket(this)),m_manager(Manager::get()) {
    connect(socket,&QTcpSocket::readyRead,this,&Client::onReadyRead);
    connect(socket,&QTcpSocket::connected,this,&Client::connected);
    connect(socket,&QTcpSocket::disconnected,this,[this] {
        id=0; friends.clear();receiveBuffer.clear();events.clear();
        emit disconnected();
    });
}
QString Client::getClientName(){return name;}
QString Client::getClientAvatar(){return avatar;}
Status Client::getClientStatus(){return status;}
int Client::getClientID(){return id;}
Data &Client::getClientData(){return d;}
QHash<Data,int> &Client::getFriends(){return friends;}
void Client::setFriends(QHash<Data,int> &v){friends=v;}
void Client::setClientName(QString v){name=v;}
void Client::setClientAvatar(QString v){avatar=v;}
void Client::setClientStatus(Status v){status=v;}
void Client::setClientID(int v){id=v;}
void Client::setClientData(Data &v){d=v;}
void Client::setEndpoint(const QString &h,int p){host=h;port=p;}
bool Client::ensureConnected() {
    if(socket->state()==QAbstractSocket::ConnectedState)return true;
    socket->abort();socket->connectToHost(host,quint16(port));
    if(socket->waitForConnected(5000))return true;
    lastError="Cannot connect to ChatServer.";return false;
}
void Client::disconnectFromServer(){socket->abort();id=0;friends.clear();responses.clear();events.clear();}
QJsonObject Client::request(const QString &operation,const QJsonObject &args) {
    if(requesting) {lastError="A request is already in progress.";return {};}
    if(!ensureConnected())return {};
    requesting=true;
    const auto rid=QString::number(++nextRequest);
    const auto payload=QJsonDocument(QJsonObject{{"type",operation},{"request_id",rid},{"args",args}}).toJson(QJsonDocument::Compact)+'\n';
    if(payload.size()>16*1024*1024) {requesting=false;lastError="Request is too large.";return {};}
    socket->write(payload);socket->flush();
    QElapsedTimer elapsed;elapsed.start();
    while(!responses.contains(rid) && socket->state()==QAbstractSocket::ConnectedState && elapsed.elapsed()<10000)
        socket->waitForReadyRead(int(10000-elapsed.elapsed()));
    requesting=false;
    if(!responses.contains(rid)) {
        socket->abort(); lastError="Server did not reply. Reconnect and check whether the operation completed.";
        emit serverError(lastError);return {};
    }
    const auto response=responses.take(rid);
    if(!response["ok"].toBool()) {
        lastError=response["error"].toString("Request failed.");emit serverError(lastError);return response;
    }
    lastError.clear();
    QTimer::singleShot(0,this,&Client::dispatchEvents);
    return response;
}
void Client::onReadyRead() {
    receiveBuffer+=socket->readAll();
    while(true) {
        auto end=receiveBuffer.indexOf('\n');
        if(end<0) {if(receiveBuffer.size()>16*1024*1024)socket->abort();break;}
        if(end>16*1024*1024){socket->abort();break;}
        const auto line=receiveBuffer.left(end);receiveBuffer.remove(0,end+1);
        QJsonParseError error;const auto doc=QJsonDocument::fromJson(line,&error);
        if(error.error!=QJsonParseError::NoError||!doc.isObject()){socket->abort();break;}
        const auto o=doc.object();
        if(o["type"]=="response") {
            if(requesting && o["request_id"].toString()==QString::number(nextRequest))responses.insert(o["request_id"].toString(),o);
        } else {events.append(o);if(events.size()>1000){socket->abort();break;}}
    }
    if(!requesting)QTimer::singleShot(0,this,&Client::dispatchEvents);
}
void Client::dispatchEvents() {
    if(requesting||dispatching)return;
    dispatching=true;
    while(!events.isEmpty() && !requesting && id>0) {
        auto o=events.takeFirst();const auto type=o["type"].toString();
        if(type=="message") {
            Data data=Utils::buildData(o);m_manager->resolveDataMedia(data);
            Message msg=data.message;emit newMessage(data);emit updateConversations(msg,data);
        } else if(type=="friend_update") emit updateFriends(o["id"].toInt(),o["bool"].toBool());
        else if(type=="relationships_changed") emit relationshipsChanged();
    }
    dispatching=false;
}
