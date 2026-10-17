#include "manager.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QSqlError>
#include <QUuid>
#include <stdexcept>

namespace {
constexpr int MaxFileBytes = 8 * 1024 * 1024;
void require(bool ok, const char *error) {
    if (!ok) {
        throw std::runtime_error(error);
    }
}

QString text(const QJsonObject &object, const char *key, int maximum, bool allowEmpty = false) {
    require(object.value(key).isString(), "invalid_request");
    const auto fieldText = object.value(key).toString();
    require(fieldText.size() <= maximum && (allowEmpty || !fieldText.trimmed().isEmpty()) &&
                !fieldText.contains(QChar::Null),
            "invalid_request");
    return fieldText;
}

int positiveId(const QJsonObject &object, const char *key) {
    const auto value = object.value(key);
    const int numericId = value.toInt(-1);
    require(value.isDouble() && numericId > 0 && value.toDouble() == numericId, "invalid_request");
    return numericId;
}

QByteArray decodeFileBytes(const QJsonObject &object, int maximum) {
    const auto value = text(object, "bytes", (maximum + 2) / 3 * 4, true).toLatin1();
    const auto decoded =
        QByteArray::fromBase64Encoding(value, QByteArray::AbortOnBase64DecodingErrors);
    require(bool(decoded) && decoded.decoded.size() <= maximum, "invalid_file");
    return decoded.decoded;
}

QString attachmentFileName(const QJsonObject &object) {
    auto fileName = text(object, "filename", 255);
    fileName.replace('\\', '/');
    fileName = fileName.section('/', -1);
    require(!fileName.isEmpty() && fileName != "." && fileName != "..", "invalid_file");
    return fileName;
}

} // namespace

Manager::Manager(QObject *parent)
    : QObject(parent),
      connectionName(QUuid::createUuid().toString()) {
}

Manager::~Manager() {
    database.close();
    database = QSqlDatabase();
    QSqlDatabase::removeDatabase(connectionName);
}

bool Manager::init(const QString &configPath, QString &error) {
    QFile config(configPath);
    if (configPath.startsWith(':') || !config.open(QIODevice::ReadOnly) || config.size() > 65536) {
        error = "Cannot read the external database configuration.";
        return false;
    }
#ifdef Q_OS_UNIX
    if (config.permissions() &
        (QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup |
         QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther)) {
        error = "Database configuration must be private (chmod 600).";
        return false;
    }
#endif
    QJsonParseError parseError;
    const auto document = QJsonDocument::fromJson(config.readAll(), &parseError);
    const auto configuration = document.object();
    if (parseError.error != QJsonParseError::NoError || !document.isObject() ||
        configuration["host"].toString().isEmpty() ||
        configuration["database"].toString().isEmpty() ||
        configuration["user"].toString().isEmpty() ||
        configuration["password"].toString().isEmpty() || configuration["port"].toInt() < 1 ||
        configuration["port"].toInt() > 65535) {
        error = "Invalid database configuration.";
        return false;
    }
    database = QSqlDatabase::addDatabase("QPSQL", connectionName);
    database.setHostName(configuration["host"].toString());
    database.setPort(configuration["port"].toInt());
    database.setDatabaseName(configuration["database"].toString());
    database.setUserName(configuration["user"].toString());
    database.setPassword(configuration["password"].toString());
    database.setConnectOptions("connect_timeout=5");
    if (!database.open()) {
        error = "Cannot connect to PostgreSQL. Check server configuration and service.";
        return false;
    }
    try {
        query("SET statement_timeout = '5s'");
        query("SET search_path = pg_catalog, public");
    } catch (...) {
        error = "Could not initialize database session.";
        return false;
    }
    return true;
}

QSqlQuery Manager::query(const QString &sql, const QVariantList &values) {
    QSqlQuery queryResult(database);
    require(queryResult.prepare(sql), "database_error");
    for (const auto &v : values) {
        queryResult.addBindValue(v);
    }
    require(queryResult.exec(), "database_error");
    return queryResult;
}

QJsonObject Manager::profile(int userId) {
    auto queryResult =
        query("SELECT id,nickname,status::text FROM public.users WHERE id=?", {userId});
    require(queryResult.next(), "not_found");
    return {{"id", queryResult.value(0).toInt()},
            {"name", queryResult.value(1).toString()},
            {"status", queryResult.value(2).toString()},
            {"avatar", QString("db://avatar/%1").arg(userId)}};
}

bool Manager::friends(int actorId, int peerId) {
    auto queryResult = query(
        "SELECT 1 FROM public.friendship WHERE status='ACTIVE' AND ((user_id=? AND friend_id=?) OR "
        "(user_id=? AND friend_id=?))",
        {actorId, peerId, peerId, actorId});
    return queryResult.next();
}

bool Manager::related(int actorId, int peerId) {
    if (actorId == peerId) {
        return true;
    }
    auto queryResult = query(
        "SELECT 1 FROM public.friendship WHERE (user_id=? AND friend_id=?) OR (user_id=? AND "
        "friend_id=?) UNION ALL SELECT 1 FROM public.conversation WHERE (user_1=? AND user_2=?) OR "
        "(user_1=? AND user_2=?) LIMIT 1",
        {actorId, peerId, peerId, actorId, actorId, peerId, peerId, actorId});
    return queryResult.next();
}

QList<int> Manager::friendIds(int actorId) {
    auto queryResult = query(
        "SELECT CASE WHEN user_id=? THEN friend_id ELSE user_id END FROM public.friendship WHERE "
        "status='ACTIVE' AND (user_id=? OR friend_id=?)",
        {actorId, actorId, actorId});
    QList<int> result;
    while (queryResult.next()) {
        result.append(queryResult.value(0).toInt());
    }
    return result;
}

int Manager::partner(int actorId, int conversationId) {
    auto queryResult = query(
        "SELECT user_1,user_2 FROM public.conversation WHERE conversation_id=? AND (user_1=? OR user_2=?)",
        {conversationId, actorId, actorId});
    require(queryResult.next(), "forbidden");
    return queryResult.value(0).toInt() == actorId ? queryResult.value(1).toInt()
                                                   : queryResult.value(0).toInt();
}

int Manager::conversation(int actorId, int peerId, bool create) {
    require(actorId != peerId, "invalid_request");
    auto queryResult = query(
        "SELECT conversation_id FROM public.conversation WHERE (user_1=? AND user_2=?) OR (user_1=? "
        "AND user_2=?) ORDER BY conversation_id LIMIT 1",
        {actorId, peerId, peerId, actorId});
    if (queryResult.next()) {
        return queryResult.value(0).toInt();
    }
    if (!create) {
        return -1;
    }
    require(friends(actorId, peerId), "forbidden");
    queryResult = query(
        "INSERT INTO public.conversation(user_1,user_2) VALUES (?,?) RETURNING conversation_id",
        {actorId, peerId});
    require(queryResult.next(), "database_error");
    return queryResult.value(0).toInt();
}
QJsonArray Manager::attachments(int messageId) {
    auto queryResult = query(
        "SELECT id,filename,content_type,size_bytes FROM public.attachement WHERE message_id=? ORDER BY id",
        {messageId});
    QJsonArray result;
    while (queryResult.next()) {
        result.append(
            QJsonObject{{"id", queryResult.value(0).toInt()},
                        {"name", queryResult.value(1).toString()},
                        {"path", QString("db://attachment/%1").arg(queryResult.value(0).toInt())},
                        {"type", queryResult.value(2).toString()},
                        {"size", QString::number(queryResult.value(3).toLongLong()) + " bytes"}});
    }
    return result;
}

QJsonObject Manager::message(QSqlQuery &queryResult) {
    return {{"message_id", queryResult.value(0).toInt()},
            {"messageBody", queryResult.value(1).toString()},
            {"messageSender", queryResult.value(2).toInt()},
            {"messageSentIn", queryResult.value(3).toInt()},
            {"messageTime", queryResult.value(4).toString()},
            {"attachs", attachments(queryResult.value(0).toInt())}};
}

QJsonObject Manager::login(const QJsonObject &arguments) {
    const auto username = text(arguments, "username", 128);
    const auto password = text(arguments, "password", 1024);
    // Preserve the M1 password format for this migration; password hardening is separate.
    auto hash = QString::fromLatin1(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
    auto queryResult =
        query("SELECT id FROM public.users WHERE username=? AND password_hash=?", {username, hash});
    require(queryResult.next(), "invalid_credentials");
    return {{"user", profile(queryResult.value(0).toInt())}};
}

QJsonObject Manager::registerUser(const QJsonObject &arguments) {
    const auto username = text(arguments, "username", 128).trimmed();
    const auto nickname = text(arguments, "nickname", 128).trimmed();
    const auto password = text(arguments, "password", 1024);
    require(!username.contains(';') && !nickname.contains(';'), "invalid_request");
    require(database.transaction(), "database_error");
    try {
        // Serialize registrations across server instances without granting table ownership.
        query("SELECT pg_advisory_xact_lock(1145656131)");
        auto queryResult = query("SELECT 1 FROM public.users WHERE username=? OR nickname=?",
                                 {username, nickname});
        require(!queryResult.next(), "account_exists");
        const auto hash = QString::fromLatin1(
            QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
        query(
            "INSERT INTO public.users(username,password_hash,nickname,created_at,avatar) VALUES (?,?,?,"
            "CURRENT_DATE,':/avatars/default.png')",
            {username, hash, nickname});
        require(database.commit(), "database_error");
    } catch (...) {
        database.rollback();
        throw;
    }
    return {};
}

QJsonObject Manager::sendMessage(int actorId, const QJsonObject &arguments) {
    const auto body = text(arguments, "body", 65536, true);
    const int peerId = positiveId(arguments, "peer_id");
    require(arguments["attachments"].isArray(), "invalid_request");
    const auto files = arguments["attachments"].toArray();
    require(files.size() <= 8 && (!body.isEmpty() || !files.isEmpty()), "invalid_request");
    require(friends(actorId, peerId), "forbidden");
    int conversationId = arguments["conversation_id"].toInt(-1);
    QJsonObject result;
    require(database.transaction(), "database_error");
    try {
        if (conversationId == -1) {
            conversationId = conversation(actorId, peerId, true);
        }
        require(partner(actorId, conversationId) == peerId, "forbidden");
        const auto sentAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        auto queryResult = query(
            "INSERT INTO public.message(body,sent_by,sent_in,sent_at) VALUES (?,?,?,?) RETURNING id",
            {body, actorId, conversationId, sentAt});
        require(queryResult.next(), "database_error");
        const int messageId = queryResult.value(0).toInt();
        int totalAttachmentBytes = 0;
        for (const auto &value : files) {
            require(value.isObject(), "invalid_file");
            const auto fileObject = value.toObject();
            const auto fileBytes = decodeFileBytes(fileObject, MaxFileBytes);
            totalAttachmentBytes += fileBytes.size();
            require(totalAttachmentBytes <= MaxFileBytes, "file_too_large");
            const auto fileName = attachmentFileName(fileObject);
            query(
                "INSERT INTO public.attachement(message_id,filename,content_type,size_bytes,data,file_data) "
                "VALUES (?,?,?,?,'',?)",
                {messageId, fileName, QFileInfo(fileName).suffix(), fileBytes.size(), fileBytes});
        }
        queryResult = query("SELECT id,body,sent_by,sent_in,sent_at FROM public.message WHERE id=?",
                            {messageId});
        require(queryResult.next(), "database_error");
        result = message(queryResult);
        require(database.commit(), "database_error");
    } catch (...) {
        database.rollback();
        throw;
    }
    auto event = profile(actorId);
    for (auto fieldIterator = result.begin(); fieldIterator != result.end(); ++fieldIterator) {
        event[fieldIterator.key()] = fieldIterator.value();
    }
    event["type"] = "message";
    emit notifyUser(peerId, event);
    return result;
}

QJsonObject
Manager::changeFriendship(int actorId, const QString &operation, const QJsonObject &arguments) {
    int peerId = 0;
    if (operation == "friend_request") {
        auto queryResult = query("SELECT id FROM public.users WHERE nickname=?",
                                 {text(arguments, "nickname", 128)});
        require(queryResult.next(), "not_found");
        peerId = queryResult.value(0).toInt();
        require(peerId != actorId, "invalid_request");
        queryResult = query(
            "SELECT status FROM public.friendship WHERE (user_id=? AND friend_id=?) OR (user_id=? AND "
            "friend_id=?)",
            {actorId, peerId, peerId, actorId});
        if (queryResult.next()) {
            return {{"status", queryResult.value(0).toString()}};
        }
        query(
            "INSERT INTO public.friendship(user_id,friend_id,status,start_date) VALUES (?,?,'PENDING',"
            "CURRENT_DATE)",
            {actorId, peerId});
    } else {
        peerId = positiveId(arguments, "peer_id");
        const bool cancel = operation == "friend_cancel";
        QSqlQuery queryResult;
        if (operation == "friend_accept") {
            queryResult = query(
                "UPDATE public.friendship SET status='ACTIVE' WHERE user_id=? AND friend_id=? AND status='PENDING'",
                {peerId, actorId});
        } else {
            queryResult = query(
                "DELETE FROM public.friendship WHERE user_id=? AND friend_id=? AND status='PENDING'",
                {cancel ? actorId : peerId, cancel ? peerId : actorId});
        }
        require(queryResult.numRowsAffected() > 0, "forbidden");
    }
    emit notifyUser(peerId, {{"type", "relationships_changed"}, {"id", actorId}});
    return {{"status", operation == "friend_request" ? "Request Sent" : "OK"}};
}

QJsonObject Manager::dispatch(int actorId, const QString &operation, const QJsonObject &arguments) {
    // Explicitly reject old client-controlled actor fields, including the old relay protocol.
    for (const auto &key : {"user_id", "sender_id", "messageSender", "actor_id"}) {
        require(!arguments.contains(key), "invalid_request");
    }
    if (operation == "login") {
        return login(arguments);
    }
    if (operation == "register") {
        return registerUser(arguments);
    }
    if (operation == "available") {
        const auto field = text(arguments, "field", 16);
        require(field == "username" || field == "nickname", "invalid_request");
        auto queryResult = query("SELECT 1 FROM public.users WHERE " + field + "=?",
                                 {text(arguments, "value", 128)});
        return {{"available", !queryResult.next()}};
    }
    require(actorId > 0, "authentication_required");
    if (operation == "friends") {
        QJsonArray rows;
        for (int peerId : friendIds(actorId)) {
            rows.append(profile(peerId));
        }
        return {{"users", rows}};
    }
    if (operation == "requests") {
        const auto direction = text(arguments, "direction", 16);
        require(direction == "incoming" || direction == "outgoing", "invalid_request");
        auto queryResult = query(
            direction == "incoming"
                ? "SELECT user_id FROM public.friendship WHERE friend_id=? AND status='PENDING'"
                : "SELECT friend_id FROM public.friendship WHERE user_id=? AND status='PENDING'",
            {actorId});
        QJsonArray rows;
        while (queryResult.next()) {
            rows.append(profile(queryResult.value(0).toInt()));
        }
        return {{"users", rows}};
    }
    if (operation == "friend_request" || operation == "friend_accept" ||
        operation == "friend_decline" || operation == "friend_cancel") {
        return changeFriendship(actorId, operation, arguments);
    }
    if (operation == "conversation") {
        return {
            {"conversation_id", conversation(actorId, positiveId(arguments, "peer_id"), false)}};
    }
    if (operation == "conversations") {
        auto queryResult = query(
            "SELECT c.conversation_id,CASE WHEN c.user_1=? THEN c.user_2 ELSE c.user_1 END,m.body,"
            "m.sent_by FROM public.conversation c JOIN LATERAL (SELECT body,sent_by,id FROM "
            "public.message WHERE sent_in=c.conversation_id ORDER BY id DESC LIMIT 1) m ON true WHERE "
            "c.user_1=? OR c.user_2=? ORDER BY m.id DESC",
            {actorId, actorId, actorId});
        QJsonArray rows;
        while (queryResult.next()) {
            rows.append(QJsonObject{{"conversation_id", queryResult.value(0).toInt()},
                                    {"user", profile(queryResult.value(1).toInt())},
                                    {"body", queryResult.value(2).toString()},
                                    {"sender", queryResult.value(3).toInt()}});
        }
        return {{"conversations", rows}};
    }
    if (operation == "messages") {
        const int conversationId = positiveId(arguments, "conversation_id");
        partner(actorId, conversationId);
        const int after = arguments["after_id"].toInt(0);
        require(after >= 0, "invalid_request");
        auto queryResult = query(
            "SELECT id,body,sent_by,sent_in,sent_at FROM public.message WHERE sent_in=? AND id>? ORDER "
            "BY id LIMIT 100",
            {conversationId, after});
        QJsonArray rows;
        while (queryResult.next()) {
            rows.append(message(queryResult));
        }
        return {{"messages", rows}};
    }
    if (operation == "send_message") {
        return sendMessage(actorId, arguments);
    }
    if (operation == "avatar") {
        const int peerId = positiveId(arguments, "peer_id");
        require(related(actorId, peerId), "forbidden");
        auto queryResult =
            query("SELECT avatar_data,avatar_filename FROM public.users WHERE id=?", {peerId});
        require(queryResult.next(), "not_found");
        return {{"bytes", QString::fromLatin1(queryResult.value(0).toByteArray().toBase64())},
                {"filename", queryResult.value(1).toString()}};
    }
    if (operation == "set_avatar") {
        require(!arguments.contains("peer_id"), "invalid_request");
        const auto fileBytes = decodeFileBytes(arguments, 2 * 1024 * 1024);
        require(!fileBytes.isEmpty(), "invalid_file");
        query(
            "UPDATE public.users SET avatar='db://avatar/'||id::text,avatar_data=?,avatar_filename=? WHERE id=?",
            {fileBytes, attachmentFileName(arguments), actorId});
        return {};
    }
    if (operation == "attachment") {
        const int attachmentId = positiveId(arguments, "attachment_id");
        auto queryResult = query(
            "SELECT a.filename,a.file_data FROM public.attachement a JOIN public.message m ON "
            "m.id=a.message_id JOIN public.conversation c ON c.conversation_id=m.sent_in WHERE a.id=? "
            "AND (c.user_1=? OR c.user_2=?)",
            {attachmentId, actorId, actorId});
        require(queryResult.next(), "forbidden");
        require(queryResult.value(1).toByteArray().size() <= MaxFileBytes, "file_too_large");
        return {{"filename", queryResult.value(0).toString()},
                {"bytes", QString::fromLatin1(queryResult.value(1).toByteArray().toBase64())}};
    }
    throw std::runtime_error("unknown_operation");
}
