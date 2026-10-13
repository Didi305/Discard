#include "manager.h"
#include <QFile>
#include <QSaveFile>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QJsonArray>
#include <QTimer>
#include <QCryptographicHash>
#include <QDebug>

Manager::Manager(QObject *parent):QObject(parent) {}
Manager *Manager::get(){static Manager instance;return &instance;}
void Manager::setClient(Client *c){client=c;}
Client *Manager::getClient(){return client;}
bool Manager::connectServer(){return client && client->ensureConnected();}
QString Manager::lastError() const{return client?client->errorString():"Client is not initialized.";}
QJsonObject Manager::call(const QString &op,const QJsonObject &args){return client->request(op,args);}
QString Manager::cacheMedia(const QByteArray &bytes,const QString &category,qint64 id,const QString &filename) const {
    QString suffix=QFileInfo(filename).suffix().toLower();suffix.remove(QRegularExpression("[^a-z0-9]"));
    if(suffix.isEmpty())suffix="bin";
    QDir root(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if(!root.mkpath("media/"+category))return {};
    QString cacheName = QString::number(id);
    // A new filename also invalidates Qt's filename-based pixmap cache.
    if (category == "avatars")
        cacheName += "-" + QString::fromLatin1(QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex());
    const auto path=root.filePath(QString("media/%1/%2.%3").arg(category,cacheName,suffix));
    {
        QFile old(path);
        if(old.open(QIODevice::ReadOnly)&&old.readAll()==bytes)return path;
    } // Release the existing file before QSaveFile replaces it on Windows.
    QSaveFile file(path);
    if(!file.open(QIODevice::WriteOnly)||file.write(bytes)!=bytes.size()||!file.commit()) {
        qWarning() << "Could not cache media:" << file.errorString();
        return {};
    }
    return path;
}
QString Manager::avatarPathFromValues(int id,const QByteArray &bytes,const QString &name) {
    auto path=bytes.isEmpty()?QString(":/avatars/default.png"):cacheMedia(bytes,"avatars",id,name);
    if(path.isEmpty())path=":/avatars/default.png";
    avatarPaths[id]=path;return path;
}
Data Manager::profile(const QJsonObject &o) {
    Data d{};d.user_id=o["id"].toInt();d.displayName=o["name"].toString();
    d.status=Utils::statusFromString(o["status"].toString());
    d.avatar=avatarPathForUser(d.user_id,true);return d;
}
bool Manager::authenticate(const QString &username,const QString &password) {
    auto reply=call("login",{{"username",username},{"password",password}});
    if(!reply["ok"].toBool())return false;
    avatarPaths.clear();
    auto user=profile(reply["result"].toObject()["user"].toObject());
    client->setClientID(user.user_id);client->setClientName(user.displayName);
    client->setClientStatus(user.status);client->setClientAvatar(user.avatar);client->setClientData(user);
    fetchFriends(user.user_id,client->getFriends());return true;
}
bool Manager::createUser(QString username,QString password,QString nickname) {
    return call("register",{{"username",username},{"password",password},{"nickname",nickname}})["ok"].toBool();
}
bool Manager::checkUsernameAvailibity(QString value) {
    auto r=call("available",{{"field","username"},{"value",value}});
    return r["ok"].toBool()&&r["result"].toObject()["available"].toBool();
}
bool Manager::checkNicknameAvailibity(QString value) {
    auto r=call("available",{{"field","nickname"},{"value",value}});
    return r["ok"].toBool()&&r["result"].toObject()["available"].toBool();
}
bool Manager::updateUserAvatar(QString path,QString nickname) {
    Q_UNUSED(nickname)
    QFile file(path);if(!file.open(QIODevice::ReadOnly)||file.size()>2*1024*1024)return false;
    auto r=call("set_avatar",{{"filename",QFileInfo(path).fileName()},{"bytes",QString::fromLatin1(file.readAll().toBase64())}});
    if(r["ok"].toBool())avatarPaths.remove(client->getClientID());
    return r["ok"].toBool();
}
QString Manager::avatarPathForUser(int userId,bool reuseCached) {
    if(reuseCached&&avatarPaths.contains(userId)&&QFileInfo::exists(avatarPaths[userId]))return avatarPaths[userId];
    auto r=call("avatar",{{"peer_id",userId}});
    if(!r["ok"].toBool())return ":/avatars/default.png";
    auto o=r["result"].toObject();return avatarPathFromValues(userId,QByteArray::fromBase64(o["bytes"].toString().toLatin1()),o["filename"].toString());
}
QString Manager::checkFriendship(QString nickname) {
    auto r=call("friend_request",{{"nickname",nickname}});
    return r["ok"].toBool()?r["result"].toObject()["status"].toString():QString();
}
std::vector<std::unique_ptr<Row>> Manager::fetchRequests(int userId,RequestType type) {
    Q_UNUSED(userId)
    auto r=call("requests",{{"direction",type==RequestType::Pending?"incoming":"outgoing"}});
    std::vector<std::unique_ptr<Row>> rows;
    for(const auto &v:r["result"].toObject()["users"].toArray()) {
        auto d=profile(v.toObject());
        if(type==RequestType::Pending)rows.emplace_back(std::make_unique<PendingRow>(d));
        else rows.emplace_back(std::make_unique<OutgoingRow>(d));
    }
    return rows;
}
std::vector<std::unique_ptr<Row>> Manager::fetchPendingRequests(int id){return fetchRequests(id,RequestType::Pending);}
std::vector<std::unique_ptr<Row>> Manager::fetchOutgoingRequests(int id){return fetchRequests(id,RequestType::Outgoing);}
QHash<Data,int>& Manager::fetchFriends(int userId,QHash<Data,int>& friends) {
    Q_UNUSED(userId)
    const auto r=call("friends");
    if(!r["ok"].toBool())return friends;
    friends.clear();
    for(const auto &v:r["result"].toObject()["users"].toArray()){const auto o=v.toObject();friends.insert(profile(o),o["online"].toBool()?1:0);}
    return friends;
}
bool Manager::acceptRequest(Data d){auto r=call("friend_accept",{{"peer_id",d.user_id}});if(r["ok"].toBool())QTimer::singleShot(0,client,&Client::relationshipsChanged);return r["ok"].toBool();}
bool Manager::declineRequest(int id){return call("friend_decline",{{"peer_id",id}})["ok"].toBool();}
bool Manager::cancelRequest(int id){return call("friend_cancel",{{"peer_id",id}})["ok"].toBool();}
int Manager::checkConversation(int ownId,int peerId) {
    Q_UNUSED(ownId)
    auto r=call("conversation",{{"peer_id",peerId}});return r["result"].toObject()["conversation_id"].toInt(-1);
}
bool Manager::sendChatMessage(const QString &body,int conv,int peer,const QVector<Attachement>& files,Data &sent) {
    QJsonArray array; qint64 total=0;
    if(files.size()>8)return false;
    for(const auto &a:files){QFile f(a.path);if(!f.open(QIODevice::ReadOnly))return false;total+=f.size();if(total>8*1024*1024)return false;
        array.append(QJsonObject{{"filename",QFileInfo(a.path).fileName()},{"bytes",QString::fromLatin1(f.readAll().toBase64())}});}
    auto r=call("send_message",{{"peer_id",peer},{"conversation_id",conv},{"body",body},{"attachments",array}});
    if(!r["ok"].toBool())return false;
    auto o=r["result"].toObject();o["id"]=client->getClientID();o["name"]=client->getClientName();
    o["status"]=Utils::stringFromStatus(client->getClientStatus());
    sent=Utils::buildData(o);resolveDataMedia(sent);return true;
}
QString Manager::attachmentPathForId(qint64 id) {
    auto r=call("attachment",{{"attachment_id",id}});if(!r["ok"].toBool())return {};
    auto o=r["result"].toObject();return cacheMedia(QByteArray::fromBase64(o["bytes"].toString().toLatin1()),"attachments",id,o["filename"].toString());
}
void Manager::resolveDataMedia(Data &data) {
    data.avatar=avatarPathForUser(data.user_id,true);
    for(auto &a:data.message.attachs) {
        const QString prefix="db://attachment/";
        if(a.path.startsWith(prefix))a.path=attachmentPathForId(a.path.mid(prefix.size()).toLongLong());
        else a.path.clear(); // Never trust a remote path to a local file.
    }
}
StringContainer Manager::fetchMessages(int conv) {
    StringContainer messages;if(conv<=0)return messages;
    int after=0;
    while(true) {
        auto r=call("messages",{{"conversation_id",conv},{"after_id",after}});if(!r["ok"].toBool())break;
        const auto rows=r["result"].toObject()["messages"].toArray();
        for(const auto &v:rows){auto o=v.toObject();o["id"]=o["messageSender"];auto d=Utils::buildData(o);resolveDataMedia(d);messages.push_back(d.message);after=o["message_id"].toInt();}
        if(rows.size()<100)break;
    }
    return messages;
}
std::vector<std::unique_ptr<ConversationRow>> Manager::fetchConversations(Data &data) {
    Q_UNUSED(data)
    std::vector<std::unique_ptr<ConversationRow>> rows;
    auto r=call("conversations");
    for(const auto &v:r["result"].toObject()["conversations"].toArray()) {
        auto o=v.toObject();auto d=profile(o["user"].toObject());
        Message m{};m.body=o["body"].toString();m.senderID=o["sender"].toInt();m.sentIn=o["conversation_id"].toInt();
        rows.emplace_back(std::make_unique<ConversationRow>(m.sentIn,d,m));
    }
    return rows;
}
