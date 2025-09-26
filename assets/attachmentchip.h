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
class AttachmentChip : public QWidget {
    Q_OBJECT
public:
    explicit AttachmentChip(const QString& filePath, QWidget* parent=nullptr)
        : QWidget(parent), m_path(filePath), m_info(filePath)
    {
        setObjectName("AttachmentChip");
        setStyleSheet(
            "#AttachmentChip { background:#2b2f33; border-radius:8px; } "
            "#AttachmentChip QLabel { color:#d6d7d8; font:600 12px 'Segoe UI'; } "
            "#AttachmentChip .meta { color:#a0a4a8; font:500 11px 'Segoe UI'; }"
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

signals:
    void removeMe(AttachmentChip* self);

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
        QMimeDatabase db;
        const auto mt = db.mimeTypeForFile(fi);
        const QString name = mt.name(); // e.g. "image/png", "video/mp4"

        // 1) Images (incl. GIF – we’ll use first frame)
        if (name.startsWith("image/")) {
            QImageReader r(fi.absoluteFilePath());
            r.setAutoTransform(true);
            r.setScaledSize(QSize(96,96));                 // cheap downscale
            QImage img = r.read();
            if (!img.isNull())
                return QPixmap::fromImage(img);
        }

        // 2) Videos: use OS icon + draw a play overlay
        if (name.startsWith("video/")) {
            QFileIconProvider ip;
            QPixmap pm = ip.icon(fi).pixmap(96,96);
            QPainter p(&pm);
            p.setRenderHint(QPainter::Antialiasing, true);
            // dark circle
            QPoint c(pm.width()-24, pm.height()-24);
            p.setBrush(QColor(0,0,0,160));
            p.setPen(Qt::NoPen);
            p.drawEllipse(QRect(c-QPoint(12,12), QSize(24,24)));
            // white play triangle
            p.setBrush(Qt::white);
            QPolygon tri; tri << (c+QPoint(-4,-6)) << (c+QPoint(7,0)) << (c+QPoint(-4,6));
            p.drawPolygon(tri);
            return pm;
        }

        // 3) Other: system icon
        QFileIconProvider ip;
        return ip.icon(fi).pixmap(96,96);
    }
};
