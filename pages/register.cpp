#include "register.h"
#include "ui_register.h"
#include "manager.h"
#include <QMessageBox>
#include <QDir>
#include <QFile>
Register::Register(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Register)
    , manager(Manager::get())
{
    ui->setupUi(this);
    setStyleSheet(Utils::readText(":/styles/login_register.qss", "login_register.qss"));
    ui->cancelButton->setProperty("role", "secondary");
    ui->usernameUnavailableError->hide();
    ui->nicknameUnavailableError->hide();
    ui->passwordNotMatchingError->hide();
    ui->passwordNotMatchingError_2->hide();
    connect(ui->registerUsername, &QLineEdit::editingFinished, this, &Register::onUsernameEditingFinished);
    connect(ui->registerNickname, &QLineEdit::editingFinished, this, &Register::onNicknameEditingFinished);
    connect(ui->registerButton, &QPushButton::clicked, this, &::Register::onRegisterButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &Register::onCancelButtonClicked);
}

Register::~Register()
{
    delete ui;
}

void Register::onUsernameEditingFinished(){
    QString username = ui->registerUsername->text();
    if(!(manager->checkUsernameAvailibity(username))){
        ui->usernameUnavailableError->show();
        ui->registerUsername->setStyleSheet(QString("QLineEdit { border: 1px solid red }"));
    }
    else{
        ui->usernameUnavailableError->hide();
        ui->registerUsername->setStyleSheet(QString("QLineEdit { border: 1px solid green }"));
    }
}
void Register::onNicknameEditingFinished(){
    QString username = ui->registerNickname->text();
    if(!(manager->checkNicknameAvailibity(username))){
        ui->nicknameUnavailableError->show();
        ui->registerNickname->setStyleSheet(QString("QLineEdit { border: 1px solid red }"));
    }
    else{
        ui->nicknameUnavailableError->hide();
        ui->registerNickname->setStyleSheet(QString("QLineEdit { border: 1px solid green }"));
    }
}
void Register::onRegisterButtonClicked() {
    if (ui->registerPassword->text() != ui->registerPasswordConfirmation->text()) {
        ui->passwordNotMatchingError->show();
        ui->passwordNotMatchingError_2->show();
        return;
    }
    if (manager->createUser(ui->registerUsername->text(), ui->registerPassword->text(), ui->registerNickname->text())) {
        accept();
    } else {
        QMessageBox::warning(this, tr("Registration failed"), manager->lastError());
    }
}
void Register::onCancelButtonClicked() {
    reject();
}



