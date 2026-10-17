#include "register.h"
#include "ui_register.h"
#include <QLineEdit>
#include <QStyle>
#include <QPalette>

Register::Register(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::Register),
      manager(Manager::get()) {
    ui->setupUi(this);
    setStyleSheet(Utils::readText(":/styles/login_register.qss", "login_register.qss"));
    ui->cancelButton->setProperty("role", "secondary");
    ui->registerUsername->setFocus();

    connect(ui->registerButton, &QPushButton::clicked, this, &Register::onRegisterButtonClicked);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    for (auto *field : {ui->registerUsername, ui->registerNickname,
                        ui->registerPassword, ui->registerPasswordConfirmation}) {
        const auto icon = field->echoMode() == QLineEdit::Password
            ? QStringLiteral(":/icons/input-lock.svg") : QStringLiteral(":/icons/input-user.svg");
        field->addAction(QIcon(icon), QLineEdit::LeadingPosition);
        auto colors = field->palette();
        colors.setColor(QPalette::PlaceholderText, QColor("#a9b6cf"));
        field->setPalette(colors);
        connect(field, &QLineEdit::textEdited, this, &Register::clearError);
    }
}

Register::~Register() {
    delete ui;
}

QString Register::registeredUsername() const {
    return ui->registerUsername->text().trimmed();
}

void Register::clearError() {
    ui->registerErrorMessage->clear();
    for (auto *field : {ui->registerUsername, ui->registerNickname,
                        ui->registerPassword, ui->registerPasswordConfirmation}) {
        if (field->property("invalid").toBool()) {
            field->setProperty("invalid", false);
            field->style()->unpolish(field);
            field->style()->polish(field);
            field->update();
        }
    }
}

void Register::showError(const QString &message, QLineEdit *field) {
    ui->registerErrorMessage->setText(message);
    if (field) {
        field->setProperty("invalid", true);
        field->style()->unpolish(field);
        field->style()->polish(field);
        field->update();
        field->setFocus();
    }
}

void Register::onRegisterButtonClicked() {
    clearError();
    const QString username = ui->registerUsername->text();
    const QString nickname = ui->registerNickname->text().trimmed();
    const QString password = ui->registerPassword->text();

    if (username.trimmed().isEmpty()) {
        showError(tr("Enter a username."), ui->registerUsername);
        return;
    }
    if (!manager->checkUsernameLength(username)) {
        showError(manager->lastError(), ui->registerUsername);
        return;
    }
    if (nickname.isEmpty()) {
        showError(tr("Enter a nickname."), ui->registerNickname);
        return;
    }
    if (password.isEmpty()) {
        showError(tr("Enter a password."), ui->registerPassword);
        return;
    }
    if (password != ui->registerPasswordConfirmation->text()) {
        showError(tr("The passwords do not match."), ui->registerPasswordConfirmation);
        return;
    }

    // The registration request checks availability atomically on the server.
    // Moving between fields stays local and never waits for a network reply.
    if (!manager->createUser(username, password, nickname)) {
        showError(manager->lastError());
        return;
    }
    accept();
}
