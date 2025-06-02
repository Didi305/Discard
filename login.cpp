#include "login.h"
#include "ui_login.h"
#include "register.h"
#include "manager.h"
#include "client.h"
#include "mainwindow.h"
Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login), client(nullptr), manager(Manager::get()) {
    ui->setupUi(this);
    ui->loginErrorMessage->hide();
    connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
    connect(ui->createAccountButton, &QPushButton::clicked, this, &Login::onCreateAccountButtonClicked);
}

Login::~Login(){
    delete(ui);
}

void Login::setClient(Client *client){
    this->client = client;

}

void Login::onLoginButtonClicked(){
    QString username = ui->loginUsername->text();
    QString password = ui->loginPassword->text();
    if((manager->checkUser(username, password)).isEmpty()){
        ui->loginErrorMessage->show();
    }
    else{
        QString data = manager->checkUser(username, password);
        QStringList parts = data.split(";");
        client->setClientName(parts.value(0));
        client->setClientAvatar(parts.value(1));
        QString host = "127.0.0.1";
        int port = 1234;
        client->connectToServer(host, port);
        accept();
    }
}

void Login::onCreateAccountButtonClicked(){
    reg = new Register(this);
    reg->show();
}
