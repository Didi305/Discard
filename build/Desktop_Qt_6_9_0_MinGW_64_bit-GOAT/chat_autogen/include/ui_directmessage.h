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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTextEdit>
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
    QTextEdit *messageInputLine;
    QToolButton *toolButton;
    QToolButton *toolButton_2;

    void setupUi(QWidget *DirectMessage)
    {
        if (DirectMessage->objectName().isEmpty())
            DirectMessage->setObjectName("DirectMessage");
        DirectMessage->resize(441, 599);
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
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 417, 517));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        receiverInformationFrame = new QFrame(scrollAreaWidgetContents);
        receiverInformationFrame->setObjectName("receiverInformationFrame");
        receiverInformationFrame->setMaximumSize(QSize(16777215, 200));
        receiverInformationFrame->setStyleSheet(QString::fromUtf8(""));
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
        messagesContainer->setStyleSheet(QString::fromUtf8(""));

        verticalLayout_2->addWidget(messagesContainer);

        messagesScrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(messagesScrollArea);

        messageInputFrame = new QFrame(DirectMessage);
        messageInputFrame->setObjectName("messageInputFrame");
        QSizePolicy sizePolicy(QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(messageInputFrame->sizePolicy().hasHeightForWidth());
        messageInputFrame->setSizePolicy(sizePolicy);
        messageInputFrame->setMinimumSize(QSize(0, 0));
        messageInputFrame->setMaximumSize(QSize(16777215, 80));
        messageInputFrame->setBaseSize(QSize(40, 50));
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

        messageInputLine = new QTextEdit(messageInputFrame);
        messageInputLine->setObjectName("messageInputLine");
        messageInputLine->setMaximumSize(QSize(16777215, 400));
        messageInputLine->setInputMethodHints(Qt::InputMethodHint::ImhDate|Qt::InputMethodHint::ImhMultiLine|Qt::InputMethodHint::ImhTime);
        messageInputLine->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        messageInputLine->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        messageInputLine->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
        messageInputLine->setAutoFormatting(QTextEdit::AutoFormattingFlag::AutoAll);

        horizontalLayout->addWidget(messageInputLine);

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
        startConversationLabel->setText(QCoreApplication::translate("DirectMessage", "This is the start of the conversation", nullptr));
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
