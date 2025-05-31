#include "register.h"
#include "ui_register.h"
#include "client.h"
#include "manager.h"
Register::Register(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Register)
    , client_(nullptr)
    , manager(Manager::get())
{
    ui->setupUi(this);
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
void Register::onRegisterButtonClicked() {
    QString username = ui->registerUsername->text();
    QString password = ui->registerPassword->text();
    if (!(manager->createUser(username, password))) {
        qDebug() << "chi haja machi lhih";
    }
    else {
        reject();
    }
}

void Register::onCancelButtonClicked() {
    reject();
}
