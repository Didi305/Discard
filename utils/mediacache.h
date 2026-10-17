#pragma once
#include <QByteArray>
#include <QString>
#include <QtGlobal>

namespace MediaCache {
QString save(const QByteArray &bytes, const QString &category, qint64 id, const QString &filename);
}
