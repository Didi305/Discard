/********************************************************************************
** Form generated from reading UI file 'addfriendpage.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDFRIENDPAGE_H
#define UI_ADDFRIENDPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddFriendPage
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_7;
    QFrame *addFriendFrame;
    QHBoxLayout *addFriendLayout_6;
    QLineEdit *addFriendInput;
    QPushButton *addFriendButton;
    QScrollArea *scrollAreaPending;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_13;
    QVBoxLayout *listLayout;

    void setupUi(QWidget *AddFriendPage)
    {
        if (AddFriendPage->objectName().isEmpty())
            AddFriendPage->setObjectName("AddFriendPage");
        AddFriendPage->resize(400, 738);
        AddFriendPage->setAutoFillBackground(false);
        AddFriendPage->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(AddFriendPage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(20);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(16, 5, 16, 35);
        label_7 = new QLabel(AddFriendPage);
        label_7->setObjectName("label_7");
        QFont font;
        font.setPointSize(18);
        font.setBold(true);
        label_7->setFont(font);
        label_7->setScaledContents(false);

        verticalLayout_2->addWidget(label_7);

        addFriendFrame = new QFrame(AddFriendPage);
        addFriendFrame->setObjectName("addFriendFrame");
        addFriendFrame->setMinimumSize(QSize(150, 55));
        addFriendFrame->setAutoFillBackground(false);
        addFriendFrame->setStyleSheet(QString::fromUtf8("background-color: rgb(24, 26, 27)"));
        addFriendFrame->setFrameShape(QFrame::Shape::NoFrame);
        addFriendFrame->setFrameShadow(QFrame::Shadow::Raised);
        addFriendLayout_6 = new QHBoxLayout(addFriendFrame);
        addFriendLayout_6->setSpacing(0);
        addFriendLayout_6->setObjectName("addFriendLayout_6");
        addFriendLayout_6->setContentsMargins(20, 9, 12, 9);
        addFriendInput = new QLineEdit(addFriendFrame);
        addFriendInput->setObjectName("addFriendInput");
        addFriendInput->setFrame(false);

        addFriendLayout_6->addWidget(addFriendInput);

        addFriendButton = new QPushButton(addFriendFrame);
        addFriendButton->setObjectName("addFriendButton");
        addFriendButton->setMinimumSize(QSize(100, 30));
        QFont font1;
        font1.setPointSize(11);
        font1.setBold(true);
        addFriendButton->setFont(font1);
        addFriendButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        addFriendButton->setStyleSheet(QString::fromUtf8("background-color: green"));
        addFriendButton->setFlat(false);

        addFriendLayout_6->addWidget(addFriendButton);


        verticalLayout_2->addWidget(addFriendFrame);


        verticalLayout->addLayout(verticalLayout_2);

        scrollAreaPending = new QScrollArea(AddFriendPage);
        scrollAreaPending->setObjectName("scrollAreaPending");
        scrollAreaPending->setAutoFillBackground(false);
        scrollAreaPending->setStyleSheet(QString::fromUtf8(""));
        scrollAreaPending->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
        scrollAreaPending->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored);
        scrollAreaPending->setWidgetResizable(true);
        scrollAreaPending->setAlignment(Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 384, 525));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy);
        scrollAreaWidgetContents->setAutoFillBackground(true);
        scrollAreaWidgetContents->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_13 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_13->setSpacing(0);
        verticalLayout_13->setObjectName("verticalLayout_13");
        verticalLayout_13->setContentsMargins(15, 0, 0, 0);
        listLayout = new QVBoxLayout();
        listLayout->setSpacing(0);
        listLayout->setObjectName("listLayout");
        listLayout->setContentsMargins(-1, -1, 8, -1);

        verticalLayout_13->addLayout(listLayout);

        scrollAreaPending->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollAreaPending);


        retranslateUi(AddFriendPage);

        QMetaObject::connectSlotsByName(AddFriendPage);
    } // setupUi

    void retranslateUi(QWidget *AddFriendPage)
    {
        AddFriendPage->setWindowTitle(QCoreApplication::translate("AddFriendPage", "Form", nullptr));
        label_7->setText(QCoreApplication::translate("AddFriendPage", "<html><head/><body><p>Add a Friend</p><p><br/><span style=\" font-size:11pt;\">Just do it you friendless dog</span></p></body></html>", nullptr));
        addFriendButton->setText(QCoreApplication::translate("AddFriendPage", "Add Friend", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddFriendPage: public Ui_AddFriendPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDFRIENDPAGE_H
