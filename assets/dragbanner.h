#pragma once
#include <QWidget>
#include <utils.h>

class DragBanner : public QWidget {
    Q_OBJECT
public:
    explicit DragBanner(QWidget *parent=nullptr)
        : QWidget(parent, Qt::FramelessWindowHint)
    {
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
        setAttribute(Qt::WA_TranslucentBackground, true);
        hide();
    }

    void showBanner(const QString& title, const QString& subtitle, int count=1) {
        m_title = title;
        m_sub   = subtitle;
        m_count = count;
        if (parentWidget()) setGeometry(parentWidget()->rect());
        show(); raise(); update();
    }
    void hideBanner() { hide(); }

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        // 1) dim the background
        p.fillRect(rect(), QColor(0,0,0,120));

        // 2) centered card
        const int W = 420, H = 180;
        QRect card((width()-W)/2, (height()-H)/2, W, H);

        // card fill (Discord-ish blurple tint)
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(88,101,242,230));         // #5865F2 with alpha
        p.drawRoundedRect(card, 12, 12);

        // dashed border
        QPen dashed(QColor(255,255,255,210), 2, Qt::DashLine);
        dashed.setDashPattern({6,4});
        p.setBrush(Qt::NoBrush);
        p.setPen(dashed);
        p.drawRoundedRect(card.adjusted(4,4,-4,-4), 10, 10);

        // tiny “files” icon stack
        QRect icon(card.left()+18, card.top()+28, 64, 64);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255,255,255,230));
        auto doc = [&](QPoint o){
            QRect r(icon.translated(o).adjusted(0,0,-8,-8));
            p.drawRoundedRect(r, 6, 6);
            // dog-ear
            QPainterPath dog; dog.moveTo(r.right()-14, r.top());
            dog.lineTo(r.right(), r.top()+14); dog.lineTo(r.right(), r.top()); dog.closeSubpath();
            p.fillPath(dog, QColor(200,200,255));
        };
        doc(QPoint(10,10)); doc(QPoint(5,5)); doc(QPoint(0,0));

        // 3) text
        p.setPen(Qt::white);
        QFont f = font(); f.setPointSize(14); p.setFont(f);
        QString title = m_title;
        if (m_count > 1) title += QString(" (%1)").arg(m_count);
        p.drawText(QRect(card.left()+100, card.top()+36, card.width()-120, 28),
                   Qt::AlignLeft|Qt::AlignVCenter, title);

        f.setPointSize(11); p.setFont(f);
        p.setPen(QColor(240,240,255,230));
        p.drawText(QRect(card.left()+100, card.top()+70, card.width()-120, 24),
                   Qt::AlignLeft|Qt::AlignVCenter, m_sub);
    }

private:
    QString m_title, m_sub;
    int     m_count = 1;
};
