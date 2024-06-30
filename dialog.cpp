#include "dialog.h"
#include "ui_dialog.h"
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    connect(ui->cancelButton, &QPushButton::clicked, this, &Dialog::on_cancelButton_clicked);
    connect(ui->okButton, &QPushButton::clicked, this, &Dialog::on_okButton_clicked);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_cancelButton_clicked() {
    reject();
    QApplication::exit();
}

void Dialog::on_okButton_clicked() {
    accept();
}

