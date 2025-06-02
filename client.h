#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject *parent = nullptr);
    QString getClientName();
    QString getClientAvatar();
    void setClientName(QString name);
    void setClientAvatar(QString avatar);
    ~Client();

    void connectToServer(const QString &host, int port);
    void disconnectFromServer();
    void sendMessage(const QString &message);

signals:
    void connected();
    void disconnected();
    void messageReceived(QString &data);

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

private:
    QTcpSocket *socket;
    QString name;
    QString avatar;
};

#endif // CLIENT_H
