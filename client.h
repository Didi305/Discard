#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject *parent = nullptr);
    QString getClientName();
    QString getClientAvatar();
    int getClientID();
    void setClientName(QString name);
    void setClientAvatar(QString avatar);
    void setClientID(int id);
    ~Client();

    void connectToServer(const QString &host, int port);
    void disconnectFromServer();
    void sendMessage(const QString &message, QString typ, QString status);

signals:
    void connected();
    void disconnected();
    void messageReceived(QString &data);
    void updatePendingRequests(QString friend_id);
    void updateAcceptRequests(QString friend_id);
private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

private:
    QTcpSocket *socket;
    int id;
    QString name;
    QString avatar;
};

#endif // CLIENT_H
