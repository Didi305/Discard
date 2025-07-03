#include <qobject.h>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtSql>
#include <QSqlError>
#include <QApplication>
#include <QFile>
#include <QJsonObject>
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
    query.bindValue(":ava", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Chat/uploads/imag.png");
    if(!query.exec()){
        qWarning() << "Insert failed" << query.lastError().text();
        return false;
    }
    return true;
}

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
        qDebug() << "status is: " << status;
        if(status.isNull()){
            Data d;
            d.avatar = client->getClientAvatar();
            d.displayName = client->getClientName();
            d.user_id = query.value("users.id").toString().toInt();
            d.status = Utils::statusFromString(query.value("users.status").toString());
            qDebug() << "receiver's ID: " << d.user_id;
            qDebug() << "Not Amigos";
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
            qDebug() << "Found Friendship with status" << query.value("friendship.status").toString();
            qDebug() << "friend id hiya " <<query.value("users.id");
            return query.value("friendship.status").toString();
        }
    }
    return "zemla hadi";
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

std::vector<std::unique_ptr<Row>> Manager::fetchRequests(int id, RowType type){
    QString arg1 = "friend";
    QString arg2 = "user";
    if(type == RowType::Pending){
        arg1 = "user";
        arg2 = "friend";
    }
    std::vector<std::unique_ptr<Row>> requests;
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
        requests.emplace_back(std::make_unique<Row>(d, type));
    }
    return requests;
}

QHash<Data , int>& Manager::fetchFriends(int user_id, QHash<Data , int> &friends){
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
    qDebug() << "our new friend name is: " << d.displayName << " and hes preferred status mode is: " << Utils::stringFromStatus(d.status);
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
    auto friends = client->getFriends();
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
