#include "login.h"
#include "ui_login.h"
#include "register.h"
#include "manager.h"

Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login), manager(Manager::get()) {
    ui->setupUi(this);
    setStyleSheet(Utils::readText(":/styles/login_register.qss", "login_register.qss"));
    ui->loginErrorMessage->hide();
    ui->createAccountButton->setProperty("role", "secondary");
    QFontMetrics fm(ui->loginButton->font());
    ui->loginButton->setMinimumHeight(fm.height() + 20);
    connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
    connect(ui->createAccountButton, &QPushButton::clicked, this, &Login::onCreateAccountButtonClicked);
}

Login::~Login(){
    delete(ui);
}


void Login::onLoginButtonClicked(){
    auto *client = manager->getClient();
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
        client->setClientID(parts.value(2).toInt());
        client->setClientStatus(Utils::statusFromString(parts.value(3)));
        client->setFriends(manager->fetchFriends(client->getClientID(), client->getFriends()));
        Data userData = {.user_id = client->getClientID(), .displayName = client->getClientName(), .avatar = client->getClientAvatar(), .friends = client->getFriends()};
        client->setClientData(userData);
        QString host = "discard.freedynamicdns.net";
        int port = 41234;
        client->connectToServer(host, port);

        accept();
    }
}

void Login::onCreateAccountButtonClicked(){
    reg = new Register(this);
    reg->show();
}
