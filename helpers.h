#ifndef HELPERS_H
#define HELPERS_H
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>

QPixmap makeRoundedAvatar(const QPixmap &src, int size);
QWidget* makeAvatarWidget(const QPixmap &raw, int size, const QPixmap &statusIcon);
QPixmap renderSvg(const QString &resourcePath, int sidePx);
void updateAvatar(const QString &fileName);
void updateNickname(const QString &nick);
QJsonObject parseJson(const QByteArray &data);
QByteArray buildJson(const QMap<QString, QString> &fields);

#endif // HELPERS_H
