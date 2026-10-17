#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariantList>

// Database operations run only in the server process. actorId is supplied by
// ChatServer's authenticated socket map, never by request JSON.
class Manager : public QObject {
    Q_OBJECT
public:
    explicit Manager(QObject *parent = nullptr);
    ~Manager();
    bool init(const QString &configPath, QString &error);
    QJsonObject dispatch(int actorId, const QString &operation, const QJsonObject &arguments);
    QList<int> friendIds(int actorId);
signals:
    void notifyUser(int userId, QJsonObject event);

private:
    QSqlDatabase database;
    QString connectionName;
    QSqlQuery query(const QString &sql, const QVariantList &values = {});
    QJsonObject profile(int id);
    bool related(int actorId, int peerId);
    bool friends(int actorId, int peerId);
    int partner(int actorId, int conversation);
    int conversation(int actorId, int peerId, bool create);
    QJsonArray attachments(int messageId);
    QJsonObject message(QSqlQuery &row);
    QJsonObject login(const QJsonObject &arguments);
    QJsonObject registerUser(const QJsonObject &arguments);
    QJsonObject sendMessage(int actorId, const QJsonObject &arguments);
    QJsonObject
    changeFriendship(int actorId, const QString &operation, const QJsonObject &arguments);
};
