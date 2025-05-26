#include <qobject.h>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QApplication>
#include <QFile>
#include <QJsonObject>
#include "manager.h"

Manager::Manager(QObject *parent) : QObject(parent) {
}

Manager* Manager::get(){
    static Manager mgr(nullptr);
    return &mgr;
}

bool Manager::init(QJsonObject &DBConfig){
    QString path = QCoreApplication::applicationDirPath() + QLatin1String("/config.json");
    QFile file(path);

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
    return true;
}
