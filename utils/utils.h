#pragma once
#include <QtSvg/QSvgRenderer>
#include <QObject>
#include <QPainter>
#include <QStandardPaths>
#include <QPushButton>
#include <QHash>
#include <qdir.h>
#include <qfileinfo.h>
#include <qjsonarray.h>
#include <qjsonobject.h>
#include <qlayout.h>
#include <qpainterpath.h>
#include <qtcpsocket.h>
#include <QLabel>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/functional/hash.hpp>
using namespace boost::multi_index;

// tag names for the two indices
struct BySeq {};
struct ByKey {};
struct Attachement{
    QString name;
    QString path;
    QString type;
    QString size;
    bool isImage=false, isVideo=false;

};

struct Message {
    QString body;
    int senderID;
    int sentIn;
    QVector<Attachement> attachs;
    QString sentAt;
};



struct QStringHasher {
    std::size_t operator()(QString const &s) const noexcept {
        // Qt’s qHash(QString) returns a uint
        return static_cast<std::size_t>(qHash(s));
    }
};
using StringContainer = multi_index_container<
    Message,
    indexed_by<
        // 0) keep insertion order
        sequenced<tag<BySeq>>,
        // 1) hash‐lookup by Message::body, using your QStringHasher
        hashed_non_unique<
            tag<ByKey>,
            member<Message,QString,&Message::body>,
            QStringHasher,          // <- your functor here
            std::equal_to<QString>  // <- still use standard equality
            >
        >
    >;

inline QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Discard";

enum class Status {
    ONLINE, HIDDEN, DO_NOT_DISTURB
};


struct Data
{
    int user_id;
    QString displayName;
    QString avatar;
    Status status;
    Message message;
    QHash<Data, int> friends;
    bool operator==(Data const &other) const {
        return user_id == other.user_id;
    }
    void operator=(Data const &other) {
        user_id = other.user_id;
        displayName = other.displayName;
        avatar = other.avatar;
        status = other.status;
    }
};

static const QStringList kPickerEmojis = {
    "😀","😁","😂","🤣","😊","😍","😎","😭","😡","👍","👎","🙏",
    "👏","🔥","💯","🎉","❤️","💔","😅","🙃","🤔","😴","😇","🤗"
};

inline uint qHash(Data const &d, uint seed = 0) {
    // combine the hash of user_id (you can mix in other fields if needed)
    return ::qHash(d.user_id, seed);
}

namespace Utils {

static QString readText(const QString& p, const char* tag)
{
    QFile f(p);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {};
    }
    const auto s = QString::fromUtf8(f.readAll());
    return s;
}

static QString appConfigPath() {
    auto dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(dir);
    return dir + "/config.json";
}

static QJsonObject loadJson(const QString& pathOrQrc) {
    QFile f(pathOrQrc);
    if (!f.open(QIODevice::ReadOnly)) return {};
    auto doc = QJsonDocument::fromJson(f.readAll());
    return doc.isObject() ? doc.object() : QJsonObject{};
}

static QJsonObject loadAppConfig() {
    // 1) try user/machine override
    const QString diskCfg = appConfigPath();
    if (QFile::exists(diskCfg))
        return loadJson(diskCfg);

    // 2) fallback to baked-in defaults
    auto cfg = loadJson(":/config.json");

    // 3) optionally materialize a template on disk the first run
    QFile out(diskCfg);
    if (out.open(QIODevice::WriteOnly))
        out.write(QJsonDocument(cfg).toJson(QJsonDocument::Indented));

    return cfg;
}

inline QPixmap makeAvatarWithHole(const QPixmap &src, int diameter, QSvgRenderer *statusRenderer) {
    // size of the little status slot
    const int slot = diameter / 4;
    const int total = diameter + slot;

    // 1) start with a transparent canvas
    QPixmap out(diameter, diameter);
    out.fill(Qt::transparent);

    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing);

    // 2) define the avatar circle minus the status hole
    QPainterPath avatarClip;
    avatarClip.addEllipse(0, 0, diameter, diameter);

    // hole path: circle of radius slot/2 in the bottom-right corner
    const int r = slot/2;
    const int cx = diameter / 2 + diameter / 3;
    const int cy = diameter - diameter/5 ;
    QPainterPath hole;
    hole.addEllipse(cx - r, cy - r, 2*r, 2*r);
    // subtract the hole from the avatar clip
    avatarClip = avatarClip.subtracted(hole);
    p.setClipPath(avatarClip);

    // 3) draw the avatar scaled to fill that clipped region
    QPixmap scaled = src.scaled(diameter, diameter,
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation);
    int dx = (scaled.width()  - diameter) / 2;
    int dy = (scaled.height() - diameter) / 2;
    p.drawPixmap(-dx, -dy, scaled);
    // 4) turn clipping off and draw the status icon in the hole
    p.setClipping(false);
    if (statusRenderer) {
        statusRenderer->render(&p, QRectF(cx + 1 - r, cy + 1 - r, 10, 10));
    } else {
        p.setBrush(Qt::green);
        p.setPen(Qt::NoPen);
        p.drawEllipse(QPoint(cx, cy), r, r);
    }

    return out;
}

inline QPixmap makeRoundedAvatar(const QPixmap &src, int s)
{
    /* 1. scale so the shorter edge still reaches ‘s’ */
    QPixmap scaled = src.scaled(s, s,
                                Qt::KeepAspectRatioByExpanding,
                                Qt::SmoothTransformation);

    /* 2. paint the centre square through a circular clip */
    QPixmap out(s, s);
    out.fill(Qt::transparent);

    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing);

    QPainterPath clip;
    clip.addEllipse(0, 0, s, s);
    p.setClipPath(clip);

    /* centre the bigger pixmap */
    const int dx = (scaled.width()  - s) / 2;
    const int dy = (scaled.height() - s) / 2;
    p.drawPixmap(-dx, -dy, scaled);      // negative offset → crop evenly
    return out;
}
inline QPixmap renderSvg(const QString &resourcePath, int sidePx)
{
    QPixmap canvas(sidePx, sidePx);
    canvas.fill(Qt::transparent);

    QSvgRenderer svg(resourcePath);          // needs Qt::Svg module
    QPainter p(&canvas);
    svg.render(&p);                          // draw entire SVG into pixmap
    return canvas;
}

inline static const QHash<QString,Status>& statusMap() {
    static QHash<QString,Status> m = {
        { "ONLINE",         Status::ONLINE        },
        { "HIDDEN",         Status::HIDDEN        },
        { "DO_NOT_DISTURB", Status::DO_NOT_DISTURB }
    };
    return m;
}

inline Status statusFromString(const QString &str) {
    return statusMap().value(str.toUpper(), Status::HIDDEN);
}

inline QString stringFromStatus(const Status stat) {
    QString status;
    for (auto it = statusMap().constBegin(); it != statusMap().constEnd(); ++it) {
        if (it.value() == stat) {
            status = it.key().toUpper();
            break; // Stops at the first match
        }
    }
    return status;
}

static QJsonValue attachsToJson(QVector<Attachement> attachs){
    QJsonArray attachsArray;
    QJsonObject attachObj;
    for (auto a : attachs){
        attachObj["name"] = a.name;
        attachObj["path"] = a.path;
        attachObj["size"] = a.size;
        attachObj["type"] = a.type;
        attachObj["isImage"] = a.isImage;
        attachObj["isVideo"] = a.isVideo;
        attachsArray.append(attachObj);
    }
    QJsonValue attachsValue(attachsArray);

    return attachsValue;
}

static QVector<Attachement> attachsJsonToData(QJsonArray attachs){
    QVector<Attachement> attachements;
    Attachement attach;
    for (auto a : attachs){
        attach.name = a.toObject()["name"].toString();
        attach.path = a.toObject()["path"].toString();
        attach.size = a.toObject()["size"].toString();
        attach.type = a.toObject()["type"].toString();
        attach.isImage = a.toObject()["isImage"].toBool();
        attach.isVideo = a.toObject()["isVideo"].toBool();
        attachements.append(attach);
    }
    return attachements;
}

inline QByteArray jsonObjectToData(const QJsonObject &obj){
    QJsonDocument doc(obj);
    QString jsonString = doc.toJson(QJsonDocument::Compact);
    jsonString.append('\n');
    return jsonString.toUtf8();
}

inline QJsonObject parseJson(const QByteArray &data){
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    return obj;
}

inline QJsonObject buildJson(const Data &d){
    QJsonObject obj;

    obj["id"] = d.user_id;
    obj["name"] = d.displayName;
    obj["avatar"] = d.avatar;
    obj["status"] = stringFromStatus(d.status);
    obj["messageBody"] = d.message.body;
    obj["messageSender"] = d.message.senderID;
    obj["messageSentIn"] = d.message.sentIn;
    obj["attachs"] = attachsToJson(d.message.attachs);
    obj["messageTime"] = d.message.sentAt;
    return obj;
}
inline Data buildData(const QJsonObject &obj){
    Data d;
    d.user_id = obj["id"].toInt();
    d.displayName = obj["name"].toString();
    d.avatar = obj["avatar"].toString();
    d.status = statusFromString(obj["status"].toString());
    d.message.body = obj["messageBody"].toString();
    d.message.senderID = obj["messageSender"].toInt();
    d.message.sentIn = obj["messageSentIn"].toInt();
    d.message.attachs = attachsJsonToData(obj["attachs"].toArray());
    d.message.sentAt = obj["messageTime"].toString();
    return d;
}



inline QByteArray updateStatus(int user_id, bool online){
    QJsonObject stat;
    stat["type"] = "friend_update";
    stat["id"] = user_id;
    stat["bool"] = online;
    QJsonDocument doc(stat);
    QString payload = doc.toJson(QJsonDocument::Compact);
    payload.append('\n');
    return payload.toUtf8();
}
static QString toHtml(const QString &plain)
{
    return plain.toHtmlEscaped().replace('\n', "<br/>");
}
static QString wrapEmojisForHtml(QString htmlEscaped)
{
    for (const QString &e : kPickerEmojis) {
        htmlEscaped.replace(e, "<span class=\"emoji\">" + e + "</span>");
    }
    return htmlEscaped;
}
static QString embedAvatar(QTextDocument *doc, const QPixmap &px)
{
    const QString id = QStringLiteral("ava_%1").arg((qulonglong)px.cacheKey());
    if (!doc->resource(QTextDocument::ImageResource, QUrl(id)).isValid())
        doc->addResource(QTextDocument::ImageResource, QUrl(id), px);
    return id;
}

static void deleteChildren(QLayout* lay){
    while (auto* item = lay->takeAt(0)){
        item->widget()->deleteLater();
        delete item;
    }
}
}
