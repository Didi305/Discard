#include "mediacache.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSaveFile>
#include <QStandardPaths>

QString MediaCache::save(const QByteArray &bytes, const QString &category, qint64 id, const QString &filename) {
    QString suffix = QFileInfo(filename).suffix().toLower();
    suffix.remove(QRegularExpression("[^a-z0-9]"));
    if (suffix.isEmpty()) suffix = "bin";
    QDir root(QStandardPaths::writableLocation(QStandardPaths::CacheLocation));
    if (!root.mkpath("media/" + category)) return {};
    QString cacheName = QString::number(id);
    // Content-based avatar filenames also invalidate Qt's pixmap cache.
    if (category == "avatars")
        cacheName += "-" + QString::fromLatin1(QCryptographicHash::hash(bytes, QCryptographicHash::Sha256).toHex());
    const QString path = root.filePath(QString("media/%1/%2.%3").arg(category, cacheName, suffix));
    {
        QFile old(path);
        if (old.open(QIODevice::ReadOnly) && old.readAll() == bytes) return path;
    } // Close the old file before replacing it on Windows.
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size() || !file.commit()) {
        qWarning() << "Could not cache media:" << file.errorString();
        return {};
    }
    return path;
}
