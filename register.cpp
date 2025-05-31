#include "register.h"
#include "ui_register.h"
#include "client.h"
#include "manager.h"
#include <QErrorMessage>
Register::Register(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Register)
    , client_(nullptr)
    , manager(Manager::get())
{
    ui->setupUi(this);
    ui->usernameUnavailableError->hide();
    ui->passwordNotMatchingError->hide();
    ui->passwordNotMatchingError_2->hide();
    connect(ui->registerUsername, &QLineEdit::editingFinished, this, &Register::onUsernameEditingFinished);
    connect(ui->registerButton, &QPushButton::clicked, this, &::Register::onRegisterButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &Register::onCancelButtonClicked);
}

Register::~Register()
{
    delete ui;
}
void Register::setClient(Client *client){
    this->client_ = client;
}

void Register::onUsernameEditingFinished(){
    QString username = ui->registerUsername->text();
    if(!(manager->checkAvailibity(username))){
        ui->usernameUnavailableError->show();
        ui->registerUsername->setStyleSheet(QString("QLineEdit { border: 1px solid red }"));
    }
    else{
        ui->usernameUnavailableError->hide();
        ui->registerUsername->setStyleSheet(QString("QLineEdit { border: 1px solid green }"));
    }
}
void Register::onRegisterButtonClicked() {
    bool validCreation = false;
    QErrorMessage error;
    QString username = ui->registerUsername->text();
    QString password = ui->registerPassword->text();
    QString password_confirmation = ui->registerPasswordConfirmation->text();
    if(password != password_confirmation){
        ui->passwordNotMatchingError->show();
        ui->passwordNotMatchingError_2->show();
    }
    validCreation = true;
    if(validCreation){
        if (manager->createUser(username, password)) {
            reject();
        }
    }
}

void Register::onCancelButtonClicked() {
    reject();
}



