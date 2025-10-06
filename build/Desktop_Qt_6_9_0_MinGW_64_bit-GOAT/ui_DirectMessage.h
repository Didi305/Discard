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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DirectMessage
{
public:
    QVBoxLayout *verticalLayout_2;
    QTextBrowser *messageView;
    QFrame *messageInputFrame;
    QVBoxLayout *verticalLayout;
    QScrollArea *scrollArea;
    QWidget *inputAttachements;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *leftButtonLayout;
    QPushButton *addAttachButton;
    QTextEdit *messageInputLine;
    QHBoxLayout *rightButtonsLayout;
    QToolButton *emotesButton;
    QToolButton *GIFButton;
    QPushButton *sendButton;

    void setupUi(QWidget *DirectMessage)
    {
        if (DirectMessage->objectName().isEmpty())
            DirectMessage->setObjectName("DirectMessage");
        DirectMessage->resize(1063, 599);
        DirectMessage->setAutoFillBackground(true);
        DirectMessage->setStyleSheet(QString::fromUtf8(""));
        verticalLayout_2 = new QVBoxLayout(DirectMessage);
        verticalLayout_2->setObjectName("verticalLayout_2");
        messageView = new QTextBrowser(DirectMessage);
        messageView->setObjectName("messageView");
        QFont font;
        font.setPointSize(8);
        messageView->setFont(font);

        verticalLayout_2->addWidget(messageView);

        messageInputFrame = new QFrame(DirectMessage);
        messageInputFrame->setObjectName("messageInputFrame");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(messageInputFrame->sizePolicy().hasHeightForWidth());
        messageInputFrame->setSizePolicy(sizePolicy);
        messageInputFrame->setMinimumSize(QSize(0, 75));
        messageInputFrame->setMaximumSize(QSize(16777215, 16777215));
        messageInputFrame->setBaseSize(QSize(40, 50));
        messageInputFrame->setAutoFillBackground(true);
        messageInputFrame->setStyleSheet(QString::fromUtf8(""));
        messageInputFrame->setFrameShape(QFrame::Shape::StyledPanel);
        messageInputFrame->setFrameShadow(QFrame::Shadow::Raised);
        verticalLayout = new QVBoxLayout(messageInputFrame);
        verticalLayout->setObjectName("verticalLayout");
        scrollArea = new QScrollArea(messageInputFrame);
        scrollArea->setObjectName("scrollArea");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(scrollArea->sizePolicy().hasHeightForWidth());
        scrollArea->setSizePolicy(sizePolicy1);
        scrollArea->setMinimumSize(QSize(0, 0));
        scrollArea->setMaximumSize(QSize(16777215, 16777215));
        scrollArea->setAutoFillBackground(true);
        scrollArea->setFrameShape(QFrame::Shape::NoFrame);
        scrollArea->setWidgetResizable(true);
        inputAttachements = new QWidget();
        inputAttachements->setObjectName("inputAttachements");
        inputAttachements->setGeometry(QRect(0, 0, 1025, 68));
        QSizePolicy sizePolicy2(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(inputAttachements->sizePolicy().hasHeightForWidth());
        inputAttachements->setSizePolicy(sizePolicy2);
        inputAttachements->setMinimumSize(QSize(0, 0));
        inputAttachements->setStyleSheet(QString::fromUtf8(""));
        scrollArea->setWidget(inputAttachements);

        verticalLayout->addWidget(scrollArea);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        leftButtonLayout = new QVBoxLayout();
        leftButtonLayout->setObjectName("leftButtonLayout");
        leftButtonLayout->setContentsMargins(-1, 4, -1, -1);
        addAttachButton = new QPushButton(messageInputFrame);
        addAttachButton->setObjectName("addAttachButton");
        sizePolicy2.setHeightForWidth(addAttachButton->sizePolicy().hasHeightForWidth());
        addAttachButton->setSizePolicy(sizePolicy2);
        addAttachButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        addAttachButton->setStyleSheet(QString::fromUtf8(""));
        QIcon icon(QIcon::fromTheme(QIcon::ThemeIcon::ListAdd));
        addAttachButton->setIcon(icon);
        addAttachButton->setIconSize(QSize(24, 24));
        addAttachButton->setFlat(true);

        leftButtonLayout->addWidget(addAttachButton);


        horizontalLayout->addLayout(leftButtonLayout);

        messageInputLine = new QTextEdit(messageInputFrame);
        messageInputLine->setObjectName("messageInputLine");
        sizePolicy1.setHeightForWidth(messageInputLine->sizePolicy().hasHeightForWidth());
        messageInputLine->setSizePolicy(sizePolicy1);
        messageInputLine->setMinimumSize(QSize(0, 20));
        messageInputLine->setMaximumSize(QSize(16777215, 16777215));
        messageInputLine->setInputMethodHints(Qt::InputMethodHint::ImhDate|Qt::InputMethodHint::ImhMultiLine|Qt::InputMethodHint::ImhTime);
        messageInputLine->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        messageInputLine->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
        messageInputLine->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
        messageInputLine->setAutoFormatting(QTextEdit::AutoFormattingFlag::AutoAll);

        horizontalLayout->addWidget(messageInputLine);

        rightButtonsLayout = new QHBoxLayout();
        rightButtonsLayout->setSpacing(24);
        rightButtonsLayout->setObjectName("rightButtonsLayout");
        rightButtonsLayout->setContentsMargins(-1, 4, 6, -1);
        emotesButton = new QToolButton(messageInputFrame);
        emotesButton->setObjectName("emotesButton");
        emotesButton->setMinimumSize(QSize(32, 32));
        emotesButton->setMaximumSize(QSize(32, 32));
        emotesButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        emotesButton->setIconSize(QSize(21, 21));
        emotesButton->setAutoRaise(true);

        rightButtonsLayout->addWidget(emotesButton);

        GIFButton = new QToolButton(messageInputFrame);
        GIFButton->setObjectName("GIFButton");
        GIFButton->setMinimumSize(QSize(28, 28));
        QFont font1;
        font1.setPointSize(9);
        GIFButton->setFont(font1);
        GIFButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        GIFButton->setStyleSheet(QString::fromUtf8(""));
        GIFButton->setIconSize(QSize(24, 24));
        GIFButton->setAutoRaise(false);

        rightButtonsLayout->addWidget(GIFButton);

        sendButton = new QPushButton(messageInputFrame);
        sendButton->setObjectName("sendButton");
        sendButton->setMinimumSize(QSize(28, 28));
        sendButton->setMaximumSize(QSize(32, 32));
        sendButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        sendButton->setAutoFillBackground(false);
        sendButton->setStyleSheet(QString::fromUtf8(""));
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend));
        sendButton->setIcon(icon1);
        sendButton->setIconSize(QSize(24, 24));
        sendButton->setFlat(true);

        rightButtonsLayout->addWidget(sendButton);


        horizontalLayout->addLayout(rightButtonsLayout);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addWidget(messageInputFrame);


        retranslateUi(DirectMessage);

        QMetaObject::connectSlotsByName(DirectMessage);
    } // setupUi

    void retranslateUi(QWidget *DirectMessage)
    {
        DirectMessage->setWindowTitle(QCoreApplication::translate("DirectMessage", "Form", nullptr));
        addAttachButton->setText(QString());
        emotesButton->setText(QCoreApplication::translate("DirectMessage", "...", nullptr));
        GIFButton->setText(QCoreApplication::translate("DirectMessage", "GIF", nullptr));
        sendButton->setText(QString());
#if QT_CONFIG(shortcut)
        sendButton->setShortcut(QString());
#endif // QT_CONFIG(shortcut)
    } // retranslateUi

};

namespace Ui {
    class DirectMessage: public Ui_DirectMessage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIRECTMESSAGE_H
