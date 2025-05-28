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
    QLabel *label;
    QLineEdit *loginUsername;
    QLineEdit *loginPassword;
    QLabel *label_2;
    QLabel *label_3;

    void setupUi(QDialog *Login)
    {
        if (Login->objectName().isEmpty())
            Login->setObjectName("Login");
        Login->resize(607, 514);
        loginButton = new QPushButton(Login);
        loginButton->setObjectName("loginButton");
        loginButton->setGeometry(QRect(200, 300, 101, 24));
        createAccountButton = new QPushButton(Login);
        createAccountButton->setObjectName("createAccountButton");
        createAccountButton->setGeometry(QRect(360, 300, 101, 24));
        label = new QLabel(Login);
        label->setObjectName("label");
        label->setGeometry(QRect(270, 100, 121, 51));
        QFont font;
        font.setPointSize(26);
        label->setFont(font);
        loginUsername = new QLineEdit(Login);
        loginUsername->setObjectName("loginUsername");
        loginUsername->setGeometry(QRect(240, 190, 171, 24));
        loginPassword = new QLineEdit(Login);
        loginPassword->setObjectName("loginPassword");
        loginPassword->setGeometry(QRect(240, 230, 171, 24));
        label_2 = new QLabel(Login);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(140, 180, 91, 31));
        QFont font1;
        font1.setPointSize(14);
        label_2->setFont(font1);
        label_3 = new QLabel(Login);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(140, 220, 91, 31));
        label_3->setFont(font1);

        retranslateUi(Login);

        QMetaObject::connectSlotsByName(Login);
    } // setupUi

    void retranslateUi(QDialog *Login)
    {
        Login->setWindowTitle(QCoreApplication::translate("Login", "Dialog", nullptr));
        loginButton->setText(QCoreApplication::translate("Login", "Login", nullptr));
        createAccountButton->setText(QCoreApplication::translate("Login", "Create Account", nullptr));
        label->setText(QCoreApplication::translate("Login", "Login", nullptr));
        label_2->setText(QCoreApplication::translate("Login", "Username:", nullptr));
        label_3->setText(QCoreApplication::translate("Login", "Password:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Login: public Ui_Login {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGIN_H
