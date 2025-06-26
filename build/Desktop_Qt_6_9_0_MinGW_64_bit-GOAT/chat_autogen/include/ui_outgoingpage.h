/********************************************************************************
** Form generated from reading UI file 'outgoingpage.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OUTGOINGPAGE_H
#define UI_OUTGOINGPAGE_H

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

class Ui_OutgoingPage
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_2;
    QFrame *searchFrame;
    QHBoxLayout *addFriendLayout_2;
    QLineEdit *searchInput;
    QPushButton *searchButton;
    QScrollArea *scrollAreaOutgoing;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_5;
    QVBoxLayout *listLayout;

    void setupUi(QWidget *OutgoingPage)
    {
        if (OutgoingPage->objectName().isEmpty())
            OutgoingPage->setObjectName("OutgoingPage");
        OutgoingPage->resize(400, 714);
        OutgoingPage->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(OutgoingPage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(20);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(16, 5, 16, 35);
        label_2 = new QLabel(OutgoingPage);
        label_2->setObjectName("label_2");
        QFont font;
        font.setPointSize(18);
        font.setBold(true);
        label_2->setFont(font);
        label_2->setScaledContents(false);

        verticalLayout_2->addWidget(label_2);

        searchFrame = new QFrame(OutgoingPage);
        searchFrame->setObjectName("searchFrame");
        searchFrame->setMinimumSize(QSize(150, 55));
        searchFrame->setAutoFillBackground(false);
        searchFrame->setStyleSheet(QString::fromUtf8("background-color: rgb(24, 26, 27)"));
        searchFrame->setFrameShape(QFrame::Shape::NoFrame);
        searchFrame->setFrameShadow(QFrame::Shadow::Raised);
        addFriendLayout_2 = new QHBoxLayout(searchFrame);
        addFriendLayout_2->setSpacing(0);
        addFriendLayout_2->setObjectName("addFriendLayout_2");
        addFriendLayout_2->setContentsMargins(20, 9, 12, 9);
        searchInput = new QLineEdit(searchFrame);
        searchInput->setObjectName("searchInput");
        searchInput->setFrame(false);

        addFriendLayout_2->addWidget(searchInput);

        searchButton = new QPushButton(searchFrame);
        searchButton->setObjectName("searchButton");
        searchButton->setMinimumSize(QSize(100, 30));
        QFont font1;
        font1.setPointSize(11);
        font1.setBold(true);
        searchButton->setFont(font1);
        searchButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        searchButton->setStyleSheet(QString::fromUtf8("background-color: green"));
        searchButton->setFlat(false);

        addFriendLayout_2->addWidget(searchButton);


        verticalLayout_2->addWidget(searchFrame);


        verticalLayout->addLayout(verticalLayout_2);

        scrollAreaOutgoing = new QScrollArea(OutgoingPage);
        scrollAreaOutgoing->setObjectName("scrollAreaOutgoing");
        scrollAreaOutgoing->setStyleSheet(QString::fromUtf8("#scrollAreaPending {\n"
"	border-top: 1px solid #3a3c43;\n"
"}"));
        scrollAreaOutgoing->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOn);
        scrollAreaOutgoing->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustIgnored);
        scrollAreaOutgoing->setWidgetResizable(true);
        scrollAreaOutgoing->setAlignment(Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignRight|Qt::AlignmentFlag::AlignTrailing);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 384, 565));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(scrollAreaWidgetContents->sizePolicy().hasHeightForWidth());
        scrollAreaWidgetContents->setSizePolicy(sizePolicy);
        scrollAreaWidgetContents->setStyleSheet(QString::fromUtf8("#scrollAreaWidgetContents {\n"
"	border-top: 1px solid #3a3c43;\n"
"}"));
        verticalLayout_5 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setObjectName("verticalLayout_5");
        verticalLayout_5->setContentsMargins(15, 0, 0, 0);
        listLayout = new QVBoxLayout();
        listLayout->setSpacing(0);
        listLayout->setObjectName("listLayout");
        listLayout->setContentsMargins(-1, -1, 8, -1);

        verticalLayout_5->addLayout(listLayout);

        scrollAreaOutgoing->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollAreaOutgoing);


        retranslateUi(OutgoingPage);

        QMetaObject::connectSlotsByName(OutgoingPage);
    } // setupUi

    void retranslateUi(QWidget *OutgoingPage)
    {
        OutgoingPage->setWindowTitle(QCoreApplication::translate("OutgoingPage", "Form", nullptr));
        label_2->setText(QCoreApplication::translate("OutgoingPage", "<html><head/><body><p>Outgoing Friend Requests</p></body></html>", nullptr));
        searchButton->setText(QCoreApplication::translate("OutgoingPage", "Search", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OutgoingPage: public Ui_OutgoingPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OUTGOINGPAGE_H
