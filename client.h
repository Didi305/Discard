#ifndef CLIENT_H
#define CLIENT_H
#include <QTcpSocket>
#include "row.h"
class Manager;

class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject *parent = nullptr);
    QString getClientName();
    QString getClientAvatar();
    int getClientID();
    std::vector<Data> getFriends();
    void setFriends(std::vector<Data> friends);
    void setClientName(QString name);
    void setClientAvatar(QString avatar);
    void setClientID(int id);
    ~Client();

    void connectToServer(const QString &host, int port);
    void disconnectFromServer();
    void sendMessage(Data &d, QString typ, QString status);

signals:
    void connected();
    void disconnected();
    void messageReceived(QString &data);
    void updatePendingRequests(int friend_id);
    void updateAcceptRequests(Data &d, QString dataType);
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
    std::vector<Data> friends;
};

#endif // CLIENT_H
