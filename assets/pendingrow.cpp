#include "pendingrow.h"


PendingRow::PendingRow(Data d, QWidget *parent)
    : Row(d, parent)
{
    QPixmap checkSvg = Utils::renderSvg(path + "/resources/icons/check.svg", S);
    QPixmap closeSvg = Utils::renderSvg(path + "/resources/icons/close.svg", S);
    QPixmap checkContainer = Utils::makeRoundedAvatar(checkSvg, S);
    QPixmap closeContainer = Utils::makeRoundedAvatar(closeSvg, S);
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
