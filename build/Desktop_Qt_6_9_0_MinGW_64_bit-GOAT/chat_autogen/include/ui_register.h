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

    void setupUi(QDialog *Register)
    {
        if (Register->objectName().isEmpty())
            Register->setObjectName("Register");
        Register->resize(634, 536);
        registerUsername = new QLineEdit(Register);
        registerUsername->setObjectName("registerUsername");
        registerUsername->setGeometry(QRect(240, 210, 171, 24));
        registerPassword = new QLineEdit(Register);
        registerPassword->setObjectName("registerPassword");
        registerPassword->setGeometry(QRect(240, 250, 171, 24));
        label_3 = new QLabel(Register);
        label_3->setObjectName("label_3");
        label_3->setGeometry(QRect(140, 240, 91, 31));
        QFont font;
        font.setPointSize(14);
        label_3->setFont(font);
        label_2 = new QLabel(Register);
        label_2->setObjectName("label_2");
        label_2->setGeometry(QRect(140, 200, 91, 31));
        label_2->setFont(font);
        registerButton = new QPushButton(Register);
        registerButton->setObjectName("registerButton");
        registerButton->setGeometry(QRect(200, 350, 101, 24));
        cancelButton = new QPushButton(Register);
        cancelButton->setObjectName("cancelButton");
        cancelButton->setGeometry(QRect(360, 350, 101, 24));
        label = new QLabel(Register);
        label->setObjectName("label");
        label->setGeometry(QRect(260, 110, 151, 51));
        QFont font1;
        font1.setPointSize(26);
        label->setFont(font1);
        label_4 = new QLabel(Register);
        label_4->setObjectName("label_4");
        label_4->setGeometry(QRect(140, 280, 91, 31));
        label_4->setFont(font);
        registerPasswordConfirmation = new QLineEdit(Register);
        registerPasswordConfirmation->setObjectName("registerPasswordConfirmation");
        registerPasswordConfirmation->setGeometry(QRect(240, 290, 171, 24));

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
        label_4->setText(QCoreApplication::translate("Register", "Confirm:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Register: public Ui_Register {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTER_H
