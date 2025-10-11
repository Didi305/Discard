/********************************************************************************
** Form generated from reading UI file 'login.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGIN_H
#define UI_LOGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Login
{
public:
    QPushButton *loginButton;
    QPushButton *createAccountButton;
    QLabel *title;
    QLineEdit *loginUsername;
    QLineEdit *loginPassword;
    QLabel *username;
    QLabel *password;
    QLabel *loginErrorMessage;

    void setupUi(QDialog *Login)
    {
        if (Login->objectName().isEmpty())
            Login->setObjectName("Login");
        Login->resize(607, 514);
        Login->setAutoFillBackground(false);
        loginButton = new QPushButton(Login);
        loginButton->setObjectName("loginButton");
        loginButton->setGeometry(QRect(170, 360, 101, 35));
        QSizePolicy sizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(loginButton->sizePolicy().hasHeightForWidth());
        loginButton->setSizePolicy(sizePolicy);
        loginButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        createAccountButton = new QPushButton(Login);
        createAccountButton->setObjectName("createAccountButton");
        createAccountButton->setGeometry(QRect(330, 360, 101, 35));
        createAccountButton->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
        title = new QLabel(Login);
        title->setObjectName("title");
        title->setGeometry(QRect(260, 80, 121, 71));
        QFont font;
        font.setPointSize(26);
        title->setFont(font);
        loginUsername = new QLineEdit(Login);
        loginUsername->setObjectName("loginUsername");
        loginUsername->setGeometry(QRect(230, 200, 171, 41));
        loginUsername->setAutoFillBackground(false);
        loginUsername->setStyleSheet(QString::fromUtf8(""));
        loginPassword = new QLineEdit(Login);
        loginPassword->setObjectName("loginPassword");
        loginPassword->setGeometry(QRect(230, 250, 171, 41));
        loginPassword->setAutoFillBackground(false);
        loginPassword->setEchoMode(QLineEdit::EchoMode::Password);
        username = new QLabel(Login);
        username->setObjectName("username");
        username->setGeometry(QRect(133, 200, 91, 31));
        QFont font1;
        font1.setPointSize(14);
        username->setFont(font1);
        password = new QLabel(Login);
        password->setObjectName("password");
        password->setGeometry(QRect(139, 250, 91, 31));
        password->setFont(font1);
        loginErrorMessage = new QLabel(Login);
        loginErrorMessage->setObjectName("loginErrorMessage");
        loginErrorMessage->setEnabled(false);
        loginErrorMessage->setGeometry(QRect(230, 170, 211, 16));
        loginErrorMessage->setMouseTracking(true);
        loginErrorMessage->setFrameShape(QFrame::Shape::NoFrame);
        loginErrorMessage->setScaledContents(false);

        retranslateUi(Login);

        QMetaObject::connectSlotsByName(Login);
    } // setupUi

    void retranslateUi(QDialog *Login)
    {
        Login->setWindowTitle(QString());
        loginButton->setText(QCoreApplication::translate("Login", "Login", nullptr));
        createAccountButton->setText(QCoreApplication::translate("Login", "Create Account", nullptr));
        title->setText(QCoreApplication::translate("Login", "Login", nullptr));
        username->setText(QCoreApplication::translate("Login", "Username:", nullptr));
        password->setText(QCoreApplication::translate("Login", "Password:", nullptr));
        loginErrorMessage->setText(QCoreApplication::translate("Login", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700; color:#aa0000;\">Username or password incorrect</span></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Login: public Ui_Login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
