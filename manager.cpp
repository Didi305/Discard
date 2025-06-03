#include <qobject.h>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QFile>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QDateTime>
#include "manager.h"
#include "client.h"
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
            "SELECT avatar, nickname, id "
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
            qDebug() << "User found with nickname:" << query.value("nickname").toString() + "and avatar path: " + query.value("avatar").toString();
            return query.value("nickname").toString() + ";" + query.value("avatar").toString() + ";" + query.value("id").toString();
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
            qDebug() << "User found with ID" << query.value("id").toString();
            return query.value("id").toString();
        }else{
            qDebug() << "User doesn't exist";
            return "";
        }
    }
}

bool Manager::createUser(QString username, QString password, QString nickname){
    QByteArray pw = password.toUtf8();
    QByteArray password_hash = QCryptographicHash::hash(pw, QCryptographicHash::Sha256);

    QSqlQuery query(db());
    static const QString sql = QStringLiteral(
        "INSERT INTO users (username, password_hash, nickname, created_at) "
        "VALUES ( :user, :pass, :nick, :date )"
);

    // 2) Prepare the statement
    if (!query.prepare(sql)) {
        qWarning() << "prepare failed:" << query.lastError().text();
    }
    query.prepare(sql);

    query.bindValue(":user", username);
    query.bindValue(":pass", password_hash.toHex());
    query.bindValue(":nick", nickname);
    query.bindValue(":date", QDateTime::currentDateTime());
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
        "SELECT user_id, friend_id, status "
        "FROM friendship "
        "WHERE user_id = :u AND friend_id = :f "
        );
    query.prepare(sql);
    query.bindValue(":u" , client->getClientID());
    query.bindValue(":f", friendID);

    if(!query.exec()){
        qCritical() << query.lastError().text();
    }
    if(query.first()){
        qDebug() << "Friend found with status:  " << query.value("status").toString();
        return query.value("status").toString();
    }else{
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
        return "Request Sent";
    }
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
