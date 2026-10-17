#include "attachmentpayload.h"
#include "utils.h"
#include <QFile>
#include <QFileInfo>
#include <QJsonObject>

AttachmentPayload::Result AttachmentPayload::prepare(const QVector<Attachement> &files) {
    constexpr int MaximumFiles = 8;
    constexpr qint64 MaximumBytes = 8 * 1024 * 1024;
    if (files.size() > MaximumFiles) return {false, {}, "too_many_attachments"};

    QJsonArray attachments;
    qint64 total = 0;
    for (const auto &attachment : files) {
        QFile file(attachment.path);
        if (!file.open(QIODevice::ReadOnly)) return {false, {}, "attachment_read_failed"};
        const qint64 remaining = MaximumBytes - total;
        if (file.size() > remaining) return {false, {}, "attachments_too_large"};
        // Read at most one byte beyond the limit, even if the file grows.
        const auto bytes = file.read(remaining + 1);
        if (file.error() != QFileDevice::NoError) return {false, {}, "attachment_read_failed"};
        if (bytes.size() > remaining) return {false, {}, "attachments_too_large"};
        total += bytes.size();
        attachments.append(QJsonObject{
            {"filename", QFileInfo(attachment.path).fileName()},
            {"bytes", QString::fromLatin1(bytes.toBase64())}
        });
    }
    return {true, attachments, {}};
}
