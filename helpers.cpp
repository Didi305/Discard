#include "helpers.h"
#include <QtSvg/QSvgRenderer>
#include <qjsonobject.h>



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
