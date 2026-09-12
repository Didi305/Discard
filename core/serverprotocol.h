#pragma once

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace ServerProtocol {

inline QJsonObject parseJson(const QByteArray &data)
{
    return QJsonDocument::fromJson(data).object();
}

inline QByteArray jsonObjectToData(const QJsonObject &obj)
{
    QByteArray data =
        QJsonDocument(obj).toJson(QJsonDocument::Compact);

    data.append('\n');
    return data;
}

inline QByteArray updateStatus(int userId, bool online)
{
    QJsonObject obj;
    obj["type"] = "friend_update";
    obj["id"] = userId;
    obj["bool"] = online;

    return jsonObjectToData(obj);
}

}