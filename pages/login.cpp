#include "login.h"
#include "ui_login.h"
#include "register.h"
#include "manager.h"
#include <QApplication>

Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login), manager(Manager::get()) {
    ui->setupUi(this);
    setStyleSheet(Utils::readText(":/styles/login_register.qss", "login_register.qss"));
    ui->loginErrorMessage->setEnabled(true);
    ui->loginErrorMessage->setTextFormat(Qt::PlainText);
    ui->loginErrorMessage->setWordWrap(true);
    ui->loginErrorMessage->setAlignment(Qt::AlignCenter);
    ui->loginErrorMessage->setGeometry(60, 300, width() - 120, 48);
    ui->loginErrorMessage->setStyleSheet(QStringLiteral("color: #ffb4b4; font-size: 14px;"));
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


void Login::showError(const QString &message) {
    ui->loginErrorMessage->setText(message);
    ui->loginErrorMessage->show();
}

bool Login::connectToDatabase() {
    ui->loginErrorMessage->hide();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    const bool connected = manager->connectServer();
    QApplication::restoreOverrideCursor();
    if (!connected) {
        showError(tr("Cannot connect to the server.\nCheck the connection and try again."));
    }
    return connected;
}

void Login::onLoginButtonClicked(){
    auto *client = manager->getClient();
    QString username = ui->loginUsername->text();
    QString password = ui->loginPassword->text();
    if (username.isEmpty() || password.isEmpty()) {
        showError(tr("Enter your username and password."));
        return;
    }
    if (!connectToDatabase()) return;
    if (!manager->authenticate(username, password)) {
        showError(manager->lastError());
        return;
    }
    accept();
}

void Login::onCreateAccountButtonClicked(){
    if (!connectToDatabase()) return;
    reg = new Register(this);
    reg->show();
}
