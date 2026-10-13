#pragma once
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QSaveFile>
#include <QStandardPaths>
#include <QDir>

namespace AttachmentActions {
inline void saveAs(QWidget *parent, const QString &source, const QString &displayName) {
    const QFileInfo info(source);
    QString name = displayName;
    name.replace('\\', '/');
    name = name.section('/', -1);
    if (name.isEmpty() || name == "." || name == "..") name = info.fileName();
    const QString target = QFileDialog::getSaveFileName(parent, QObject::tr("Save attachment"),
        QDir(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).filePath(name));
    if (target.isEmpty()) return;
    if (QFileInfo(target).canonicalFilePath() == info.canonicalFilePath() && info.exists()) return;
    QFile input(source);
    QSaveFile output(target);
    if (!input.open(QIODevice::ReadOnly) || !output.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(parent, QObject::tr("Cannot save attachment"),
                             QObject::tr("The source file could not be read or the destination could not be opened."));
        return;
    }
    while (!input.atEnd()) {
        const QByteArray chunk = input.read(64 * 1024);
        if (input.error() != QFileDevice::NoError || output.write(chunk) != chunk.size()) {
            output.cancelWriting();
            QMessageBox::warning(parent, QObject::tr("Cannot save attachment"), QObject::tr("Copying the file failed."));
            return;
        }
    }
    if (!output.commit())
        QMessageBox::warning(parent, QObject::tr("Cannot save attachment"), output.errorString());
}
}
