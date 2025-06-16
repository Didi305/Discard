#include <QStandardPaths>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QSvgRenderer>
#include <qpainterpath.h>
#include <qstyleoption.h>
#include "helpers.h"
#include "row.h"
const int S = 32;
QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Chat";
Row::Row(const Data &d, RowType type, QWidget *parent)
    : QFrame(parent), m_data(d), type()
{
    auto *h = new QHBoxLayout(this);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(64);
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_Hover);

    /* ---------- avatar (left) ----------------------------------- */
    auto *ava = new QLabel;
    ava->setFixedSize(52, 52);          // the avatar slot

    /* 1 ─ Try to load the real picture -------------------------------- */
    QPixmap raw = d.avatar;       // your existing load logic  // fallback

    ava->setPixmap(makeRoundedAvatar(raw, 52));
    ava->setObjectName("avatar");
    ava->setAlignment(Qt::AlignCenter);

    /* ---------- two-line name block ----------------------------- */
    auto *name  = new QLabel(d.displayName);
    name->setObjectName("name");
    QPixmap checkSvg = renderSvg(path + "/resources/icons/check.svg", S);
    QPixmap closeSvg = renderSvg(path + "/resources/icons/close.svg", S);
    QPixmap checkContainer = makeRoundedAvatar(checkSvg, S);
    QPixmap closeContainer = makeRoundedAvatar(closeSvg, S);
    h->setContentsMargins(16,0,16,0);
    h->setSpacing(16);
    h->addWidget(ava);
    h->addWidget(name, 1);
    if(type == RowType::Pending){
        auto *accept = new QPushButton();
        auto *decline = new QPushButton();
        accept->setIcon(checkContainer);
        accept->setIconSize(QSize(19, 19));
        decline->setIcon(closeContainer);
        decline->setIconSize(QSize(19, 19));
        accept ->setObjectName("accept");
        accept->setCursor(Qt::PointingHandCursor);
        decline->setCursor(Qt::PointingHandCursor);
        decline->setObjectName("decline");
        h->addWidget(accept);
        h->addWidget(decline);
        connect(accept,  &QPushButton::clicked,
                this, [this]{ emit accepted(m_data);  });
        connect(decline, &QPushButton::clicked,
                this, [this]{ emit declined(m_data); });
    }
    else if(type == RowType::Outgoing){
        auto *cancel = new QPushButton();
        cancel->setIcon(closeContainer);
        cancel->setIconSize(QSize(19, 19));
        cancel->setCursor(Qt::PointingHandCursor);
        cancel->setObjectName("cancel");
        h->addWidget(cancel);
        connect(cancel, &QPushButton::clicked,
                this, [this]{ emit canceled(m_data); });
    }

    /* ---------- row layout -------------------------------------- */
    // stretch = 1 (takes remaining width)
}

/* Paint gray hover backdrop + 1-px separator line */
void Row::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    QStyleOption opt;  opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);  // ♦ fills from QSS
    if (underMouse()) {                     // hover overlay
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor("#3a3c43"));
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect().adjusted(0,0,-1,-1), 8, 8);
    }
    p.setPen(QColor("#3a3c43"));            // separator
    p.drawLine(6, height()-1, width()-6, height()-1);
    QWidget::paintEvent(e);
}
