#include "login.h"
#include "ui_login.h"
#include "register.h"
#include "manager.h"
#include "twofactor.h"
#include <QApplication>
#include <QStyle>
#include <QPalette>

Login::Login(QWidget *parent) : QDialog(parent), ui(new Ui::Login), manager(Manager::get()) {
    ui->setupUi(this);
    setStyleSheet(Utils::readText(":/styles/login_register.qss", "login_register.qss"));
    ui->createAccountButton->setProperty("role", "secondary");
    for (auto *field : {ui->loginUsername, ui->loginPassword}) {
        const auto icon = field->echoMode() == QLineEdit::Password
            ? QStringLiteral(":/icons/input-lock.svg") : QStringLiteral(":/icons/input-user.svg");
        field->addAction(QIcon(icon), QLineEdit::LeadingPosition);
        auto colors = field->palette();
        colors.setColor(QPalette::PlaceholderText, QColor("#a9b6cf"));
        field->setPalette(colors);
    }
    ui->loginUsername->setFocus();
    connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginButtonClicked);
    connect(ui->createAccountButton, &QPushButton::clicked, this, &Login::onCreateAccountButtonClicked);
    connect(ui->loginUsername, &QLineEdit::textEdited, ui->loginErrorMessage, &QLabel::clear);
    connect(ui->loginPassword, &QLineEdit::textEdited, ui->loginErrorMessage, &QLabel::clear);
}

Login::~Login() {
    delete ui;
}

void Login::showError(const QString &message) {
    showMessage(message, false);
}

void Login::showMessage(const QString &message, bool success) {
    ui->loginErrorMessage->setText(message);
    ui->loginErrorMessage->setProperty("success", success);
    ui->loginErrorMessage->style()->unpolish(ui->loginErrorMessage);
    ui->loginErrorMessage->style()->polish(ui->loginErrorMessage);
    ui->loginErrorMessage->update();
}

bool Login::connectToServer() {
    ui->loginErrorMessage->clear();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    const bool connected = manager->connectServer();
    QApplication::restoreOverrideCursor();
    if (!connected) {
        showError(tr("Cannot connect to the server.\nCheck the connection and try again."));
    }
    return connected;
}

void Login::onLoginButtonClicked() {
    const QString username = ui->loginUsername->text();
    const QString password = ui->loginPassword->text();
    if (username.isEmpty() || password.isEmpty()) {
        showError(tr("Enter your username and password."));
        return;
    }
    if (!manager->checkUsernameLength(username)) {
        showError(manager->lastError());
        return;
    }
    if (!connectToServer()) {
        return;
    }
    if (!manager->authenticate(username, password)) {
        if (manager->secondFactorRequired()) {
            // No main window or user data is exposed while this prompt is pending.
            ui->loginPassword->clear();
            SecondFactorDialog secondFactor(manager, this);
            if (secondFactor.exec() == QDialog::Accepted) {
                accept();
            }
            return;
        }
        showError(manager->lastError());
        return;
    }
    accept();
}

void Login::onCreateAccountButtonClicked() {
    Register registration(this);
    if (registration.exec() == QDialog::Accepted) {
        ui->loginUsername->setText(registration.registeredUsername());
        ui->loginPassword->clear();
        ui->loginPassword->setFocus();
        showMessage(tr("Account created. Enter your password to sign in."), true);
    }
}
