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

class Ui_amigos
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *friendsScrollArea;
    QWidget *friendsScrollAreaContent;
    QVBoxLayout *verticalLayout_6;
    QVBoxLayout *onlineFriends;
    QVBoxLayout *offlineFriends;

    void setupUi(QWidget *amigos)
    {
        if (amigos->objectName().isEmpty())
            amigos->setObjectName("amigos");
        amigos->resize(355, 487);
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(amigos->sizePolicy().hasHeightForWidth());
        amigos->setSizePolicy(sizePolicy);
        amigos->setMinimumSize(QSize(355, 0));
        amigos->setAutoFillBackground(false);
        amigos->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(amigos);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        friendsScrollArea = new QScrollArea(amigos);
        friendsScrollArea->setObjectName("friendsScrollArea");
        friendsScrollArea->setAutoFillBackground(false);
        friendsScrollArea->setStyleSheet(QString::fromUtf8(""));
        friendsScrollArea->setLineWidth(0);
        friendsScrollArea->setWidgetResizable(true);
        friendsScrollAreaContent = new QWidget();
        friendsScrollAreaContent->setObjectName("friendsScrollAreaContent");
        friendsScrollAreaContent->setGeometry(QRect(0, 0, 353, 485));
        friendsScrollAreaContent->setAutoFillBackground(false);
        friendsScrollAreaContent->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_6 = new QVBoxLayout(friendsScrollAreaContent);
        verticalLayout_6->setSpacing(10);
        verticalLayout_6->setObjectName("verticalLayout_6");
        verticalLayout_6->setContentsMargins(0, 0, 0, 0);
        onlineFriends = new QVBoxLayout();
        onlineFriends->setSpacing(0);
        onlineFriends->setObjectName("onlineFriends");

        verticalLayout_6->addLayout(onlineFriends);

        offlineFriends = new QVBoxLayout();
        offlineFriends->setSpacing(0);
        offlineFriends->setObjectName("offlineFriends");
        offlineFriends->setContentsMargins(0, -1, -1, -1);

        verticalLayout_6->addLayout(offlineFriends);

        verticalLayout_6->setStretch(1, 1);
        friendsScrollArea->setWidget(friendsScrollAreaContent);

        verticalLayout->addWidget(friendsScrollArea);


        retranslateUi(amigos);

        QMetaObject::connectSlotsByName(amigos);
    } // setupUi

    void retranslateUi(QWidget *amigos)
    {
        amigos->setWindowTitle(QCoreApplication::translate("amigos", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class amigos: public Ui_amigos {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRIENDS_H
