#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>
#include "row.h"
#include "utils.h"

class Manager;

class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject *parent = nullptr);
    QString getClientName();
    QString getClientAvatar();
    int getClientID();
    QHash<Data, int>& getFriends();
    void setFriends(QHash<Data, int> &friends);
    void setClientName(QString name);
    void setClientAvatar(QString avatar);
    void setClientID(int id);
    ~Client();

    void connectToServer(const QString &host, int port);
    void disconnectFromServer();
    void sendMessage(Data &d, QString typ, QString status);
    void checkFriendsStatus(QHash<Data, int> friends);
signals:
    void connected();
    void disconnected();
    void messageReceived(QString &data);
    void updatePendingRequests(int friend_id);
    void updateAcceptRequests(Data &d, QString dataType);
    void friendsReady(QHash<Data, int> friends);
    void newFriendRequest(Data &d, QString dataType);
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

private:
    QTcpSocket *socket;
    int id;
    QString name;
    QString avatar;
    Manager* m_manager;
    QHash<Data, int> friends;
    Status status;
};

#endif // CLIENT_H
