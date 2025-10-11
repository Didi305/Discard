#include <QStandardPaths>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPainter>
#include <QSvgRenderer>
#include <qpainterpath.h>
#include <qstyleoption.h>
#include "utils.h"
#include "row.h"
#include "manager.h"



Row::Row(const Data &d, QWidget *parent)
    : QFrame(parent), m_data(d), m_manager(Manager::get()), ava(new QLabel), name(new QLabel)
{
    h = new QHBoxLayout(this);
    v = new QVBoxLayout;
    setFixedHeight(68);
    setCursor(Qt::PointingHandCursor);
    setAttribute(Qt::WA_Hover);
    static std::unordered_map<Status, QString> statusPaths = {
        {Status::ONLINE, "resources/icons/ONLINE.svg"},
        {Status::HIDDEN, "resources/icons/HIDDEN.svg"},
        {Status::DO_NOT_DISTURB, "resources/icons/DO_NOT_DISTURB.svg"}
    };
    static std::unordered_map<Status,std::unique_ptr<QSvgRenderer>> statusIcons = []{
        std::unordered_map<Status,std::unique_ptr<QSvgRenderer>> m;
        m[Status::ONLINE] = std::make_unique<QSvgRenderer>(path + "/resources/icons/active.svg");
        m[Status::HIDDEN] = std::make_unique<QSvgRenderer>(path + "/resources/icons/hidden.svg");
        m[Status::DO_NOT_DISTURB] = std::make_unique<QSvgRenderer>(path + "/resources/icons/disturb.svg");
        return m;
    }();

    auto amigos = m_manager->getClient()->getFriends();
    ava->setStyleSheet("background: transparent");
    bool online = amigos[d];
    QPixmap raw = d.avatar;
    QPixmap finalAvatar;
    if(online){
        finalAvatar = Utils::makeAvatarWithHole(raw, 48, statusIcons[d.status].get());
    }
    else finalAvatar = Utils::makeAvatarWithHole(raw, 48, statusIcons[Status::HIDDEN].get());
    ava->setPixmap(finalAvatar);

    ava->setObjectName("avatar");

    name->setText(d.displayName);
    QFont f = name->font();
    f.setPointSize(12);
    f.setWeight(QFont::Bold);    // or QFont::Bold
    name->setFont(f);
    name->setStyleSheet("background: transparent");
    h->addWidget(ava, 0);
    v->addWidget(name, 0);
    h->addLayout(v, 1);
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
    if (drawLine) p.drawLine(6, height()-1, width()-6, height()-1);
    QWidget::paintEvent(e);
}
