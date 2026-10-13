#include "manager.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QCryptographicHash>
#include <QDateTime>
#include <QSqlError>
#include <QRegularExpression>
#include <QUuid>
#include <stdexcept>

namespace {
constexpr int MaxFileBytes = 8 * 1024 * 1024;
void require(bool ok, const char *error) { if (!ok) throw std::runtime_error(error); }
QString text(const QJsonObject &o, const char *key, int maximum, bool empty = false) {
    require(o.value(key).isString(), "invalid_request");
    const auto s = o.value(key).toString();
    require(s.size() <= maximum && (empty || !s.trimmed().isEmpty()) && !s.contains(QChar::Null), "invalid_request");
    return s;
}
int id(const QJsonObject &o, const char *key) {
    const auto value = o.value(key);
    const int n = value.toInt(-1);
    require(value.isDouble() && n > 0 && value.toDouble() == n, "invalid_request");
    return n;
}
QByteArray bytes(const QJsonObject &o, int maximum) {
    const auto value = text(o, "bytes", (maximum + 2) / 3 * 4, true).toLatin1();
    const auto decoded = QByteArray::fromBase64Encoding(value, QByteArray::AbortOnBase64DecodingErrors);
    require(bool(decoded) && decoded.decoded.size() <= maximum, "invalid_file");
    return decoded.decoded;
}
QString filename(const QJsonObject &o) {
    auto name = text(o, "filename", 255);
    name.replace('\\', '/');
    name = name.section('/', -1);
    require(!name.isEmpty() && name != "." && name != "..", "invalid_file");
    return name;
}
}

Manager::Manager(QObject *parent) : QObject(parent), connectionName(QUuid::createUuid().toString()) {}
Manager::~Manager() {
    database.close(); database = QSqlDatabase(); QSqlDatabase::removeDatabase(connectionName);
}
bool Manager::init(const QString &configPath, QString &error) {
    QFile config(configPath);
    if (configPath.startsWith(':') || !config.open(QIODevice::ReadOnly) || config.size() > 65536) {
        error = "Cannot read the external database configuration."; return false;
    }
#ifdef Q_OS_UNIX
    if (config.permissions() & (QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup |
                               QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther)) {
        error = "Database configuration must be private (chmod 600)."; return false;
    }
#endif
    QJsonParseError parse;
    const auto document = QJsonDocument::fromJson(config.readAll(), &parse);
    const auto cfg = document.object();
    if (parse.error != QJsonParseError::NoError || !document.isObject() ||
        cfg["host"].toString().isEmpty() || cfg["database"].toString().isEmpty() ||
        cfg["user"].toString().isEmpty() || cfg["password"].toString().isEmpty() ||
        cfg["port"].toInt() < 1 || cfg["port"].toInt() > 65535) {
        error = "Invalid database configuration."; return false;
    }
    database = QSqlDatabase::addDatabase("QPSQL", connectionName);
    database.setHostName(cfg["host"].toString()); database.setPort(cfg["port"].toInt());
    database.setDatabaseName(cfg["database"].toString()); database.setUserName(cfg["user"].toString());
    database.setPassword(cfg["password"].toString());
    database.setConnectOptions("connect_timeout=5");
    if (!database.open()) { error = "Cannot connect to PostgreSQL. Check server configuration and service."; return false; }
    try {
        query("SET statement_timeout = '5s'");
        query("SET search_path = pg_catalog, public");
    } catch (...) { error = "Could not initialize database session."; return false; }
    return true;
}
QSqlQuery Manager::query(const QString &sql, const QVariantList &values) {
    QSqlQuery q(database);
    require(q.prepare(sql), "database_error");
    for (const auto &v : values) q.addBindValue(v);
    require(q.exec(), "database_error");
    return q;
}
QJsonObject Manager::profile(int userId) {
    auto q = query("SELECT id,nickname,status::text FROM public.users WHERE id=?", {userId});
    require(q.next(), "not_found");
    return {{"id",q.value(0).toInt()},{"name",q.value(1).toString()},
            {"status",q.value(2).toString()},{"avatar",QString("db://avatar/%1").arg(userId)}};
}
bool Manager::friends(int actor, int other) {
    auto q = query("SELECT 1 FROM public.friendship WHERE status='ACTIVE' AND ((user_id=? AND friend_id=?) OR (user_id=? AND friend_id=?))", {actor,other,other,actor});
    return q.next();
}
bool Manager::related(int actor, int other) {
    if (actor == other) return true;
    auto q = query("SELECT 1 FROM public.friendship WHERE (user_id=? AND friend_id=?) OR (user_id=? AND friend_id=?) UNION ALL SELECT 1 FROM public.conversation WHERE (user_1=? AND user_2=?) OR (user_1=? AND user_2=?) LIMIT 1", {actor,other,other,actor,actor,other,other,actor});
    return q.next();
}
QList<int> Manager::friendIds(int actor) {
    auto q = query("SELECT CASE WHEN user_id=? THEN friend_id ELSE user_id END FROM public.friendship WHERE status='ACTIVE' AND (user_id=? OR friend_id=?)", {actor,actor,actor});
    QList<int> result; while(q.next()) result.append(q.value(0).toInt()); return result;
}
int Manager::partner(int actor, int conv) {
    auto q = query("SELECT user_1,user_2 FROM public.conversation WHERE conversation_id=? AND (user_1=? OR user_2=?)", {conv,actor,actor});
    require(q.next(), "forbidden");
    return q.value(0).toInt() == actor ? q.value(1).toInt() : q.value(0).toInt();
}
int Manager::conversation(int actor, int other, bool create) {
    require(actor != other, "invalid_request");
    auto q = query("SELECT conversation_id FROM public.conversation WHERE (user_1=? AND user_2=?) OR (user_1=? AND user_2=?) ORDER BY conversation_id LIMIT 1", {actor,other,other,actor});
    if(q.next()) return q.value(0).toInt();
    if(!create) return -1;
    require(friends(actor,other), "forbidden");
    q = query("INSERT INTO public.conversation(user_1,user_2) VALUES (?,?) RETURNING conversation_id", {actor,other});
    require(q.next(), "database_error"); return q.value(0).toInt();
}
QJsonArray Manager::attachments(int messageId) {
    auto q = query("SELECT id,filename,content_type,size_bytes FROM public.attachement WHERE message_id=? ORDER BY id", {messageId});
    QJsonArray result;
    while(q.next()) result.append(QJsonObject{{"id",q.value(0).toInt()},{"name",q.value(1).toString()},
        {"path",QString("db://attachment/%1").arg(q.value(0).toInt())},{"type",q.value(2).toString()},
        {"size",QString::number(q.value(3).toLongLong())+" bytes"}});
    return result;
}
QJsonObject Manager::message(QSqlQuery &q) {
    return {{"message_id",q.value(0).toInt()},{"messageBody",q.value(1).toString()},
        {"messageSender",q.value(2).toInt()},{"messageSentIn",q.value(3).toInt()},
        {"messageTime",q.value(4).toString()},{"attachs",attachments(q.value(0).toInt())}};
}
QJsonObject Manager::login(const QJsonObject &a) {
    const auto username = text(a,"username",128);
    const auto password = text(a,"password",1024);
    // Preserve the M1 password format for this migration; password hardening is separate.
    auto hash = QString::fromLatin1(QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha256).toHex());
    auto q = query("SELECT id FROM public.users WHERE username=? AND password_hash=?", {username,hash});
    require(q.next(), "invalid_credentials");
    return {{"user",profile(q.value(0).toInt())}};
}
QJsonObject Manager::registerUser(const QJsonObject &a) {
    const auto username = text(a,"username",128).trimmed();
    const auto nickname = text(a,"nickname",128).trimmed();
    const auto password = text(a,"password",1024);
    require(!username.contains(';') && !nickname.contains(';'), "invalid_request");
    require(database.transaction(), "database_error");
    try {
        // Serialize registrations across server instances without granting table ownership.
        query("SELECT pg_advisory_xact_lock(1145656131)");
        auto q = query("SELECT 1 FROM public.users WHERE username=? OR nickname=?",{username,nickname});
        require(!q.next(),"account_exists");
        const auto hash = QString::fromLatin1(QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha256).toHex());
        query("INSERT INTO public.users(username,password_hash,nickname,created_at,avatar) VALUES (?,?,?,CURRENT_DATE,':/avatars/default.png')",{username,hash,nickname});
        require(database.commit(),"database_error");
    } catch (...) { database.rollback(); throw; }
    return {};
}
QJsonObject Manager::sendMessage(int actor, const QJsonObject &a) {
    const auto body = text(a,"body",65536,true);
    const int other = id(a,"peer_id");
    require(a["attachments"].isArray(),"invalid_request");
    const auto files = a["attachments"].toArray();
    require(files.size() <= 8 && (!body.isEmpty() || !files.isEmpty()),"invalid_request");
    require(friends(actor,other),"forbidden");
    int conv = a["conversation_id"].toInt(-1);
    QJsonObject result;
    require(database.transaction(),"database_error");
    try {
        if(conv == -1) conv = conversation(actor,other,true);
        require(partner(actor,conv) == other,"forbidden");
        const auto time = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        auto q = query("INSERT INTO public.message(body,sent_by,sent_in,sent_at) VALUES (?,?,?,?) RETURNING id",{body,actor,conv,time});
        require(q.next(),"database_error"); const int mid = q.value(0).toInt();
        int total = 0;
        for(const auto &value : files) {
            require(value.isObject(),"invalid_file"); const auto f = value.toObject();
            const auto data = bytes(f,MaxFileBytes); total += data.size(); require(total <= MaxFileBytes,"file_too_large");
            const auto name = filename(f);
            query("INSERT INTO public.attachement(message_id,filename,content_type,size_bytes,data,file_data) VALUES (?,?,?,?,'',?)",
                {mid,name,QFileInfo(name).suffix(),data.size(),data});
        }
        q = query("SELECT id,body,sent_by,sent_in,sent_at FROM public.message WHERE id=?",{mid});
        require(q.next(),"database_error"); result = message(q);
        require(database.commit(),"database_error");
    } catch (...) { database.rollback(); throw; }
    auto event = profile(actor);
    for(auto it = result.begin();it != result.end();++it) event[it.key()] = it.value();
    event["type"] = "message";
    emit notifyUser(other,event);
    return result;
}
QJsonObject Manager::changeFriendship(int actor, const QString &op, const QJsonObject &a) {
    int other = 0;
    if(op == "friend_request") {
        auto q = query("SELECT id FROM public.users WHERE nickname=?",{text(a,"nickname",128)});
        require(q.next(),"not_found"); other = q.value(0).toInt(); require(other != actor,"invalid_request");
        q = query("SELECT status FROM public.friendship WHERE (user_id=? AND friend_id=?) OR (user_id=? AND friend_id=?)",{actor,other,other,actor});
        if(q.next()) return {{"status",q.value(0).toString()}};
        query("INSERT INTO public.friendship(user_id,friend_id,status,start_date) VALUES (?,?,'PENDING',CURRENT_DATE)",{actor,other});
    } else {
        other = id(a,"peer_id");
        const bool cancel = op == "friend_cancel";
        QSqlQuery q;
        if(op == "friend_accept") q = query("UPDATE public.friendship SET status='ACTIVE' WHERE user_id=? AND friend_id=? AND status='PENDING'",{other,actor});
        else q = query("DELETE FROM public.friendship WHERE user_id=? AND friend_id=? AND status='PENDING'",{cancel?actor:other,cancel?other:actor});
        require(q.numRowsAffected() > 0,"forbidden");
    }
    emit notifyUser(other,{{"type","relationships_changed"},{"id",actor}});
    return {{"status",op == "friend_request" ? "Request Sent" : "OK"}};
}
QJsonObject Manager::dispatch(int actor, const QString &op, const QJsonObject &a) {
    // Explicitly reject old client-controlled actor fields, including the old relay protocol.
    for(const auto &key : {"user_id","sender_id","messageSender","actor_id"}) require(!a.contains(key),"invalid_request");
    if(op == "login") return login(a);
    if(op == "register") return registerUser(a);
    if(op == "available") {
        const auto field = text(a,"field",16); require(field == "username" || field == "nickname","invalid_request");
        auto q = query("SELECT 1 FROM public.users WHERE "+field+"=?",{text(a,"value",128)});
        return {{"available",!q.next()}};
    }
    require(actor > 0,"authentication_required");
    if(op == "friends") {
        QJsonArray rows; for(int other : friendIds(actor)) rows.append(profile(other)); return {{"users",rows}};
    }
    if(op == "requests") {
        const auto direction = text(a,"direction",16); require(direction == "incoming" || direction == "outgoing","invalid_request");
        auto q = query(direction == "incoming" ? "SELECT user_id FROM public.friendship WHERE friend_id=? AND status='PENDING'" : "SELECT friend_id FROM public.friendship WHERE user_id=? AND status='PENDING'",{actor});
        QJsonArray rows; while(q.next()) rows.append(profile(q.value(0).toInt())); return {{"users",rows}};
    }
    if(op == "friend_request" || op == "friend_accept" || op == "friend_decline" || op == "friend_cancel") return changeFriendship(actor,op,a);
    if(op == "conversation") return {{"conversation_id",conversation(actor,id(a,"peer_id"),false)}};
    if(op == "conversations") {
        auto q = query("SELECT c.conversation_id,CASE WHEN c.user_1=? THEN c.user_2 ELSE c.user_1 END,m.body,m.sent_by FROM public.conversation c JOIN LATERAL (SELECT body,sent_by,id FROM public.message WHERE sent_in=c.conversation_id ORDER BY id DESC LIMIT 1) m ON true WHERE c.user_1=? OR c.user_2=? ORDER BY m.id DESC",{actor,actor,actor});
        QJsonArray rows;
        while(q.next()) rows.append(QJsonObject{{"conversation_id",q.value(0).toInt()},{"user",profile(q.value(1).toInt())},{"body",q.value(2).toString()},{"sender",q.value(3).toInt()}});
        return {{"conversations",rows}};
    }
    if(op == "messages") {
        const int conv = id(a,"conversation_id"); partner(actor,conv);
        const int after = a["after_id"].toInt(0); require(after >= 0,"invalid_request");
        auto q = query("SELECT id,body,sent_by,sent_in,sent_at FROM public.message WHERE sent_in=? AND id>? ORDER BY id LIMIT 100",{conv,after});
        QJsonArray rows; while(q.next()) rows.append(message(q)); return {{"messages",rows}};
    }
    if(op == "send_message") return sendMessage(actor,a);
    if(op == "avatar") {
        const int other = id(a,"peer_id"); require(related(actor,other),"forbidden");
        auto q = query("SELECT avatar_data,avatar_filename FROM public.users WHERE id=?",{other}); require(q.next(),"not_found");
        return {{"bytes",QString::fromLatin1(q.value(0).toByteArray().toBase64())},{"filename",q.value(1).toString()}};
    }
    if(op == "set_avatar") {
        require(!a.contains("peer_id"),"invalid_request");
        const auto data = bytes(a,2*1024*1024); require(!data.isEmpty(),"invalid_file");
        query("UPDATE public.users SET avatar='db://avatar/'||id::text,avatar_data=?,avatar_filename=? WHERE id=?",{data,filename(a),actor});
        return {};
    }
    if(op == "attachment") {
        const int attachmentId = id(a,"attachment_id");
        auto q = query("SELECT a.filename,a.file_data FROM public.attachement a JOIN public.message m ON m.id=a.message_id JOIN public.conversation c ON c.conversation_id=m.sent_in WHERE a.id=? AND (c.user_1=? OR c.user_2=?)",{attachmentId,actor,actor});
        require(q.next(),"forbidden");
        require(q.value(1).toByteArray().size() <= MaxFileBytes,"file_too_large");
        return {{"filename",q.value(0).toString()},{"bytes",QString::fromLatin1(q.value(1).toByteArray().toBase64())}};
    }
    throw std::runtime_error("unknown_operation");
}
