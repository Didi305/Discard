/********************************************************************************
** Form generated from reading UI file 'register.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTER_H
#define UI_REGISTER_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Register
{
public:
    QLineEdit *registerUsername;
    QLineEdit *registerPassword;
    QLabel *label_3;
    QLabel *label_2;
    QPushButton *registerButton;
    QPushButton *cancelButton;
    QLabel *label;
    QLabel *label_4;
    QLineEdit *registerPasswordConfirmation;
    QLabel *usernameUnavailableError;
    QLabel *passwordNotMatchingError_2;
    QLabel *passwordNotMatchingError;
    QLabel *label_5;
    QLabel *nicknameUnavailableError;
    QLineEdit *registerNickname;

    void setupUi(QDialog *Register)
    {
        if (Register->objectName().isEmpty())
            Register->setObjectName("Register");
        Register->resize(634, 536);
        registerUsername = new QLineEdit(Register);
        registerUsername->setObjectName("registerUsername");
        registerUsername->setEnabled(true);
        registerUsername->setGeometry(QRect(240, 130, 171, 24));
        registerPassword = new QLineEdit(Register);
        registerPassword->setObjectName("registerPassword");
        registerPassword->setGeometry(QRect(240, 260, 171, 24));
        label_3 = new QLabel(Register);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(140, 250, 91, 31));
        QFont font;
        font.setPointSize(14);
        label_3->setFont(font);
        label_2 = new QLabel(Register);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(140, 120, 91, 31));
        label_2->setFont(font);
        registerButton = new QPushButton(Register);
        registerButton->setObjectName("registerButton");
        registerButton->setGeometry(QRect(190, 410, 101, 24));
        cancelButton = new QPushButton(Register);
        cancelButton->setObjectName("cancelButton");
        cancelButton->setGeometry(QRect(350, 410, 101, 24));
        label = new QLabel(Register);
        label->setObjectName("label");
        label->setGeometry(QRect(250, 30, 151, 51));
        QFont font1;
        font1.setPointSize(26);
        label->setFont(font1);
        label_4 = new QLabel(Register);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(70, 310, 161, 31));
        label_4->setFont(font);
        registerPasswordConfirmation = new QLineEdit(Register);
        registerPasswordConfirmation->setObjectName("registerPasswordConfirmation");
        registerPasswordConfirmation->setGeometry(QRect(240, 320, 171, 24));
        usernameUnavailableError = new QLabel(Register);
        usernameUnavailableError->setObjectName("usernameUnavailableError");
        usernameUnavailableError->setEnabled(false);
        usernameUnavailableError->setGeometry(QRect(240, 110, 151, 16));
        usernameUnavailableError->setMouseTracking(true);
        usernameUnavailableError->setFrameShape(QFrame::Shape::NoFrame);
        usernameUnavailableError->setScaledContents(false);
        passwordNotMatchingError_2 = new QLabel(Register);
        passwordNotMatchingError_2->setObjectName("passwordNotMatchingError_2");
        passwordNotMatchingError_2->setEnabled(false);
        passwordNotMatchingError_2->setGeometry(QRect(240, 300, 151, 16));
        passwordNotMatchingError_2->setMouseTracking(true);
        passwordNotMatchingError_2->setFrameShape(QFrame::Shape::NoFrame);
        passwordNotMatchingError_2->setScaledContents(false);
        passwordNotMatchingError = new QLabel(Register);
        passwordNotMatchingError->setObjectName("passwordNotMatchingError");
        passwordNotMatchingError->setEnabled(false);
        passwordNotMatchingError->setGeometry(QRect(240, 240, 151, 16));
        passwordNotMatchingError->setMouseTracking(true);
        passwordNotMatchingError->setFrameShape(QFrame::Shape::NoFrame);
        passwordNotMatchingError->setScaledContents(false);
        label_5 = new QLabel(Register);
        label_5->setObjectName("label_5");
        label_5->setGeometry(QRect(140, 180, 91, 31));
        label_5->setFont(font);
        nicknameUnavailableError = new QLabel(Register);
        nicknameUnavailableError->setObjectName("nicknameUnavailableError");
        nicknameUnavailableError->setEnabled(false);
        nicknameUnavailableError->setGeometry(QRect(240, 170, 151, 16));
        nicknameUnavailableError->setMouseTracking(true);
        nicknameUnavailableError->setFrameShape(QFrame::Shape::NoFrame);
        nicknameUnavailableError->setScaledContents(false);
        registerNickname = new QLineEdit(Register);
        registerNickname->setObjectName("registerNickname");
        registerNickname->setEnabled(true);
        registerNickname->setGeometry(QRect(240, 190, 171, 24));
        usernameUnavailableError->raise();
        registerUsername->raise();
        registerPassword->raise();
        label_3->raise();
        label_2->raise();
        registerButton->raise();
        cancelButton->raise();
        label->raise();
        label_4->raise();
        registerPasswordConfirmation->raise();
        passwordNotMatchingError_2->raise();
        passwordNotMatchingError->raise();
        label_5->raise();
        nicknameUnavailableError->raise();
        registerNickname->raise();

        retranslateUi(Register);

        QMetaObject::connectSlotsByName(Register);
    } // setupUi

    void retranslateUi(QDialog *Register)
    {
        Register->setWindowTitle(QCoreApplication::translate("Register", "Dialog", nullptr));
        label_3->setText(QCoreApplication::translate("Register", "Password:", nullptr));
        label_2->setText(QCoreApplication::translate("Register", "Username:", nullptr));
        registerButton->setText(QCoreApplication::translate("Register", "Create", nullptr));
        cancelButton->setText(QCoreApplication::translate("Register", "Cancel", nullptr));
        label->setText(QCoreApplication::translate("Register", "Register", nullptr));
        label_4->setText(QCoreApplication::translate("Register", "Confirm Password:", nullptr));
        usernameUnavailableError->setText(QCoreApplication::translate("Register", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700; color:#aa0000;\">Username is unavailable</span></p></body></html>", nullptr));
        passwordNotMatchingError_2->setText(QCoreApplication::translate("Register", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700; color:#aa0000;\">Password not matching</span></p></body></html>", nullptr));
        passwordNotMatchingError->setText(QCoreApplication::translate("Register", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700; color:#aa0000;\">Password not matching</span></p></body></html>", nullptr));
        label_5->setText(QCoreApplication::translate("Register", "Nickname:", nullptr));
        nicknameUnavailableError->setText(QCoreApplication::translate("Register", "<html><head/><body><p><span style=\" font-size:10pt; font-weight:700; color:#aa0000;\">Nickname is unavailable</span></p></body></html>", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Register: public Ui_Register {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTER_H
