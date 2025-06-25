#include "helpers.h"
#include <QtSvg/QSvgRenderer>
#include <qgridlayout.h>
#include <qjsonobject.h>
#include <qlabel.h>
#include <qwidget.h>



QPixmap makeRoundedAvatar(const QPixmap &src, int s)
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

QWidget* makeAvatarWidget(const QPixmap &raw, int size, const QPixmap &statusIcon)
{
    // 1) Create a 1×1 grid so we can layer the status on top of the avatar
    auto *container = new QWidget;
    container->setFixedSize(size, size);

    auto *layout = new QGridLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // 2) The avatar label (fills the whole cell)
    auto *ava = new QLabel;
    ava->setFixedSize(size, size);
    ava->setPixmap(makeRoundedAvatar(raw, size));
    ava->setAlignment(Qt::AlignCenter);
    layout->addWidget(ava, 0, 0);

    // 3) The status label (small, bottom-right)
    int iconSize = size / 6;
    auto *status = new QLabel;
    status->setFixedSize(iconSize, iconSize);
    status->setPixmap(statusIcon.scaled(iconSize, iconSize,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation));
    // Place it in the same cell, but aligned bottom-right
    layout->addWidget(status, 0, 0, Qt::AlignBottom | Qt::AlignRight);

    return container;
}

QPixmap renderSvg(const QString &resourcePath, int sidePx)
{
    QPixmap canvas(sidePx, sidePx);
    canvas.fill(Qt::transparent);

    QSvgRenderer svg(resourcePath);          // needs Qt::Svg module
    QPainter p(&canvas);
    svg.render(&p);                          // draw entire SVG into pixmap
    return canvas;
}

QJsonObject parseJson(const QByteArray &data){
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();
    return obj;
}

QByteArray buildJson(const QMap<QString, QString> &fields){
    QJsonObject obj;
    for(auto it = fields.constBegin(); it != fields.constEnd(); ++it){
        obj[it.key()] = it.value();
    }
    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Compact);
}
