#include "messagebubble.h"
#include "utils.h"
#include <qtextedit.h>
#include <QTextBrowser>
#include "messagerow.h"

MessageRow::MessageRow(QString body, Data d, bool withAvatar, QString time, QWidget *parent)
    : Row(d, parent), nameDateLayout(new QHBoxLayout)
{
    QLabel* timeLabel = new QLabel(time);
    timeLabel->setObjectName("timeLabel");
    ava->setAlignment(Qt::AlignTop);
    v->setSpacing(0);
    h->removeWidget(name);
    if(withAvatar){
        h->setContentsMargins(8, 24, 0, 0);
        name->setContentsMargins(3, 0, 0, 0);
        nameDateLayout->setSpacing(10);
        nameDateLayout->addWidget(name, 0);
        nameDateLayout->addWidget(timeLabel, 1);
        v->addLayout(nameDateLayout);
    }
    else{
        name->deleteLater();
        h->setContentsMargins(8, 0, 0, 0);
        ava->setPixmap(QPixmap());
        qDebug() << "message sent at: " << time;
        ava->setMinimumWidth(48);
        ava->setContentsMargins(12, 8, 0, 0);
        ava->setAlignment(Qt::AlignTop);
    }
    auto *msg = new MessageBubble(this);
    msg->setObjectName("message");
    msg->setText(body);
    v->addWidget(msg);
}


