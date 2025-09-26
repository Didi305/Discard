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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *mainContainer;
    QVBoxLayout *verticalLayout_3;
    QFrame *topBar;
    QHBoxLayout *horizontalLayout_3;
    QFrame *LEFT_TOPBAR;
    QStackedWidget *MID_RIGHT_TOPBAR;
    QWidget *friendsTopBar;
    QHBoxLayout *horizontalLayout_2;
    QLabel *friendsLabel;
    QPushButton *addFriend;
    QPushButton *outgoing;
    QSpacerItem *horizontalSpacer;
    QWidget *directMessageTopBar;
    QWidget *columnContainer;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayoutLeft;
    QWidget *something;
    QVBoxLayout *verticalLayout;
    QWidget *options;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QWidget *widget_2;
    QFrame *userPanel;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *userAvatarButton;
    QLabel *userNameLabel;
    QPushButton *disconnectButton;
    QStackedWidget *verticalLayoutMid;
    QWidget *friends;
    QVBoxLayout *verticalLayout_4;
    QStackedWidget *ADD_OUTGOING;
    QVBoxLayout *verticalLayoutRight;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1093, 762);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        MainWindow->setIconSize(QSize(64, 64));
        mainContainer = new QWidget(MainWindow);
        mainContainer->setObjectName("mainContainer");
        mainContainer->setAutoFillBackground(false);
        mainContainer->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_3 = new QVBoxLayout(mainContainer);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        topBar = new QFrame(mainContainer);
        topBar->setObjectName("topBar");
        topBar->setMaximumSize(QSize(16777215, 40));
        topBar->setFrameShape(QFrame::Shape::StyledPanel);
        topBar->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_3 = new QHBoxLayout(topBar);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        LEFT_TOPBAR = new QFrame(topBar);
        LEFT_TOPBAR->setObjectName("LEFT_TOPBAR");
        LEFT_TOPBAR->setMinimumSize(QSize(354, 0));
        LEFT_TOPBAR->setMaximumSize(QSize(353, 16777215));
        LEFT_TOPBAR->setFrameShape(QFrame::Shape::StyledPanel);
        LEFT_TOPBAR->setFrameShadow(QFrame::Shadow::Raised);

        horizontalLayout_3->addWidget(LEFT_TOPBAR);

        MID_RIGHT_TOPBAR = new QStackedWidget(topBar);
        MID_RIGHT_TOPBAR->setObjectName("MID_RIGHT_TOPBAR");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MID_RIGHT_TOPBAR->sizePolicy().hasHeightForWidth());
        MID_RIGHT_TOPBAR->setSizePolicy(sizePolicy);
        MID_RIGHT_TOPBAR->setMinimumSize(QSize(0, 0));
        MID_RIGHT_TOPBAR->setMaximumSize(QSize(16777215, 51));
        MID_RIGHT_TOPBAR->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        MID_RIGHT_TOPBAR->setAutoFillBackground(false);
        MID_RIGHT_TOPBAR->setStyleSheet(QString::fromUtf8(""));
        MID_RIGHT_TOPBAR->setFrameShape(QFrame::Shape::NoFrame);
        MID_RIGHT_TOPBAR->setFrameShadow(QFrame::Shadow::Raised);
        MID_RIGHT_TOPBAR->setLineWidth(0);
        friendsTopBar = new QWidget();
        friendsTopBar->setObjectName("friendsTopBar");
        horizontalLayout_2 = new QHBoxLayout(friendsTopBar);
        horizontalLayout_2->setSpacing(20);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(-1, 5, 0, -1);
        friendsLabel = new QLabel(friendsTopBar);
        friendsLabel->setObjectName("friendsLabel");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(friendsLabel->sizePolicy().hasHeightForWidth());
        friendsLabel->setSizePolicy(sizePolicy1);
        friendsLabel->setMinimumSize(QSize(0, 25));
        friendsLabel->setMaximumSize(QSize(100, 20));
        QFont font;
        font.setPointSize(9);
        font.setBold(false);
        friendsLabel->setFont(font);

        horizontalLayout_2->addWidget(friendsLabel);

        addFriend = new QPushButton(friendsTopBar);
        addFriend->setObjectName("addFriend");
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(addFriend->sizePolicy().hasHeightForWidth());
        addFriend->setSizePolicy(sizePolicy2);
        addFriend->setMinimumSize(QSize(100, 30));
        addFriend->setMaximumSize(QSize(75, 16777215));
        addFriend->setFont(font);
        addFriend->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        addFriend->setAutoFillBackground(false);
        addFriend->setStyleSheet(QString::fromUtf8(""));
        addFriend->setFlat(true);

        horizontalLayout_2->addWidget(addFriend);

        outgoing = new QPushButton(friendsTopBar);
        outgoing->setObjectName("outgoing");
        sizePolicy2.setHeightForWidth(outgoing->sizePolicy().hasHeightForWidth());
        outgoing->setSizePolicy(sizePolicy2);
        outgoing->setMinimumSize(QSize(0, 30));
        outgoing->setMaximumSize(QSize(75, 16777215));
        QFont font1;
        font1.setPointSize(9);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setUnderline(false);
        font1.setStrikeOut(false);
        font1.setKerning(true);
        outgoing->setFont(font1);
        outgoing->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        outgoing->setStyleSheet(QString::fromUtf8(""));
        outgoing->setFlat(true);

        horizontalLayout_2->addWidget(outgoing);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        MID_RIGHT_TOPBAR->addWidget(friendsTopBar);
        directMessageTopBar = new QWidget();
        directMessageTopBar->setObjectName("directMessageTopBar");
        MID_RIGHT_TOPBAR->addWidget(directMessageTopBar);

        horizontalLayout_3->addWidget(MID_RIGHT_TOPBAR);


        verticalLayout_3->addWidget(topBar);

        columnContainer = new QWidget(mainContainer);
        columnContainer->setObjectName("columnContainer");
        columnContainer->setAutoFillBackground(false);
        horizontalLayout_5 = new QHBoxLayout(columnContainer);
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayoutLeft = new QVBoxLayout();
        verticalLayoutLeft->setSpacing(0);
        verticalLayoutLeft->setObjectName("verticalLayoutLeft");
        something = new QWidget(columnContainer);
        something->setObjectName("something");
        something->setMinimumSize(QSize(355, 0));
        QPalette palette;
        QBrush brush(QColor(255, 255, 255, 255));
        brush.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::WindowText, brush);
        QBrush brush1(QColor(43, 45, 49, 255));
        brush1.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush1);
        QBrush brush2(QColor(45, 45, 45, 255));
        brush2.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Light, brush2);
        QBrush brush3(QColor(37, 37, 37, 255));
        brush3.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Midlight, brush3);
        QBrush brush4(QColor(15, 15, 15, 255));
        brush4.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Dark, brush4);
        QBrush brush5(QColor(20, 20, 20, 255));
        brush5.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::BrightText, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush1);
        QBrush brush6(QColor(0, 0, 0, 255));
        brush6.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Shadow, brush6);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::AlternateBase, brush4);
        QBrush brush7(QColor(255, 255, 220, 255));
        brush7.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipBase, brush7);
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush8(QColor(255, 255, 255, 127));
        brush8.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Accent, brush6);
#endif
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::WindowText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Dark, brush4);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::BrightText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ButtonText, brush);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Shadow, brush6);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::AlternateBase, brush4);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipBase, brush7);
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::ToolTipText, brush6);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::PlaceholderText, brush8);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Accent, brush6);
#endif
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::WindowText, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Light, brush2);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Midlight, brush3);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Dark, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Mid, brush5);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Text, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::BrightText, brush);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ButtonText, brush4);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush1);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Shadow, brush6);
        QBrush brush9(QColor(30, 30, 30, 255));
        brush9.setStyle(Qt::BrushStyle::SolidPattern);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::AlternateBase, brush9);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipBase, brush7);
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::ToolTipText, brush6);
        QBrush brush10(QColor(15, 15, 15, 127));
        brush10.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::PlaceholderText, brush10);
#endif
        QBrush brush11(QColor(21, 21, 21, 255));
        brush11.setStyle(Qt::BrushStyle::SolidPattern);
#if QT_VERSION >= QT_VERSION_CHECK(6, 6, 0)
        palette.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Accent, brush11);
#endif
        something->setPalette(palette);
        something->setAutoFillBackground(true);
        something->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(something);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(-1, -1, -1, 17);
        options = new QWidget(something);
        options->setObjectName("options");
        sizePolicy2.setHeightForWidth(options->sizePolicy().hasHeightForWidth());
        options->setSizePolicy(sizePolicy2);
        options->setMinimumSize(QSize(200, 200));
        options->setMaximumSize(QSize(355, 200));
        options->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_2 = new QVBoxLayout(options);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(5, 0, 5, -1);
        pushButton = new QPushButton(options);
        pushButton->setObjectName("pushButton");
        pushButton->setMinimumSize(QSize(330, 45));
        pushButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton->setAutoFillBackground(false);
        pushButton->setStyleSheet(QString::fromUtf8(""));
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::UserAvailable));
        pushButton->setIcon(icon);
        pushButton->setFlat(true);

        verticalLayout_2->addWidget(pushButton);

        pushButton_2 = new QPushButton(options);
        pushButton_2->setObjectName("pushButton_2");
        pushButton_2->setMinimumSize(QSize(330, 45));
        pushButton_2->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton_2->setAutoFillBackground(false);
        pushButton_2->setStyleSheet(QString::fromUtf8(""));
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend));
        pushButton_2->setIcon(icon1);
        pushButton_2->setFlat(true);

        verticalLayout_2->addWidget(pushButton_2);

        pushButton_3 = new QPushButton(options);
        pushButton_3->setObjectName("pushButton_3");
        pushButton_3->setMinimumSize(QSize(330, 45));
        pushButton_3->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton_3->setAutoFillBackground(false);
        pushButton_3->setStyleSheet(QString::fromUtf8(""));
        QIcon icon2(QIcon::fromTheme(QIcon::ThemeIcon::AudioCard));
        pushButton_3->setIcon(icon2);
        pushButton_3->setFlat(true);

        verticalLayout_2->addWidget(pushButton_3);

        pushButton_4 = new QPushButton(options);
        pushButton_4->setObjectName("pushButton_4");
        pushButton_4->setMinimumSize(QSize(330, 45));
        pushButton_4->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton_4->setAutoFillBackground(false);
        pushButton_4->setStyleSheet(QString::fromUtf8(""));
        pushButton_4->setIcon(icon);
        pushButton_4->setFlat(true);

        verticalLayout_2->addWidget(pushButton_4);


        verticalLayout->addWidget(options);

        widget_2 = new QWidget(something);
        widget_2->setObjectName("widget_2");
        widget_2->setStyleSheet(QString::fromUtf8("background: 030303ff;;"));

        verticalLayout->addWidget(widget_2);

        userPanel = new QFrame(something);
        userPanel->setObjectName("userPanel");
        sizePolicy2.setHeightForWidth(userPanel->sizePolicy().hasHeightForWidth());
        userPanel->setSizePolicy(sizePolicy2);
        userPanel->setMinimumSize(QSize(335, 60));
        userPanel->setMaximumSize(QSize(335, 60));
        userPanel->setAutoFillBackground(false);
        userPanel->setStyleSheet(QString::fromUtf8(""));
        userPanel->setFrameShape(QFrame::Shape::StyledPanel);
        userPanel->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout_4 = new QHBoxLayout(userPanel);
        horizontalLayout_4->setSpacing(12);
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        horizontalLayout_4->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        horizontalLayout_4->setContentsMargins(5, 0, 12, 0);
        userAvatarButton = new QPushButton(userPanel);
        userAvatarButton->setObjectName("userAvatarButton");
        userAvatarButton->setEnabled(true);
        sizePolicy2.setHeightForWidth(userAvatarButton->sizePolicy().hasHeightForWidth());
        userAvatarButton->setSizePolicy(sizePolicy2);
        userAvatarButton->setMinimumSize(QSize(48, 48));
        userAvatarButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        userAvatarButton->setIconSize(QSize(48, 48));
        userAvatarButton->setFlat(true);

        horizontalLayout_4->addWidget(userAvatarButton);

        userNameLabel = new QLabel(userPanel);
        userNameLabel->setObjectName("userNameLabel");
        userNameLabel->setStyleSheet(QString::fromUtf8("setBold: true;"));

        horizontalLayout_4->addWidget(userNameLabel);

        disconnectButton = new QPushButton(userPanel);
        disconnectButton->setObjectName("disconnectButton");
        disconnectButton->setMinimumSize(QSize(44, 44));
        disconnectButton->setMaximumSize(QSize(44, 44));
        disconnectButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        disconnectButton->setAutoFillBackground(false);
        disconnectButton->setStyleSheet(QString::fromUtf8(""));
        disconnectButton->setIconSize(QSize(26, 26));
        disconnectButton->setFlat(true);

        horizontalLayout_4->addWidget(disconnectButton);

        horizontalLayout_4->setStretch(0, 1);
        horizontalLayout_4->setStretch(1, 2);
        horizontalLayout_4->setStretch(2, 1);

        verticalLayout->addWidget(userPanel);


        verticalLayoutLeft->addWidget(something);


        horizontalLayout_5->addLayout(verticalLayoutLeft);

        verticalLayoutMid = new QStackedWidget(columnContainer);
        verticalLayoutMid->setObjectName("verticalLayoutMid");
        sizePolicy.setHeightForWidth(verticalLayoutMid->sizePolicy().hasHeightForWidth());
        verticalLayoutMid->setSizePolicy(sizePolicy);
        verticalLayoutMid->setAutoFillBackground(false);
        verticalLayoutMid->setStyleSheet(QString::fromUtf8(""));
        friends = new QWidget();
        friends->setObjectName("friends");
        friends->setAutoFillBackground(false);
        friends->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_4 = new QVBoxLayout(friends);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        ADD_OUTGOING = new QStackedWidget(friends);
        ADD_OUTGOING->setObjectName("ADD_OUTGOING");
        QPalette palette1;
        QBrush brush12(QColor(255, 192, 203, 255));
        brush12.setStyle(Qt::BrushStyle::SolidPattern);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Button, brush12);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Base, brush12);
        palette1.setBrush(QPalette::ColorGroup::Active, QPalette::ColorRole::Window, brush12);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Button, brush12);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Base, brush12);
        palette1.setBrush(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Window, brush12);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Button, brush12);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Base, brush12);
        palette1.setBrush(QPalette::ColorGroup::Disabled, QPalette::ColorRole::Window, brush12);
        ADD_OUTGOING->setPalette(palette1);
        ADD_OUTGOING->setAutoFillBackground(false);
        ADD_OUTGOING->setStyleSheet(QString::fromUtf8(""));
        ADD_OUTGOING->setLineWidth(0);
        ADD_OUTGOING->setMidLineWidth(0);

        verticalLayout_4->addWidget(ADD_OUTGOING);

        verticalLayoutMid->addWidget(friends);

        horizontalLayout_5->addWidget(verticalLayoutMid);

        verticalLayoutRight = new QVBoxLayout();
        verticalLayoutRight->setSpacing(0);
        verticalLayoutRight->setObjectName("verticalLayoutRight");

        horizontalLayout_5->addLayout(verticalLayoutRight);


        verticalLayout_3->addWidget(columnContainer);

        MainWindow->setCentralWidget(mainContainer);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1093, 21));
        MainWindow->setMenuBar(menubar);

        retranslateUi(MainWindow);

        verticalLayoutMid->setCurrentIndex(0);
        ADD_OUTGOING->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Discard", nullptr));
        friendsLabel->setText(QCoreApplication::translate("MainWindow", "Friends", nullptr));
        addFriend->setText(QCoreApplication::translate("MainWindow", "Add Friend", nullptr));
        outgoing->setText(QCoreApplication::translate("MainWindow", "Pending", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "   Friends", nullptr));
        pushButton_2->setText(QCoreApplication::translate("MainWindow", "   Message Requests", nullptr));
        pushButton_3->setText(QCoreApplication::translate("MainWindow", "   Helala Music", nullptr));
        pushButton_4->setText(QCoreApplication::translate("MainWindow", "   Idk", nullptr));
        userAvatarButton->setText(QString());
        userNameLabel->setText(QString());
        disconnectButton->setText(QString());
#if QT_CONFIG(shortcut)
        disconnectButton->setShortcut(QString());
#endif // QT_CONFIG(shortcut)
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
