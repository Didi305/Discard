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
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayoutLeft;
    QWidget *widget_2;
    QStackedWidget *topBarColumnsContainer;
    QWidget *friends;
    QVBoxLayout *verticalLayout_4;
    QFrame *topBarFrame;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QPushButton *outgoing;
    QPushButton *addFriend;
    QSpacerItem *horizontalSpacer;
    QWidget *columnsContainer;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayoutCenter;
    QStackedWidget *ADD_OUTGOING;
    QVBoxLayout *verticalLayoutRight;
    QWidget *directMessages;
    QMenuBar *menubar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1093, 751);
        MainWindow->setStyleSheet(QString::fromUtf8(""));
        MainWindow->setIconSize(QSize(64, 64));
        mainContainer = new QWidget(MainWindow);
        mainContainer->setObjectName("mainContainer");
        mainContainer->setStyleSheet(QString::fromUtf8(""));
        horizontalLayout_3 = new QHBoxLayout(mainContainer);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayoutLeft = new QVBoxLayout();
        verticalLayoutLeft->setSpacing(0);
        verticalLayoutLeft->setObjectName("verticalLayoutLeft");
        widget_2 = new QWidget(mainContainer);
        widget_2->setObjectName("widget_2");
        widget_2->setMinimumSize(QSize(355, 0));
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
        widget_2->setPalette(palette);
        widget_2->setStyleSheet(QString::fromUtf8("background-color: #2b2d31;\n"
"\n"
"border: 1px solid #3a3c43;\n"
"\n"
"border-right: none;"));

        verticalLayoutLeft->addWidget(widget_2);


        horizontalLayout_3->addLayout(verticalLayoutLeft);

        topBarColumnsContainer = new QStackedWidget(mainContainer);
        topBarColumnsContainer->setObjectName("topBarColumnsContainer");
        topBarColumnsContainer->setStyleSheet(QString::fromUtf8("background-color: #32353a;"));
        friends = new QWidget();
        friends->setObjectName("friends");
        friends->setAutoFillBackground(false);
        friends->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_4 = new QVBoxLayout(friends);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setObjectName("verticalLayout_4");
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        topBarFrame = new QFrame(friends);
        topBarFrame->setObjectName("topBarFrame");
        topBarFrame->setMinimumSize(QSize(50, 50));
        topBarFrame->setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        topBarFrame->setAutoFillBackground(false);
        topBarFrame->setStyleSheet(QString::fromUtf8(""));
        topBarFrame->setFrameShape(QFrame::Shape::NoFrame);
        topBarFrame->setFrameShadow(QFrame::Shadow::Raised);
        topBarFrame->setLineWidth(0);
        horizontalLayout_2 = new QHBoxLayout(topBarFrame);
        horizontalLayout_2->setSpacing(20);
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        horizontalLayout_2->setContentsMargins(15, 5, 10, 10);
        label = new QLabel(topBarFrame);
        label->setObjectName("label");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy);
        label->setMaximumSize(QSize(100, 20));
        QFont font;
        font.setPointSize(14);
        font.setWeight(QFont::ExtraBold);
        label->setFont(font);

        horizontalLayout_2->addWidget(label);

        outgoing = new QPushButton(topBarFrame);
        outgoing->setObjectName("outgoing");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Fixed, QSizePolicy::Policy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(outgoing->sizePolicy().hasHeightForWidth());
        outgoing->setSizePolicy(sizePolicy1);
        outgoing->setMaximumSize(QSize(75, 16777215));
        QFont font1;
        font1.setPointSize(11);
        font1.setBold(true);
        font1.setItalic(false);
        font1.setUnderline(false);
        font1.setStrikeOut(false);
        font1.setKerning(true);
        outgoing->setFont(font1);
        outgoing->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        outgoing->setStyleSheet(QString::fromUtf8("#outgoing { \n"
"	background: #32353a; \n"
"}\n"
""));
        outgoing->setFlat(true);

        horizontalLayout_2->addWidget(outgoing);

        addFriend = new QPushButton(topBarFrame);
        addFriend->setObjectName("addFriend");
        sizePolicy1.setHeightForWidth(addFriend->sizePolicy().hasHeightForWidth());
        addFriend->setSizePolicy(sizePolicy1);
        addFriend->setMinimumSize(QSize(100, 0));
        addFriend->setMaximumSize(QSize(75, 16777215));
        QFont font2;
        font2.setPointSize(12);
        font2.setBold(true);
        addFriend->setFont(font2);
        addFriend->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        addFriend->setAutoFillBackground(false);
        addFriend->setStyleSheet(QString::fromUtf8("\n"
"#addFriend{background: #32353a;}"));
        addFriend->setFlat(true);

        horizontalLayout_2->addWidget(addFriend);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_4->addWidget(topBarFrame);

        columnsContainer = new QWidget(friends);
        columnsContainer->setObjectName("columnsContainer");
        columnsContainer->setAutoFillBackground(false);
        columnsContainer->setStyleSheet(QString::fromUtf8(""));
        horizontalLayout = new QHBoxLayout(columnsContainer);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayoutCenter = new QVBoxLayout();
        verticalLayoutCenter->setSpacing(0);
        verticalLayoutCenter->setObjectName("verticalLayoutCenter");
        ADD_OUTGOING = new QStackedWidget(columnsContainer);
        ADD_OUTGOING->setObjectName("ADD_OUTGOING");
        QPalette palette1;
        QBrush brush12(QColor(50, 53, 58, 255));
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

        verticalLayoutCenter->addWidget(ADD_OUTGOING);


        horizontalLayout->addLayout(verticalLayoutCenter);

        verticalLayoutRight = new QVBoxLayout();
        verticalLayoutRight->setSpacing(0);
        verticalLayoutRight->setObjectName("verticalLayoutRight");
        verticalLayoutRight->setContentsMargins(0, -1, -1, -1);

        horizontalLayout->addLayout(verticalLayoutRight);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 1);

        verticalLayout_4->addWidget(columnsContainer);

        topBarColumnsContainer->addWidget(friends);
        directMessages = new QWidget();
        directMessages->setObjectName("directMessages");
        topBarColumnsContainer->addWidget(directMessages);

        horizontalLayout_3->addWidget(topBarColumnsContainer);

        MainWindow->setCentralWidget(mainContainer);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1093, 21));
        MainWindow->setMenuBar(menubar);

        retranslateUi(MainWindow);

        ADD_OUTGOING->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Discard", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Friends", nullptr));
        outgoing->setText(QCoreApplication::translate("MainWindow", "Pending", nullptr));
        addFriend->setText(QCoreApplication::translate("MainWindow", "Add Friend", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
