#include <QStandardPaths>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QSvgRenderer>
#include <qpainterpath.h>
#include "helpers.h"
#include "requestrow.h"

QString path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Chat";
RequestRow::RequestRow(const RequestData &d, QWidget *parent)
    : QWidget(parent), m_data(d)
{
    const int S = 32;
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(64);
    setAttribute(Qt::WA_Hover, true);
    setAttribute(Qt::WA_StyledBackground, true);
    /* ---------- avatar (left) ----------------------------------- */
    auto *ava = new QLabel;
    ava->setFixedSize(52, 52);          // the avatar slot

    /* 1 ─ Try to load the real picture -------------------------------- */
    QPixmap raw = d.avatar;       // your existing load logic  // fallback

    ava->setPixmap(makeRoundedAvatar(raw, 52));
    ava->setStyleSheet("background: transparent;");   // <- no bars
    ava->setAlignment(Qt::AlignCenter);

    /* ---------- two-line name block ----------------------------- */
    auto *name  = new QLabel(d.displayName);
    name->setStyleSheet("color:#fff;  font-weight:600;");
    name->setStyleSheet("background: transparent;");
    /* ---------- buttons (right) --------------------------------- */
    auto buttonStyle = QStringLiteral(R"(
        QPushButton {
            color:#fff;
        }
        QPushButton#accept:hover { background:#2f8f54; }
        QPushButton#decline:hover { background:#f04747; }
    )");
    QPixmap checkSvg = renderSvg(path + "/resources/icons/check.svg", S);
    QPixmap closeSvg = renderSvg(path + "/resources/icons/close.svg", S);
    QPixmap checkContainer = makeRoundedAvatar(checkSvg, S);
    QPixmap closeContainer = makeRoundedAvatar(closeSvg, S);
    auto *accept = new QPushButton();
    auto *decline = new QPushButton();
    accept->setFixedSize(S, S);
    accept->setIcon(checkContainer);
    accept->setIconSize(QSize(22, 22));
    accept->setFlat(true);
    decline->setIcon(closeContainer);
    decline->setIconSize(QSize(19, 19));
    decline->setFlat(true);
    accept ->setObjectName("accept");
    decline->setObjectName("decline");
    accept ->setStyleSheet(buttonStyle);
    decline->setStyleSheet(buttonStyle);

    connect(accept,  &QPushButton::clicked,
            this, [this]{ emit accepted(m_data);  });
    connect(decline, &QPushButton::clicked,
            this, [this]{ emit declined(m_data); });

    /* ---------- row layout -------------------------------------- */
    auto *h = new QHBoxLayout(this);
    h->setContentsMargins(16,0,16,0);
    h->setSpacing(16);
    h->addWidget(ava);
    h->addWidget(name, 1);    // stretch = 1 (takes remaining width)
    h->addWidget(accept);
    h->addWidget(decline);
}

/* Paint gray hover backdrop + 1-px separator line */
void RequestRow::paintEvent(QPaintEvent *e)
{
    QPainter p(this);

    if (underMouse()) {                               // rounded hover plate
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(QColor("#3a3c43"));         // Discord hover colour
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 8, 8);
    }        // hover colour

    /* thin separator across bottom – skip last pixel at right to mimic Discord */
    p.setPen(QColor("#3a3c43"));
    p.drawLine(6, height()-1, width()-6, height()-1);

    QWidget::paintEvent(e);
}
