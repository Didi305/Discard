/********************************************************************************
** Form generated from reading UI file 'dialog.ui'
**
** Created by: Qt User Interface Compiler version 6.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOG_H
#define UI_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog
{
public:
    QLineEdit *userNameInput;
    QLabel *label;
    QPushButton *okButton;
    QPushButton *cancelButton;

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName("Dialog");
        Dialog->resize(400, 300);
        userNameInput = new QLineEdit(Dialog);
        userNameInput->setObjectName("userNameInput");
        userNameInput->setGeometry(QRect(80, 160, 241, 24));
        label = new QLabel(Dialog);
        label->setObjectName("label");
        label->setGeometry(QRect(150, 130, 261, 20));
        okButton = new QPushButton(Dialog);
        okButton->setObjectName("okButton");
        okButton->setGeometry(QRect(160, 200, 80, 24));
        cancelButton = new QPushButton(Dialog);
        cancelButton->setObjectName("cancelButton");
        cancelButton->setGeometry(QRect(160, 230, 80, 24));

        retranslateUi(Dialog);

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QCoreApplication::translate("Dialog", "Dialog", nullptr));
        label->setText(QCoreApplication::translate("Dialog", "SMEYTEK A ZAML ", nullptr));
        okButton->setText(QCoreApplication::translate("Dialog", "merta7", nullptr));
        cancelButton->setText(QCoreApplication::translate("Dialog", "thala thala", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Dialog: public Ui_Dialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOG_H
