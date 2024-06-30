#ifndef CLIENT_H
#define CLIENT_H

#include <QTcpSocket>
class Client : public QObject {
    Q_OBJECT
public:
    Client(QObject *parent = nullptr);
    ~Client();

    void connectToServer(const QString &host, int port);
    void disconnectFromServer();
    void sendMessage(const QString &message);

signals:
    void connected();
    void disconnected();
    void messageReceived();

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();

private:
    QTcpSocket *socket;
    QString username;
};

#endif // CLIENT_H
