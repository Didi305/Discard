/********************************************************************************
** Form generated from reading UI file 'friends.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRIENDS_H
#define UI_FRIENDS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Friends
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_6;
    QVBoxLayout *onlineFriends;
    QVBoxLayout *offlineFriends;

    void setupUi(QWidget *Friends)
    {
        if (Friends->objectName().isEmpty())
            Friends->setObjectName("Friends");
        Friends->resize(397, 750);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Friends->sizePolicy().hasHeightForWidth());
        Friends->setSizePolicy(sizePolicy);
        Friends->setMinimumSize(QSize(355, 0));
        verticalLayout = new QVBoxLayout(Friends);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(Friends);
        scrollArea->setObjectName("scrollArea");
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 395, 748));
        verticalLayout_6 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_6->setSpacing(10);
        verticalLayout_6->setObjectName("verticalLayout_6");
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        onlineFriends = new QVBoxLayout();
        onlineFriends->setSpacing(15);
        onlineFriends->setObjectName("onlineFriends");

        verticalLayout_6->addLayout(onlineFriends);

        offlineFriends = new QVBoxLayout();
        offlineFriends->setSpacing(0);
        offlineFriends->setObjectName("offlineFriends");
        offlineFriends->setContentsMargins(0, -1, -1, -1);

        verticalLayout_6->addLayout(offlineFriends);

        verticalLayout_6->setStretch(1, 1);
        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollArea);


        retranslateUi(Friends);

        QMetaObject::connectSlotsByName(Friends);
    } // setupUi

    void retranslateUi(QWidget *Friends)
    {
        Friends->setWindowTitle(QCoreApplication::translate("Friends", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Friends: public Ui_Friends {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRIENDS_H
