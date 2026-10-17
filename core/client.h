#pragma once
#include "row.h"
#include "utils.h"
#include <QJsonObject>
#include <QTcpSocket>
class Manager;
class Client : public QObject {
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client() override = default;
    QString getClientName();
    QString getClientAvatar();
    Status getClientStatus();
    int getClientID();
    Data &getClientData();
    QHash<Data, int> &getFriends();

    void setFriends(QHash<Data, int> &updatedFriends);
    void setClientName(QString displayName);
    void setClientStatus(Status userStatus);
    void setClientAvatar(QString avatarPath);
    void setClientID(int userId);
    void setClientData(Data &userData);
    void setEndpoint(const QString &hostname, int serverPort);
    QString serverEndpoint() const;
    bool ensureConnected();
    QJsonObject request(const QString &operation, const QJsonObject &arguments = {});
    void disconnectFromServer();
    QString errorString() const {
        return lastError;
    }
signals:
    void connected();
    void disconnected();
    void updatePendingRequests(int friendUserId);
    void updateAcceptRequest(QJsonObject &event, QString type);
    void updateFriends(int userId, bool online);
    void updateConversations(Message &message, Data &userData);
    void friendsReady(QHash<Data, int> friends);
    void addNewFriendRow(Data userData);
    void newMessage(Data userData);
    void relationshipsChanged();

private:
    QTcpSocket *socket;
    Manager *m_manager;
    int id = 0;
    QString name;
    QString avatar;
    Status status = Status::ONLINE;
    Data clientData{};
    QHash<Data, int> friends;
    QString host = "127.0.0.1";
    int port = 41234;
    QByteArray receiveBuffer;
    QHash<QString, QJsonObject> responses;
    QList<QJsonObject> events;
    quint64 requestCounter = 0;
    bool requesting = false;
    bool dispatching = false;
    QString lastError;
    void onReadyRead();
    void dispatchEvents();
};
