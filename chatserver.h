#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QTcpServer>
class ChatServer : public QObject {
    Q_OBJECT
public:
    ChatServer(QObject *parent = nullptr);
    ~ChatServer();

signals:
    void messageReceived();

public slots:
    void onNewConnection();
    void readClient();
    void clientDisconnected();
private:
    QTcpServer *tcpServer;
    QList<QTcpSocket*> clients;

};


#endif
