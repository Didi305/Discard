#pragma once

#include <QWidget>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileIconProvider>
#include <QImageReader>
#include <QPainter>
#include <QMimeDatabase>

static QPixmap squareThumbFromImage(const QString& path, int side = 96)
{
    QImageReader r(path);
    r.setAutoTransform(true);

    // Read a reasonably sized image (don’t decode full-res for a small chip)
    const QSize target(side * 2, side * 2);               // slight oversample for quality
    r.setScaledSize(target);
    QImage img = r.read();
    if (img.isNull()) return QPixmap();                   // caller will fallback

    // Center-crop to square ('cover' behavior)
    const int w = img.width(), h = img.height();
    const int s = qMin(w, h);
    const int x = (w - s) / 2;
    const int y = (h - s) / 2;

    QImage cropped = img.copy(x, y, s, s).scaled(
        side, side, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    return QPixmap::fromImage(cropped);
}

static void paintCenteredPlayBadge(QPixmap& pm)
{
    if (pm.isNull()) return;
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QPoint c(pm.width() / 2, pm.height() / 2);
    const int    R = qMax(18, pm.width() / 6);            // circle radius relative to size

    // Circle
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 160));
    p.drawEllipse(QRect(c - QPoint(R, R), QSize(2*R, 2*R)));

    // Triangle (play)
    p.setBrush(Qt::white);
    const int th = R;                                     // triangle half-height
    const int tw = int(R * 1.1);                          // triangle width
    QPolygon tri;
    tri << QPoint(c.x() - tw/3, c.y() - th/2)             // slight optical shift left
        << QPoint(c.x() + tw*2/3, c.y())
        << QPoint(c.x() - tw/3, c.y() + th/2);
    p.drawPolygon(tri);
}


class AttachementChip : public QWidget {
    Q_OBJECT
public:
    explicit AttachementChip(const QString& filePath, QWidget* parent=nullptr)
        : QWidget(parent), m_path(filePath), m_info(filePath)
    {
        setObjectName("AttachementChip");
        setStyleSheet(
            "#AttachementChip { background:#2b2f33; border-radius:8px; } "
            "#AttachementChip QLabel { color:#d6d7d8; font:600 12px 'Segoe UI'; } "
            "#AttachementChip .meta { color:#a0a4a8; font:500 11px 'Segoe UI'; }"
            );
        setFixedWidth(150);
        auto lay = new QHBoxLayout(this);
        lay->setContentsMargins(8,6,8,6);
        lay->setSpacing(8);

        // icon / thumbnail
        QLabel* icon = new QLabel(this); icon->setFixedSize(40,40);
        icon->setScaledContents(true);
        icon->setPixmap(makePreview(m_info).scaled(icon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        lay->addWidget(icon);

        // text column
        auto col = new QVBoxLayout; col->setContentsMargins(0,0,0,0); col->setSpacing(0);
        auto name = new QLabel(m_info.fileName());
        name->setToolTip(m_info.absoluteFilePath());
        auto meta = new QLabel(QString("%1").arg(humanSize(m_info.size())));

        meta->setObjectName("meta");
        meta->setProperty("class", "meta");
        col->addWidget(name);
        col->addWidget(meta);
        lay->addLayout(col);

        // remove button
        auto x = new QPushButton(QStringLiteral("✕")); // or an icon
        x->setCursor(Qt::PointingHandCursor);
        x->setFixedSize(24,24);
        x->setStyleSheet("QPushButton{border:0; color:#d6d7d8; } QPushButton:hover{background:#3a3f44; border-radius:6px;}");
        connect(x, &QPushButton::clicked, this, [this]{ emit removeMe(this); });
        lay->addWidget(x);
    }

    QString path() { return m_path; }
    QFileInfo info() {return m_info; }

signals:
    void removeMe(AttachementChip* self);

private:
    QString m_path;
    QFileInfo m_info;

    static QString humanSize(qint64 b){
        const char* u[]={"B","KB","MB","GB","TB"}; int i=0; double s=b;
        while (s>=1024 && i<4){ s/=1024; ++i; }
        return QString::number(s, 'f', (i?1:0)) + " " + u[i];
    }

    static QPixmap makePreview(const QFileInfo& fi)
    {
        const QString path = fi.absoluteFilePath();

        QMimeDatabase db;
        const QString mt = db.mimeTypeForFile(fi).name();      // e.g. "image/png", "video/mp4"
        const bool isImg   = mt.startsWith(QLatin1String("image/"));
        const bool isVideo = mt.startsWith(QLatin1String("video/"));

        // 1) Images (incl. GIF first frame) -> square thumbnail
        if (isImg) {
            QPixmap pm = squareThumbFromImage(path, 96);
            if (!pm.isNull()) return pm;
            // fall through to OS icon if read failed
        }

        // 2) Videos -> OS icon with centered play overlay
        if (isVideo) {
            QFileIconProvider ip;
            QPixmap pm = ip.icon(fi).pixmap(96, 96);
            if (pm.isNull()) {
                pm = QPixmap(96,96); pm.fill(Qt::transparent);
            }
            paintCenteredPlayBadge(pm);
            return pm;
        }

        // 3) Other types -> system icon (fallback)
        QFileIconProvider ip;
        QPixmap pm = ip.icon(fi).pixmap(96, 96);
        if (pm.isNull()) { pm = QPixmap(96,96); pm.fill(Qt::transparent); }
        return pm;
    }
};

