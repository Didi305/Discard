#include "messagebubble.h"
#include "utils.h"
#include <qtextedit.h>
#include <QTextBrowser>
#include "messagerow.h"
#include <QPlainTextEdit>
#include <algorithm>

MessageRow::MessageRow(QString body, Data d, bool withAvatar, QString time, QWidget *parent)
    : Row(d, parent), nameDateLayout(new QHBoxLayout)
{
    h->setSizeConstraint(QLayout::SetDefaultConstraint);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setStyleSheet("background: black");
    name->setStyleSheet("background: green");
    ava->setStyleSheet("background: red");
    ava->setAlignment(Qt::AlignTop);
    ava->setFixedSize(48, 48);
    name->setFixedHeight(20);
    if (withAvatar) {
        name->setText(d.displayName);
        QLabel *timeLabel = new QLabel(time, this);
        // if you want name+time on one line, create a small h‐layout here…
        auto *nd = new QHBoxLayout;
        nd->setContentsMargins(0,0,0,0);
        nd->setSpacing(6);
        nd->addWidget(name,      0);
        nd->addWidget(timeLabel, 0);
        v->addLayout(nd,          0); // no stretch
    } else {
        delete name;      // no name row
        name = nullptr;
        ava->setText("zamel");
    }

    // 1) create the bubble
    auto *msg = new WrapLabel(this);
    msg->setStyleSheet("background: purple");
    msg->setText(body);
    v->addWidget(msg);


}


QSize MessageRow::sizeHint() const
{/*
    QSize s = QFrame::sizeHint();   // or QWidget::sizeHint()
    qDebug() << ">>> MessageRow::sizeHint() =" << s
             << " min=" << minimumSizeHint()
             << " policy.v=" << sizePolicy().verticalPolicy();
    return s;*/
}


