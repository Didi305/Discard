/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayoutLeft;
    QWidget *widget_2;
    QVBoxLayout *verticalLayoutCenter;
    QStackedWidget *stackedWidget;
    QWidget *friends;
    QPushButton *addFriendButton;
    QLineEdit *addFriendInput;
    QLabel *label;
    QWidget *directMessages;
    QVBoxLayout *verticalLayoutRight;
    QWidget *widget;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1093, 739);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayoutLeft = new QVBoxLayout();
        verticalLayoutLeft->setObjectName("verticalLayoutLeft");
        widget_2 = new QWidget(centralwidget);
        widget_2->setObjectName("widget_2");
        widget_2->setMinimumSize(QSize(325, 0));
        widget_2->setStyleSheet(QString::fromUtf8("background-color : red"));

        verticalLayoutLeft->addWidget(widget_2);


        horizontalLayout->addLayout(verticalLayoutLeft);

        verticalLayoutCenter = new QVBoxLayout();
        verticalLayoutCenter->setSpacing(2);
        verticalLayoutCenter->setObjectName("verticalLayoutCenter");
        stackedWidget = new QStackedWidget(centralwidget);
        stackedWidget->setObjectName("stackedWidget");
        stackedWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(53, 53, 53)"));
        friends = new QWidget();
        friends->setObjectName("friends");
        addFriendButton = new QPushButton(friends);
        addFriendButton->setObjectName("addFriendButton");
        addFriendButton->setGeometry(QRect(340, 60, 80, 24));
        addFriendInput = new QLineEdit(friends);
        addFriendInput->setObjectName("addFriendInput");
        addFriendInput->setGeometry(QRect(10, 60, 321, 24));
        label = new QLabel(friends);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 20, 141, 31));
        QFont font;
        font.setPointSize(18);
        label->setFont(font);
        stackedWidget->addWidget(friends);
        directMessages = new QWidget();
        directMessages->setObjectName("directMessages");
        stackedWidget->addWidget(directMessages);

        verticalLayoutCenter->addWidget(stackedWidget);


        horizontalLayout->addLayout(verticalLayoutCenter);

        verticalLayoutRight = new QVBoxLayout();
        verticalLayoutRight->setObjectName("verticalLayoutRight");
        widget = new QWidget(centralwidget);
        widget->setObjectName("widget");
        widget->setMinimumSize(QSize(325, 0));
        widget->setStyleSheet(QString::fromUtf8("background-color : yellow"));

        verticalLayoutRight->addWidget(widget);


        horizontalLayout->addLayout(verticalLayoutRight);

        horizontalLayout->setStretch(1, 1);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1093, 21));
        MainWindow->setMenuBar(menubar);

        retranslateUi(MainWindow);

        stackedWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        addFriendButton->setText(QCoreApplication::translate("MainWindow", "Add Friend", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Add a friend", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
