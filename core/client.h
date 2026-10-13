#pragma once
#include <QTcpSocket>
#include <QJsonObject>
#include "row.h"
#include "utils.h"
class Manager;
class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QObject *parent=nullptr);
    ~Client() override = default;
    QString getClientName(); QString getClientAvatar(); Status getClientStatus();
    int getClientID(); Data &getClientData(); QHash<Data,int> &getFriends();
    void setFriends(QHash<Data,int>&); void setClientName(QString);
    void setClientStatus(Status); void setClientAvatar(QString); void setClientID(int);
    void setClientData(Data&);
    void setEndpoint(const QString &host,int port);
    bool ensureConnected();
    QJsonObject request(const QString &operation,const QJsonObject &args={});
    void disconnectFromServer();
    QString errorString() const { return lastError; }
signals:
    void connected(); void disconnected();
    void updatePendingRequests(int friend_id);
    void updateAcceptRequest(QJsonObject &obj,QString type);
    void updateFriends(int id,bool online);
    void updateConversations(Message &msg,Data &d);
    void friendsReady(QHash<Data,int> friends);
    void addNewFriendRow(Data d);
    void newMessage(Data d);
    void relationshipsChanged();
    void serverError(QString message);
private:
    QTcpSocket *socket;
    Manager *m_manager;
    int id=0;
    QString name,avatar;
    Status status=Status::ONLINE;
    Data d{};
    QHash<Data,int> friends;
    QString host="127.0.0.1";
    int port=41234;
    QByteArray receiveBuffer;
    QHash<QString,QJsonObject> responses;
    QList<QJsonObject> events;
    quint64 nextRequest=0;
    bool requesting=false,dispatching=false;
    QString lastError;
    void onReadyRead();
    void dispatchEvents();
};
