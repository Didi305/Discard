#include "attachementchip.h"
#include <qobject.h>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
#include <QSqlError>
#include <QApplication>
#include <QFile>
#include <QJsonObject>*

#include <QCryptographicHash>
#include <QDateTime>
#include <QStandardPaths>
#include "manager.h"


Manager::Manager(QObject *parent) : QObject(parent) {
}

Manager* Manager::get(){
    static Manager mgr(nullptr);
    return &mgr;
}
Client* Manager::getClient(){
    return client;
}

void Manager::setClient(Client *client){
    this->client = client;
}
QSqlDatabase& Manager::db(){
    return database;
}
bool Manager::init(QJsonObject &DBConfig, QSqlDatabase &dbex){
    QString path = QCoreApplication::applicationDirPath() + QLatin1String("/config.json");
    QFile file(path);
    database = dbex;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open" << path << ":" << file.errorString();
        return false;
    }

    const QByteArray data = file.readAll();

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if(!doc.isObject()){
        qWarning() << "JSON file doesnt contain an object";
    }
    DBConfig = doc.object();
    database.setHostName(DBConfig.take("host").toString());
    database.setPort(DBConfig.take("port").toString().toInt());
    database.setDatabaseName(DBConfig.take("database").toString());
    database.setUserName(DBConfig.take("user").toString());
    database.setPassword(DBConfig.take("password").toString());
    bool ok = database.open();
    database.driver()->subscribeToNotification("friendship");

    if (!ok) {
        qCritical() << "Cannot open DB:" << database.lastError().text();
    }
    return ok;
}


bool Manager::checkUsernameAvailibity(QString username){
    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "SELECT id "
        "FROM users "
        "WHERE username = :u "
        );
    query.prepare(sql);
    query.bindValue(":u", username);

    if(!query.exec()){
        qCritical() << query.lastError().text();
    }
    if(query.first()){
        qCritical() << "username unavailable ^^";
        return false;
    }
    else{
        qCritical() << "Username available ^^";
        return true;
    }
}

bool Manager::checkNicknameAvailibity(QString nickname){
    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "SELECT id "
        "FROM users "
        "WHERE nickname = :n "
        );
    query.prepare(sql);
    query.bindValue(":u", nickname);

    if(!query.exec()){
        qCritical() << query.lastError().text();
    }
    if(query.first()){
        qCritical() << "nick unavailable ^^";
        return false;
    }
    else{
        qCritical() << "nick available ^^";
        return true;
    }
}

QString Manager::checkUser(const QString& username, const QString& password){
    QSqlQuery query(db());
    if(!password.isEmpty()){
        QByteArray password_hash = (QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256)).toHex();
        QString sql = QStringLiteral(
            "SELECT avatar, nickname, id, status "
            "FROM users "
            "WHERE username = :u AND password_hash = :p "
            );
        query.prepare(sql);
        query.bindValue(":u" , username);
        query.bindValue(":p", password_hash);

        if(!query.exec()){
            qCritical() << query.lastError().text();
        }
        if(query.first()){
            return query.value("nickname").toString() + ";" + query.value("avatar").toString() + ";" + query.value("id").toString() + ";" + query.value("status").toString();
        }else{
            qDebug() << "Username or password wrong";
            return "";
        }
    }
    else{
        QString sql = QStringLiteral(
            "SELECT id "
            "FROM users "
            "WHERE nickname = :u "
            );
        query.prepare(sql);
        query.bindValue(":u" , username);
        if(!query.exec()){
            qCritical() << query.lastError().text();
        }
        if(query.first()){
            return query.value("id").toString();
        }else{
            return "";
        }
    }
}

bool Manager::createUser(QString username, QString password, QString nickname){
    QByteArray pw = password.toUtf8();
    QByteArray password_hash = QCryptographicHash::hash(pw, QCryptographicHash::Sha256);

    QSqlQuery query(db());
    static const QString sql = QStringLiteral(
        "INSERT INTO users (username, password_hash, nickname, created_at, avatar) "
        "VALUES ( :user, :pass, :nick, :date, :ava )"
);

    // 2) Prepare the statement
    if (!query.prepare(sql)) {
        qWarning() << "prepare failed:" << query.lastError().text();
    }
    query.prepare(sql);

    query.bindValue(":user", username);
    query.bindValue(":pass", password_hash.toHex());
    query.bindValue(":nick", nickname);
    query.bindValue(":ava", nickname);
    query.bindValue(":date", QDateTime::currentDateTime());
    query.bindValue(":ava", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Discard/uploads/imag.png");
    if(!query.exec()){
        qWarning() << "Insert failed" << query.lastError().text();
        return false;
    }
    return true;
}



void Manager::updateUserAvatar(QString path, QString nickname){
    QSqlQuery query(db());

    static const QString sql = QStringLiteral(
        "UPDATE users "
        "SET avatar = :av "
        "WHERE nickname = :nick "
        );
    query.prepare(sql);
    query.bindValue(":av", path);
    query.bindValue(":nick", nickname);
    if(!query.exec()){
        qCritical() << "avatar update failed" << query.lastError().text();
    }
}



/*--------------------Friends-------------------------------*/


QString Manager::checkFriendship(QString nickname){
    if(checkUser(nickname).isEmpty()){
        return "";
    }
    int friendID = checkUser(nickname).toInt();
    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "SELECT users.id, users.avatar, users.nickname, users.status, friendship.status "
        "FROM users "
        "LEFT JOIN friendship ON (friendship.friend_id = users.id AND friendship.user_id = :u ) OR (friendship.user_id = users.id AND friendship.friend_id = :u ) "
        "WHERE users.id = :f "
        );
    query.prepare(sql);
    query.bindValue(":u" , client->getClientID());
    query.bindValue(":f", friendID);

    if(!query.exec()){
        qCritical() << query.lastError().text();
    }

    if(query.first()){
        QString status = query.value("friendship.status").toString();
        if(status.isNull()){
            Data d;
            d.avatar = client->getClientAvatar();
            d.displayName = client->getClientName();
            d.user_id = query.value("users.id").toString().toInt();
            d.status = Utils::statusFromString(query.value("users.status").toString());
            QString sql2 = QStringLiteral(
                "INSERT INTO friendship (user_id, friend_id, status, start_date) "
                "VALUES(:u, :f, :stat, :date) "
                );
            query.prepare(sql2);
            query.bindValue(":u" , client->getClientID());
            query.bindValue(":f", friendID);
            query.bindValue(":stat" , "PENDING");
            query.bindValue(":date", QDateTime::currentDateTime());
            query.exec();
            client->sendMessage(d, "add_request");
            return "Request Sent";
        }
        else{
            return query.value("friendship.status").toString();
        }
    }
    return "zemla hadi";
}

std::vector<std::unique_ptr<Row>> Manager::fetchRequests(int id, RequestType type){
    QString arg1 = (type == RequestType::Pending ? "user" : "friend");
    QString arg2 = (type == RequestType::Pending ? "friend" : "user");

    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "SELECT users.id, users.nickname, users.avatar, users.status "
        "FROM users "
        "INNER JOIN friendship ON users.id = friendship.%1_id "
        "WHERE friendship.%2_id = :id AND friendship.status = :s"
        ).arg(arg1, arg2);
    query.prepare(sql);
    query.bindValue(":id" , id);
    query.bindValue(":s", "PENDING");
    std::vector<std::unique_ptr<Row>> requests;
    if(!query.exec()){
        qCritical() << query.lastError().text();
        return {};
    }
    while(query.next()){
        Data d;
        d.avatar = query.value("avatar").toString();
        d.displayName = query.value("nickname").toString();
        d.user_id = query.value("id").toInt();
        d.status = Utils::statusFromString(query.value("status").toString());
        if(type == RequestType::Pending) requests.emplace_back(std::make_unique<PendingRow>(d));
        else requests.emplace_back(std::make_unique<OutgoingRow>(d));
    }
    return requests;
}

std::vector<std::unique_ptr<Row>> Manager::fetchPendingRequests(int userId) {
    return fetchRequests(userId, RequestType::Pending);
}
std::vector<std::unique_ptr<Row>> Manager::fetchOutgoingRequests(int userId) {
    return fetchRequests(userId, RequestType::Outgoing);
}
QHash<Data , int>& Manager::fetchFriends(int user_id, QHash<Data , int> &friends){
    qDebug() << "user id: "  << user_id;
    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "SELECT users.id, users.nickname, users.avatar, users.status "
        "FROM users "
        "WHERE users.id IN ( "
        "SELECT friend_id "
        "FROM friendship "
        "WHERE user_id = :u AND status = :s "
        "UNION "
        "SELECT user_id "
        "FROM friendship "
        "WHERE friend_id = :u AND status = :s "
        ")"
    );
    query.prepare(sql);
    query.bindValue(":u", user_id);
    query.bindValue(":s", "ACTIVE");
    if(!query.exec()){
        qCritical() << query.lastError().text();
    }
    while(query.next()){
        Data d;
        d.avatar = query.value("users.avatar").toString();
        d.displayName = query.value("users.nickname").toString();
        d.user_id = query.value("users.id").toString().toInt();
        d.status = Utils::statusFromString(query.value("users.status").toString());
        friends.insert(d, 0);
    }

    return friends;
}


void Manager::acceptRequest(Data d){
    client->getFriends().insert(d, 0);
    client->sendMessage(d, "user_status");
    client->sendMessage(d, "update_pending");
    QSqlQuery query(db());
    static const QString sql = QStringLiteral(
        "UPDATE friendship "
        "SET status = :s "
        "WHERE user_id = :id AND friend_id = :f "
        );
    query.prepare(sql);
    query.bindValue(":id", d.user_id);
    query.bindValue(":f", client->getClientID());
    query.bindValue(":s", "ACTIVE");
    if(!query.exec()){
        qCritical() << query.lastError().text();
    }

    d.avatar = client->getClientAvatar();
    d.displayName = client->getClientName();
    d.status = client->getClientStatus();
    client->sendMessage(d, "add_row");
}

void Manager::declineRequest(int user_id){
    QSqlQuery query(db());

    static const QString sql = QStringLiteral(
        "DELETE FROM friendship "
        "WHERE user_id = :id AND friend_id = :f "
        );
    query.prepare(sql);
    query.bindValue(":id", user_id);
    query.bindValue(":f", client->getClientID());
    if(!query.exec()){
        qCritical() << query.lastError().text();
    }
    Data d;
    d.user_id = user_id;
    client->sendMessage(d, "update_pending");
}

void Manager::cancelRequest(int friend_id){
    QSqlQuery query(db());

    static const QString sql = QStringLiteral(
        "DELETE FROM friendship "
        "WHERE user_id = :id AND friend_id = :f "
        );
    query.prepare(sql);
    query.bindValue(":id", client->getClientID());
    query.bindValue(":f", friend_id);
    if(!query.exec()){
        qCritical() << query.lastError().text();
    }
    Data d;
    d.user_id = friend_id;
    client->sendMessage(d, "update_accept");
}

/*----------------DM's--------------------------------*/


int Manager::createConversation(int user1_ID, int user2_ID){
    int conversationID;
    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "INSERT INTO conversation (user_1, user_2) "
        "VALUES(:u1, :u2) "
        "RETURNING conversation_id "
        );

    query.prepare(sql);
    query.bindValue(":u1", user1_ID);
    query.bindValue(":u2", user2_ID);
    if(!query.exec()){
        qCritical() << "could not create a conversation: " << query.lastError().text();
    }
    if(query.next()){
        conversationID = query.value(0).toInt();
    }
    return conversationID;
}

int Manager::checkConversation(int user1_ID, int user2_ID){

    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "SELECT conversation_id "
        "FROM conversation "
        "WHERE (user_1 = :u1 AND user_2 = :u2) "
        "  OR (user_1 = :u2 AND user_2 = :u1)"
        );
    query.prepare(sql);
    query.bindValue(":u1", user1_ID);
    query.bindValue(":u2", user2_ID);
    if(!query.exec()){
        qCritical() << "couldnt check for conversations" << query.lastError().text();
    }
    if(!query.next()){
        qDebug() << "There is no conversation between: " << user1_ID << " AND " << user2_ID;
        int conversationID = createConversation(user1_ID, user2_ID);
        return conversationID;
    }
    return query.value("conversation_id").toInt();
}

quint64 Manager::postMessage(QString message, int conversation_ID, int sender_ID){
    qint64 message_id;
    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "INSERT INTO message (body, sent_by, sent_at, sent_in) "
        "VALUES (:m, :s, :d, :c ) "
        "RETURNING id "
        );
    query.prepare(sql);
    query.bindValue(":m", message);
    query.bindValue(":s", sender_ID);
    query.bindValue(":d", QDateTime::currentDateTimeUtc().toString());
    query.bindValue(":c", conversation_ID);
    if(!query.exec()){
        qCritical() << query.lastError().text();
    }
    if(query.next()){
        message_id = query.value(0).toLongLong();
    }
    qDebug() << "message_ id: " << message_id;
    return message_id;
}

void Manager::postAttachement(QFileInfo info, quint64 message_id){
    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "INSERT INTO Attachement (message_id, filename, content_type, size_bytes, data) "
        "VALUES (:id, :n, :type, :size, :d ) "
        );
    query.prepare(sql);
    query.bindValue(":id", message_id);
    query.bindValue(":n", info.fileName());
    query.bindValue(":type", info.suffix());
    query.bindValue(":size", info.size());
    query.bindValue(":d", info.absoluteFilePath());

    if(!query.exec()){
        qCritical() << "zmel attach" << query.lastError().text();
    }
}
StringContainer Manager::fetchMessages(int conversationID){
    StringContainer messages;
    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "SELECT body, sent_by, sent_at "
        "FROM message "
        "WHERE sent_in = :c "
        "ORDER BY sent_at ASC"
        );
    query.prepare(sql);
    query.bindValue(":c", conversationID);
    if(!query.exec()){
        qCritical() << "la walo" << query.lastError().text();
    }
    while(query.next()){
        Message msg = {query.value("body").toString(), query.value("sent_by").toInt(), query.value("sent_at").toString()};
        messages.push_back(msg);
    }

    return messages;
}

QVector<QString> Manager::fetchAttachements(qint64 message_id){
    QVector<QString> attachs;
    QSqlQuery query(db());
    QString sql = QStringLiteral(
        "SELECT body, sent_by, sent_at "
        "FROM message "
        "WHERE sent_in = :c "
        "ORDER BY sent_at ASC"
        );
    query.prepare(sql);
    query.bindValue(":c", conversationID);
    if(!query.exec()){
        qCritical() << "la walo" << query.lastError().text();
    }
    while(query.next()){
        Message msg = {query.value("body").toString(), query.value("sent_by").toInt(), query.value("sent_at").toString()};
        messages.push_back(msg);
    }

    return messages;
}










