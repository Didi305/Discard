#include "login.h"
#include "ui_login.h"
#include "register.h"
#include "manager.h"
Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login), client_(nullptr), manager(Manager::get()) {
    ui->setupUi(this);
    ui->loginErrorMessage->hide();
    connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
    connect(ui->createAccountButton, &QPushButton::clicked, this, &Login::onCreateAccountButtonClicked);
}

Login::~Login(){
    delete(ui);
}

void Login::setClient(Client *client){
    this->client_ = client;
}


void Login::onLoginButtonClicked(){
    QString username = ui->loginUsername->text();
    QString password = ui->loginPassword->text();
    if(manager->checkUser(username, password)){
        accept();
    }
    else{
        ui->loginErrorMessage->show();
    }
}

void Login::onCreateAccountButtonClicked(){
    reg = new Register(this);
    reg->show();
}
