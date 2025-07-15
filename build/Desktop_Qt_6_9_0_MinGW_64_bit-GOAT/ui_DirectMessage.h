/********************************************************************************
** Form generated from reading UI file 'directmessage.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIRECTMESSAGE_H
#define UI_DIRECTMESSAGE_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DirectMessage
{
public:
    QVBoxLayout *verticalLayout;
    QScrollArea *messagesScrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QFrame *receiverInformationFrame;
    QVBoxLayout *verticalLayout_3;
    QLabel *receiverAvatar;
    QLabel *receiverUsername;
    QLabel *startConversationLabel;
    QWidget *messagesContainer;
    QFrame *messageInputFrame;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QLineEdit *lineEdit;
    QToolButton *toolButton;
    QToolButton *toolButton_2;

    void setupUi(QWidget *DirectMessage)
    {
        if (DirectMessage->objectName().isEmpty())
            DirectMessage->setObjectName("DirectMessage");
        DirectMessage->resize(441, 602);
        DirectMessage->setAutoFillBackground(true);
        DirectMessage->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(DirectMessage);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(11, 0, 11, 0);
        messagesScrollArea = new QScrollArea(DirectMessage);
        messagesScrollArea->setObjectName("messagesScrollArea");
        messagesScrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName("scrollAreaWidgetContents");
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 417, 536));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        receiverInformationFrame = new QFrame(scrollAreaWidgetContents);
        receiverInformationFrame->setObjectName("receiverInformationFrame");
        receiverInformationFrame->setMaximumSize(QSize(16777215, 200));
        receiverInformationFrame->setStyleSheet(QString::fromUtf8("background: red;"));
        receiverInformationFrame->setFrameShape(QFrame::Shape::StyledPanel);
        receiverInformationFrame->setFrameShadow(QFrame::Shadow::Plain);
        verticalLayout_3 = new QVBoxLayout(receiverInformationFrame);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(-1, 0, 0, 0);
        receiverAvatar = new QLabel(receiverInformationFrame);
        receiverAvatar->setObjectName("receiverAvatar");
        receiverAvatar->setMaximumSize(QSize(56, 56));

        verticalLayout_3->addWidget(receiverAvatar);

        receiverUsername = new QLabel(receiverInformationFrame);
        receiverUsername->setObjectName("receiverUsername");
        receiverUsername->setMaximumSize(QSize(120, 25));

        verticalLayout_3->addWidget(receiverUsername);

        startConversationLabel = new QLabel(receiverInformationFrame);
        startConversationLabel->setObjectName("startConversationLabel");
        startConversationLabel->setMaximumSize(QSize(300, 30));

        verticalLayout_3->addWidget(startConversationLabel);


        verticalLayout_2->addWidget(receiverInformationFrame);

        messagesContainer = new QWidget(scrollAreaWidgetContents);
        messagesContainer->setObjectName("messagesContainer");
        messagesContainer->setStyleSheet(QString::fromUtf8("background: lime;"));

        verticalLayout_2->addWidget(messagesContainer);

        messagesScrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(messagesScrollArea);

        messageInputFrame = new QFrame(DirectMessage);
        messageInputFrame->setObjectName("messageInputFrame");
        messageInputFrame->setMinimumSize(QSize(330, 60));
        messageInputFrame->setMaximumSize(QSize(16777215, 16777215));
        messageInputFrame->setAutoFillBackground(true);
        messageInputFrame->setStyleSheet(QString::fromUtf8(""));
        messageInputFrame->setFrameShape(QFrame::Shape::StyledPanel);
        messageInputFrame->setFrameShadow(QFrame::Shadow::Raised);
        horizontalLayout = new QHBoxLayout(messageInputFrame);
        horizontalLayout->setObjectName("horizontalLayout");
        pushButton = new QPushButton(messageInputFrame);
        pushButton->setObjectName("pushButton");
        pushButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        pushButton->setStyleSheet(QString::fromUtf8(""));
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd));
        pushButton->setIcon(icon);
        pushButton->setIconSize(QSize(24, 24));
        pushButton->setFlat(true);

        horizontalLayout->addWidget(pushButton);

        lineEdit = new QLineEdit(messageInputFrame);
        lineEdit->setObjectName("lineEdit");
        lineEdit->setMinimumSize(QSize(0, 44));
        lineEdit->setFrame(false);

        horizontalLayout->addWidget(lineEdit);

        toolButton = new QToolButton(messageInputFrame);
        toolButton->setObjectName("toolButton");
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::AddressBookNew));
        toolButton->setIcon(icon1);

        horizontalLayout->addWidget(toolButton);

        toolButton_2 = new QToolButton(messageInputFrame);
        toolButton_2->setObjectName("toolButton_2");

        horizontalLayout->addWidget(toolButton_2);


        verticalLayout->addWidget(messageInputFrame);


        retranslateUi(DirectMessage);

        QMetaObject::connectSlotsByName(DirectMessage);
    } // setupUi

    void retranslateUi(QWidget *DirectMessage)
    {
        DirectMessage->setWindowTitle(QCoreApplication::translate("DirectMessage", "Form", nullptr));
        receiverAvatar->setText(QCoreApplication::translate("DirectMessage", "xdd", nullptr));
        receiverUsername->setText(QCoreApplication::translate("DirectMessage", "TextLabel", nullptr));
        startConversationLabel->setText(QCoreApplication::translate("DirectMessage", "xxxxxxxxxxxxxxxxxxxxxxxxxxx", nullptr));
        pushButton->setText(QString());
        toolButton->setText(QCoreApplication::translate("DirectMessage", "...", nullptr));
        toolButton_2->setText(QCoreApplication::translate("DirectMessage", "GIF", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DirectMessage: public Ui_DirectMessage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIRECTMESSAGE_H
