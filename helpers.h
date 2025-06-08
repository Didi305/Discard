#ifndef HELPERS_H
#define HELPERS_H
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>

QPixmap makeRoundedAvatar(const QPixmap &src, int size = 40);
QPixmap renderSvg(const QString &resourcePath, int sidePx);
#endif // HELPERS_H
