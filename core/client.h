#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>
#include "row.h"
#include "utils.h"
#include "chatserver.h"
class Manager;

class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject *parent = nullptr);
    QString getClientName();
    QString getClientAvatar();
    Status getClientStatus();
    int getClientID();
    Data &getClientData();
    QHash<Data, int>& getFriends();
    void setFriends(QHash<Data, int> &friends);
    void setClientName(QString name);
    void setClientStatus(Status stat);
    void setClientAvatar(QString avatar);
    void setClientID(int id);
    void setClientData(Data& d);
    ~Client();

    void connectToServer(const QString &host, int port);
    void disconnectFromServer();
    void sendMessage(Data &d, QString typ);
    void checkFriendsStatus(QJsonObject& obj);
    void updateAcceptRequests(QJsonObject& obj);
signals:
    void connected();
    void disconnected();
    void messageReceived(QString &data);
    void updatePendingRequests(int friend_id);
    void updateAcceptRequest(QJsonObject& obj, QString type);
    void updateFriends(int id, bool online);
    void updateConversations(Message& msg, Data& d);
    void friendsReady(QHash<Data, int> friends);
    void addNewFriendRow(Data d);
    void newMessage(Data d);
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

private:
    QTcpSocket *socket;
    ChatServer* server;
    int id;
    QString name;
    QString avatar;
    Manager* m_manager;
    QHash<Data, int> friends;
    Data d;
    Status status;
    QHash<QString, std::function<void(QJsonObject&)>> handlers;
};

#endif // CLIENT_H
