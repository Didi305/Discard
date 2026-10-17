#pragma once
#include <QJsonArray>
#include <QString>
#include <QVector>

struct Attachement;

namespace AttachmentPayload {
struct Result {
    bool ok = false;
    QJsonArray attachments;
    QString errorCode;
};

Result prepare(const QVector<Attachement> &files);
}
