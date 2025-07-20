#include "outgoingrow.h"

OutgoingRow::OutgoingRow(Data d, QWidget* parent) : Row (d, parent)
{
    QPixmap closeSvg = Utils::renderSvg(path + "/resources/icons/close.svg", S);
    QPixmap closeContainer = Utils::makeRoundedAvatar(closeSvg, S);
    auto *cancel = new QPushButton();
    cancel->setIcon(closeContainer);
    cancel->setIconSize(QSize(19, 19));
    cancel->setCursor(Qt::PointingHandCursor);
    cancel->setObjectName("cancel");
    h->addWidget(cancel);
    connect(cancel, &QPushButton::clicked,
            this, [this]{ emit canceled(m_data); });
}
