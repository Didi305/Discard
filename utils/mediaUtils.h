#pragma once

#include "attachementchip.h"
#include "utils.h"
#include <QImageReader>
#include <QMimeDatabase>
#include <QFileIconProvider>
#include <QBuffer>
#include <QMediaPlayer>
#include <QVideoSink>
#include <QVideoFrame>
#include <QEventLoop>
#include <QTimer>
#include <QWindow>
#include <QScreen>
#include <qdir.h>
#include <qtextdocument.h>
#include <QSize>
#include <qurlquery.h>
struct Att { QString path, name, size, id; bool isImage=false, isVideo=false; };

namespace mediaUtils {
static QPixmap addCenteredPlayBadge(const QPixmap& src)
{
    if (src.isNull()) return src;
    QPixmap out = src.copy();
    QPainter p(&out);
    p.setRenderHint(QPainter::Antialiasing, true);

    // badge size relative to thumb
    const int d = qMax(28, out.width() / 5);           // circle diameter
    const QPoint c(out.width()/2, out.height()/2);     // center

    // soft dark circle
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 170));
    p.drawEllipse(QRect(c - QPoint(d/2, d/2), QSize(d, d)));

    // white play triangle
    const int tw = d * 3 / 7;   // triangle width
    const int th = d * 4 / 7;   // triangle height
    QPolygonF tri;
    tri << QPointF(c.x() - tw/2.0, c.y() - th/2.0)
        << QPointF(c.x() + tw/2.0, c.y())
        << QPointF(c.x() - tw/2.0, c.y() + th/2.0);
    p.setBrush(Qt::white);
    p.drawPolygon(tri);

    return out;
}

static bool isImagePath(const QString& p) {
    QImageReader r(p); r.setDecideFormatFromContent(true); return r.canRead();
}

static bool isVideoPath(const QString& p) {
    QMimeDatabase db; return db.mimeTypeForFile(p).name().startsWith("video/");
}

static bool isGifPath(const QString& p) {
    return p.endsWith(".gif", Qt::CaseInsensitive);
}

static QPixmap grabVideoThumb(const QString& path, const QSize& sz = {256,256}, int timeoutMs = 800)
{
    QMediaPlayer player;
    QVideoSink   sink;
    player.setVideoSink(&sink);
    player.setSource(QUrl::fromLocalFile(path));

    QImage frame;
    QEventLoop loop;
    QObject::connect(&sink, &QVideoSink::videoFrameChanged, &loop, [&](const QVideoFrame& vf){
        QImage img = vf.toImage();
        if (!img.isNull()) { frame = img; loop.quit(); }
    });
    QTimer::singleShot(timeoutMs, &loop, &QEventLoop::quit);

    player.play();
    loop.exec();
    player.stop();

    if (frame.isNull()) return QPixmap();

    if (!sz.isEmpty())
        frame = frame.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return QPixmap::fromImage(frame);
}

static QString registerPixmap(QTextDocument* doc, const QPixmap& pm, const QString& key) {
    const QUrl url(QStringLiteral("att:%1").arg(key));
    if (!doc->resource(QTextDocument::ImageResource, url).isValid())
        doc->addResource(QTextDocument::ImageResource, url, pm);
    return url.toString();
}

static QPixmap makeImageThumbCrisp(const QString& path,
                                   const QSize& tile,   // e.g. {240,180} logical
                                   qreal dpr)           // view->devicePixelRatioF()
{
    QImageReader r(path);
    r.setDecideFormatFromContent(true);
    r.setAutoTransform(true);

    const QSize targetPx = (QSizeF(tile) * dpr).toSize();       // physical px
    if (r.size().isValid()) {
        QSize s = r.size();
        s.scale(targetPx, Qt::KeepAspectRatio);
        r.setScaledSize(s);                                     // decode near target
    }

    QImage img = r.read();
    if (img.isNull()) return {};

    img = img.scaled(targetPx, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap pm = QPixmap::fromImage(img);
    pm.setDevicePixelRatio(dpr);                                // CRUCIAL
    return pm;
}

static QPixmap makeSystemIcon(const QString& path, const QSize& display, qreal dpr) {
    QFileIconProvider ip; QIcon ic = ip.icon(QFileInfo(path));
    QPixmap pm = ic.pixmap((QSizeF(display)*dpr).toSize()); pm.setDevicePixelRatio(dpr); return pm;
}

static QPixmap makeIconAt(const QString& path, const QSize& displaySize, qreal dpr)
{
    QFileIconProvider ip;
    QIcon ic = ip.icon(QFileInfo(path));
    QSize targetPx = (QSizeF(displaySize) * dpr).toSize();
    QPixmap pm = ic.pixmap(targetPx);                         // request real size
    pm.setDevicePixelRatio(dpr);
    return pm;
}
static QPixmap tryVideoThumb(const QString& path, const QSize& display, qreal dpr, int timeoutMs = 800)
{
    QMediaPlayer player;
    QVideoSink   sink;
    player.setVideoSink(&sink);
    player.setSource(QUrl::fromLocalFile(path));

    QImage frameImg;
    QEventLoop loop;
    QObject::connect(&sink, &QVideoSink::videoFrameChanged, &loop,
                     [&](const QVideoFrame& vf){
                         if (!vf.isValid()) return;
                         QImage img = QVideoFrame(vf).toImage();
                         if (!img.isNull()) {
                             const QSize target = (QSizeF(display)*dpr).toSize();
                             img = img.scaled(target, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                             frameImg = img;
                             loop.quit();
                         }
                     });

    QTimer::singleShot(timeoutMs, &loop, &QEventLoop::quit);
    player.play();
    loop.exec();
    player.stop();

    if (frameImg.isNull()) return {};
    QPixmap pm = QPixmap::fromImage(frameImg);
    pm.setDevicePixelRatio(dpr);
    return pm;
}

static QString humanReadableSize(quint64 bytes)
{
    static const char* units[] = {"B","KB","MB","GB","TB"};
    double  sz = bytes;
    int     u  = 0;
    while (sz >= 1024.0 && u < 4) { sz /= 1024.0; ++u; }
    return QString::number(sz, 'f', (u==0 ? 0 : (sz < 10 ? 2 : 1))) + ' ' + units[u];
}
static QString makeMediaGridHtml(QTextDocument* doc,
                          QList<Attachement>& media,
                          const QSize& tile,
                          qreal dpr)
{
    QString html = R"(<table class="mediaGrid"><tr>)";
    int col = 0;

    for (const auto& a : media) {
        // --- preview source (unchanged) ---
        QString imgSrc;
        if (a.isImage && isGifPath(a.path)) {
            imgSrc = QUrl::fromLocalFile(a.path).toString();        // lets GIFs animate
        } else {
            QPixmap pm;
            if (a.isVideo) {
                pm = mediaUtils::tryVideoThumb(a.path, tile, dpr);
                if (pm.isNull())
                    pm = mediaUtils::makeSystemIcon(a.path, tile, dpr);
                else
                    pm = mediaUtils::addCenteredPlayBadge(pm);     
            } else {
                pm = mediaUtils::makeImageThumbCrisp(a.path, tile, dpr);
                if (pm.isNull())
                    pm = mediaUtils::makeSystemIcon(a.path, tile, dpr);
            }
            imgSrc = mediaUtils::registerPixmap(doc, pm, a.path + "|tile");
        }

        const bool isGif = a.isImage && mediaUtils::isGifPath(a.path);
        // --- build att:// URL that carries the *path* ---
        QUrl u(QStringLiteral("att://open"));
        QUrlQuery q;
        q.addQueryItem(QStringLiteral("path"), a.path);             // real OS path
        u.setQuery(q);

        html += QString(R"(
<td class="tile">
  <a class="media %1" href="%2">
    <img src="%3" width="%4" height="%5">
  </a>
</td>)")
                    .arg(a.isVideo ? "is-video" : (isGif ? "is-gif" : ""))
                    .arg(u.toString())
                    .arg(imgSrc)
                    .arg(tile.width())
                    .arg(tile.height());

        if (++col == 2) { html += "</tr><tr>"; col = 0; }
    }

    html += "</tr></table>";
    return html;
}
static QString makeFileListHtml(QTextDocument* doc, QList<Attachement>& files, qreal dpr)
{
    const QSize iconSz = {32,32};
    QString html = R"(<table class="fileList">)";
    for (const auto& a : files) {
        QPixmap pm = mediaUtils::makeSystemIcon(a.path, iconSz, dpr);
        const QString iconSrc = mediaUtils::registerPixmap(doc, pm, a.path + "|icon32");

        const QString pathFs = QDir::fromNativeSeparators(a.path);
        const QString href   = "att://open/" + pathFs;

        html += QString(R"(
<tr><td>
  <table class="fileChip" width="100%%" cellpadding="0" cellspacing="0">
    <tr>
      <td style="width:%1px;"><img class="fileIcon" src="%2" width="%1" height="%1"></td>
      <td class="fileMeta">
        <a href="%3" class="fileName">%4</a>
        <div class="fileSize">%5</div>
      </td>
    </tr>
  </table>
</td></tr>)")   .arg(iconSz.width())                 // %1 used three times
                    .arg(iconSrc)                        // %2
                    .arg(href)                           // %3
                    .arg(Utils::toHtml(a.name))          // %4
                    .arg(Utils::toHtml(a.size));
    }
    html += "</table>";
    return html;
}

static QString attachementsHtmlFor(const QVector<Attachement>& attachs, QTextDocument *doc, qreal dpr)
{
    QList<Attachement> media, files;
    for (auto a : attachs) {


        a.isImage = mediaUtils::isImagePath(a.path);
        a.isVideo = !a.isImage && mediaUtils::isVideoPath(a.path);

        (a.isImage || a.isVideo ? media : files).push_back(a);
    }


    QString html;
    if (!media.isEmpty())
        html += makeMediaGridHtml(doc, media, /*tile*/{320,250}, dpr);
    if (!files.isEmpty())
        html += makeFileListHtml(doc, files, dpr);

    return html;
}

static QVector<Attachement> chipToData(QVector<AttachementChip*> chips){
    QVector<Attachement> attachs;
    for (auto *c : chips){
        Attachement attach = {c->info().fileName(), c->info().absoluteFilePath(), c->info().suffix()};
        attachs.append(attach);
    }
    return attachs;
}
}










