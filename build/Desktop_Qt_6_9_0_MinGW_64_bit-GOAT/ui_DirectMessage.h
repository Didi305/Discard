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
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DirectMessage
{
public:
    QVBoxLayout *verticalLayout;
    QTextBrowser *messageView;
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
        DirectMessage->resize(1063, 599);
        DirectMessage->setAutoFillBackground(true);
        DirectMessage->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(DirectMessage);
        verticalLayout->setObjectName("verticalLayout");
        messageView = new QTextBrowser(DirectMessage);
        messageView->setObjectName("messageView");

        verticalLayout->addWidget(messageView);

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
