#include "chatserver.h"
#include "../server/manager.h"
#include "serverprotocol.h"
#include <QJsonArray>
#include <QJsonParseError>
#include <stdexcept>
namespace { constexpr qsizetype MaxFrame = 16*1024*1024; }
ChatServer::ChatServer(Manager &m, QObject *parent) : QObject(parent), manager(m) {
    clock.start();
    connect(&manager,&Manager::notifyUser,this,&ChatServer::notify);
    connect(&server,&QTcpServer::newConnection,this,[this] {
        while(server.hasPendingConnections()) {
            auto *socket=server.nextPendingConnection();
            if(sessions.size() >= 64) { socket->disconnectFromHost();socket->deleteLater();continue; }
            socket->setReadBufferSize(MaxFrame+1);
            sessions.insert(socket,{});
            connect(socket,&QTcpSocket::readyRead,this,[this,socket]{read(socket);});
            connect(socket,&QTcpSocket::disconnected,this,[this,socket] {
                const int id=sessions.value(socket).userId;
                sessions.remove(socket);
                if(id>0 && !online(id)) presence(id,false);
                socket->deleteLater();
            });
        }
    });
}
bool ChatServer::listen(const QHostAddress &address, quint16 port) { return server.listen(address,port); }
bool ChatServer::online(int id) const {
    for(const auto &s:sessions) if(s.userId==id) return true;
    return false;
}
void ChatServer::send(QTcpSocket *s,const QJsonObject &o) {
    const auto frame=ServerProtocol::jsonObjectToData(o);
    if(frame.size()>MaxFrame || s->bytesToWrite()+frame.size()>2*MaxFrame) { s->abort();return; }
    s->write(frame);
}
void ChatServer::notify(int id,const QJsonObject &event) {
    const auto sockets=sessions.keys();
    for(auto *s:sockets) if(sessions.value(s).userId==id) send(s,event);
}
void ChatServer::presence(int id,bool active) {
    try { for(int peer:manager.friendIds(id)) notify(peer,{{"type","friend_update"},{"id",id},{"bool",active}}); }
    catch(...) { /* Database errors must not escape a Qt signal handler. */ }
}
void ChatServer::read(QTcpSocket *socket) {
    if(!sessions.contains(socket)) return;
    sessions[socket].buffer+=socket->readAll();
    while(sessions.contains(socket)) {
        auto &buffer=sessions[socket].buffer;
        auto end=buffer.indexOf('\n');
        if(end<0) { if(buffer.size()>MaxFrame)socket->abort(); return; }
        if(end>MaxFrame) {socket->abort();return;}
        const auto line=buffer.left(end);buffer.remove(0,end+1);
        QJsonParseError error;
        auto doc=QJsonDocument::fromJson(line,&error);
        if(error.error!=QJsonParseError::NoError || !doc.isObject()) {socket->abort();return;}
        dispatch(socket,doc.object());
    }
}
void ChatServer::dispatch(QTcpSocket *socket,const QJsonObject &request) {
    const auto requestId=request["request_id"].toString();
    QJsonObject reply{{"type","response"},{"request_id",requestId},{"ok",false}};
    try {
        if(requestId.isEmpty() || requestId.size()>64 || !request["args"].isObject())
            throw std::runtime_error("invalid_request");
        const auto op=request["type"].toString();
        auto &session=sessions[socket];
        if(op=="login" || op=="register" || op=="available") {
            if(session.userId>0) throw std::runtime_error("already_authenticated");
            if(clock.elapsed()-session.authWindow>60000) {session.authAttempts=0;session.authWindow=clock.elapsed();}
            if(++session.authAttempts>30) throw std::runtime_error("rate_limited");
        } else if(session.userId==0) throw std::runtime_error("authentication_required");
        auto result=manager.dispatch(session.userId,op,request["args"].toObject());
        if(op=="login") {
            session.userId=result["user"].toObject()["id"].toInt();
            send(socket,{{"type","response"},{"request_id",requestId},{"ok",true},{"result",result}});
            presence(session.userId,true);
            return;
        }
        if(op=="friends") {
            auto users=result["users"].toArray();
            for(int i=0;i<users.size();++i) {auto u=users[i].toObject();u["online"]=online(u["id"].toInt());users[i]=u;}
            result["users"]=users;
        }
        reply["ok"]=true;reply["result"]=result;
    } catch(const std::exception &e) { reply["error"]=QString::fromLatin1(e.what()); }
    send(socket,reply);
}
