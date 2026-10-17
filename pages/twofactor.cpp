#include "twofactor.h"
#include "manager.h"
#include "utils/authenticatorqr.h"
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QSaveFile>
#include <QSignalBlocker>
#include <QScreen>
#include <QShowEvent>
#include <QResizeEvent>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

namespace {
// Each page gets its own natural height instead of inheriting the tallest page.
class CurrentPageStack : public QStackedWidget {
public:
    explicit CurrentPageStack(QWidget *parent) : QStackedWidget(parent) {
        layout()->setSizeConstraint(QLayout::SetNoConstraint);
    }

    QSize sizeHint() const override {
        const auto *page = currentWidget();
        if (!page) {
            return QStackedWidget::sizeHint();
        }
        QSize hint = page->sizeHint();
        if (page->hasHeightForWidth()) {
            hint.setHeight(qMax(page->minimumSizeHint().height(), page->heightForWidth(width())));
        }
        return hint;
    }

    QSize minimumSizeHint() const override {
        return sizeHint();
    }

    bool hasHeightForWidth() const override {
        // The width is fixed; sizeHint already measures the active page at that width.
        return false;
    }
};

QLabel *addText(QVBoxLayout *layout, const QString &text) {
    auto *label=new QLabel(text);
    label->setTextFormat(Qt::PlainText);
    label->setWordWrap(true);
    layout->addWidget(label);
    return label;
}

QVBoxLayout *pageLayout(QWidget *page) {
    auto *layout = new QVBoxLayout(page);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(18);
    layout->setAlignment(Qt::AlignTop);
    return layout;
}

void addHeading(QVBoxLayout *layout, const QString &heading, const QString &description) {
    addText(layout, heading)->setProperty("role", "dialogHeading");
    addText(layout, description)->setProperty("role", "hint");
}

QVBoxLayout *addSetupStep(QVBoxLayout *layout,
                         int number,
                         const QString &heading,
                         const QString &description) {
    auto *row = new QHBoxLayout;
    row->setSpacing(12);
    auto *marker = new QLabel(QString::number(number));
    marker->setObjectName("setupStepNumber");
    marker->setFixedSize(28, 28);
    marker->setAlignment(Qt::AlignCenter);
    row->addWidget(marker, 0, Qt::AlignTop);

    auto *details = new QVBoxLayout;
    details->setSpacing(8);
    addText(details, heading)->setProperty("role", "stepHeading");
    if (!description.isEmpty()) {
        addText(details, description)->setProperty("role", "hint");
    }
    row->addLayout(details, 1);
    layout->addLayout(row);
    return details;
}

struct DialogActions {
    QPushButton *primary;
    QPushButton *secondary;
};

DialogActions addActionRow(QVBoxLayout *layout,
                         const QString &primaryText,
                         const QString &secondaryText) {
    auto *row = new QHBoxLayout;
    row->setSpacing(12);
    auto *primary = new QPushButton(primaryText);
    auto *secondary = new QPushButton(secondaryText);
    primary->setProperty("role", "primary");
    secondary->setProperty("role", "secondary");
    for (auto *button : {primary, secondary}) {
        button->setMinimumHeight(44);
        button->setCursor(Qt::PointingHandCursor);
        row->addWidget(button, 1);
    }
    layout->addLayout(row);
    return {primary, secondary};
}
void styleDialog(QDialog *dialog) {
    dialog->setStyleSheet(Utils::readText(":/styles/login_register.qss", "login_register.qss")+
        "QPlainTextEdit { background: #2a2c30; color: #f0f0f2; border: 1px solid #454a52; padding: 8px; }");
    dialog->setMinimumWidth(520);
}
}

TwoFactorReminderDialog::TwoFactorReminderDialog(const QString &accountName, QWidget *parent)
    : QDialog(parent) {
    setWindowTitle(tr("Set up two-factor authentication"));
    setObjectName("twoFactorReminderDialog");
    styleDialog(this);

    auto *layout = new QVBoxLayout(this);
    layout->setSpacing(16);
    addText(layout, tr("Two-factor authentication (2FA) is turned off for %1.").arg(accountName));
    addText(layout, tr("Add an authenticator app so signing in requires your password and a code "
                       "from your phone. You can set it up now or later from the sidebar."));

    hideReminder = new QCheckBox(tr("Don't show this again for this account on this computer."), this);
    hideReminder->setObjectName("hideTwoFactorReminder");
    layout->addWidget(hideReminder);

    auto *buttons = new QDialogButtonBox(this);
    auto *setupButton = buttons->addButton(tr("Set up 2FA"), QDialogButtonBox::AcceptRole);
    auto *laterButton = buttons->addButton(tr("Not now"), QDialogButtonBox::RejectRole);
    setupButton->setDefault(true);
    laterButton->setProperty("role", "secondary");
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

bool TwoFactorReminderDialog::remindersDisabled() const {
    return hideReminder->isChecked();
}

SecondFactorDialog::SecondFactorDialog(Manager *m, QWidget *parent)
    : QDialog(parent), manager(m) {
    setWindowTitle(tr("Verify your sign-in"));
    styleDialog(this);
    auto *layout=new QVBoxLayout(this);
    addText(layout,tr("Enter the six-digit code from your authenticator app. If you cannot use your authenticator, enter one unused recovery code instead."));
    code=new QLineEdit(this);
    code->setObjectName("secondFactorCode");
    code->setPlaceholderText(tr("Authenticator code or recovery code"));
    code->setMaxLength(64);
    layout->addWidget(code);
    error=addText(layout,{});
    error->setStyleSheet("color: #ffb4b4;");
    error->hide();
    auto *buttons=new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,this);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Verify"));
    layout->addWidget(buttons);
    connect(buttons,&QDialogButtonBox::accepted,this,&SecondFactorDialog::verify);
    connect(buttons,&QDialogButtonBox::rejected,this,&SecondFactorDialog::reject);
    connect(code,&QLineEdit::returnPressed,this,&SecondFactorDialog::verify);
    code->setFocus();
}

void SecondFactorDialog::verify() {
    if(code->text().trimmed().isEmpty()) {
        error->setText(tr("Enter an authenticator code or an unused recovery code."));
        error->show();return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    const bool verified=manager->verifySecondFactor(code->text());
    QApplication::restoreOverrideCursor();
    code->clear();
    if(verified){accept();return;}
    error->setText(manager->lastError());
    error->show();
    code->setFocus();
}

void SecondFactorDialog::reject() {
    code->clear();
    manager->cancelAuthentication();
    QDialog::reject();
}

AccountSecurityDialog::AccountSecurityDialog(Manager *m, QWidget *parent, bool startSetup)
    : QDialog(parent), manager(m) {
    setWindowTitle(tr("Two-factor Authentication (2FA)"));
    setObjectName("twoFactorSettingsDialog");
    styleDialog(this);
    setFixedWidth(600);

    auto *dialogLayout = new QVBoxLayout(this);
    dialogLayout->setContentsMargins(40, 32, 40, 32);
    dialogLayout->setSpacing(16);
    dialogLayout->setSizeConstraint(QLayout::SetFixedSize);

    pages = new CurrentPageStack(this);
    pages->setFixedWidth(520);
    pages->addWidget(createStatusPage());
    pages->addWidget(createSetupPage());
    pages->addWidget(createRecoveryPage());
    pages->addWidget(createDisablePage());
    dialogLayout->addWidget(pages);

    error = addText(dialogLayout, {});
    error->setObjectName("twoFactorError");
    error->setMaximumWidth(520);
    error->hide();

    connect(pages, &QStackedWidget::currentChanged, this, [this] {
        pages->updateGeometry();
        layout()->invalidate();
        adjustSize();
    });

    refreshStatus();
    if (startSetup && !setupButton->isHidden()) {
        QTimer::singleShot(0, this, &AccountSecurityDialog::beginSetup);
    }
}

void AccountSecurityDialog::showEvent(QShowEvent *event) {
    QDialog::showEvent(event);
    scheduleCentering();
}

void AccountSecurityDialog::resizeEvent(QResizeEvent *event) {
    QDialog::resizeEvent(event);
    if (isVisible()) {
        scheduleCentering();
    }
}

void AccountSecurityDialog::scheduleCentering() {
    if (centeringQueued) {
        return;
    }
    centeringQueued = true;
    // Wait for the layout and native window frame to reach their final sizes.
    QTimer::singleShot(0, this, [this] {
        centeringQueued = false;
        if (isVisible()) {
            centerOnParent();
        }
    });
}

void AccountSecurityDialog::centerOnParent() {
    auto *owner = parentWidget() ? parentWidget()->window() : nullptr;
    const QPoint center = owner ? owner->frameGeometry().center() : frameGeometry().center();
    auto *targetScreen = QGuiApplication::screenAt(center);
    if (!targetScreen) {
        targetScreen = screen();
    }
    if (!targetScreen) {
        return;
    }

    const QRect available = targetScreen->availableGeometry();
    QRect frame = frameGeometry();
    frame.moveCenter(owner ? center : available.center());
    const int rightmost = qMax(available.left(), available.right() - frame.width() + 1);
    const int lowest = qMax(available.top(), available.bottom() - frame.height() + 1);
    frame.moveTopLeft(QPoint(qBound(available.left(), frame.left(), rightmost),
                            qBound(available.top(), frame.top(), lowest)));
    move(frame.topLeft());
}

QWidget *AccountSecurityDialog::createStatusPage() {
    auto *page = new QWidget(pages);
    auto *content = pageLayout(page);
    addHeading(content, tr("Two-factor Authentication"),
               tr("Use your password and a code from your phone to protect your account."));

    status = addText(content, {});
    status->setObjectName("twoFactorAccountStatus");
    setupButton = new QPushButton(tr("Set up two-factor authentication"), page);
    setupButton->setProperty("role", "primary");
    disableButton = new QPushButton(tr("Disable two-factor authentication"), page);
    disableButton->setProperty("role", "secondary");
    auto *close = new QPushButton(tr("Close"), page);
    close->setProperty("role", "secondary");
    for (auto *button : {setupButton, disableButton, close}) {
        button->setMinimumHeight(44);
        button->setCursor(Qt::PointingHandCursor);
        content->addWidget(button);
    }
    connect(setupButton, &QPushButton::clicked, this, &AccountSecurityDialog::beginSetup);
    connect(disableButton, &QPushButton::clicked, this, [this] {
        error->hide();
        disablePassword->clear();
        disableCode->clear();
        pages->setCurrentIndex(3);
        disablePassword->setFocus();
    });
    connect(close, &QPushButton::clicked, this, &AccountSecurityDialog::reject);
    return page;
}

QWidget *AccountSecurityDialog::createSetupPage() {
    auto *page = new QWidget(pages);
    auto *content = pageLayout(page);
    addHeading(content, tr("Set up your authenticator"),
               tr("Scan with your phone, then enter a code to finish."));

    auto *accountStep = addSetupStep(content, 1, tr("Add your account"), {});
    setupInstructions = addText(accountStep,
        tr("In your authenticator app, choose Add account, then Scan a QR code."));
    setupInstructions->setProperty("role", "hint");

    setupQr = new QLabel(page);
    setupQr->setObjectName("authenticatorQr");
    setupQr->setAlignment(Qt::AlignCenter);
    setupQr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setupQr->setAccessibleName(tr("Scan this QR code with your authenticator app"));
    accountStep->addWidget(setupQr, 0, Qt::AlignHCenter);

    manualKeyToggle = new QPushButton(tr("Can't scan? Enter key manually"), page);
    manualKeyToggle->setObjectName("manualKeyToggle");
    manualKeyToggle->setCheckable(true);
    manualKeyToggle->setAutoDefault(false);
    manualKeyToggle->setCursor(Qt::PointingHandCursor);
    accountStep->addWidget(manualKeyToggle);

    manualKeyPanel = new QWidget(page);
    auto *manualDetails = pageLayout(manualKeyPanel);
    manualDetails->setSpacing(10);
    addText(manualDetails,
            tr("Account: Discard (%1)\nType: Time based (TOTP)")
                .arg(manager->getClient()->getClientName()))->setProperty("role", "hint");
    auto *keyRow = new QHBoxLayout;
    keyRow->setSpacing(8);
    setupKey = new QLineEdit(manualKeyPanel);
    setupKey->setObjectName("totpSetupKey");
    setupKey->setReadOnly(true);
    setupKey->setFocusPolicy(Qt::ClickFocus);
    setupKey->setMinimumHeight(44);
    setupKey->setAccessibleName(tr("Private authenticator setup key"));
    keyRow->addWidget(setupKey, 1);
    auto *copyKey = new QPushButton(tr("Copy"), manualKeyPanel);
    copyKey->setProperty("role", "secondary");
    copyKey->setAutoDefault(false);
    copyKey->setMinimumHeight(44);
    copyKey->setCursor(Qt::PointingHandCursor);
    keyRow->addWidget(copyKey);
    manualDetails->addLayout(keyRow);
    addText(manualDetails, tr("Keep this setup key private."))->setProperty("role", "hint");
    accountStep->addWidget(manualKeyPanel);
    manualKeyPanel->hide();
    connect(copyKey, &QPushButton::clicked, this, [this] {
        QApplication::clipboard()->setText(setupKey->text());
    });
    connect(manualKeyToggle, &QPushButton::toggled, this, [this](bool manualEntry) {
        manualKeyPanel->setVisible(manualEntry);
        setupQr->setVisible(!manualEntry && !setupQr->pixmap().isNull());
        manualKeyToggle->setText(manualEntry
            ? tr("Scan a QR code instead") : tr("Can't scan? Enter key manually"));
        setupInstructions->setText(manualEntry
            ? tr("Choose Enter a setup key in your app, then use the details below.")
            : tr("In your authenticator app, choose Add account, then Scan a QR code."));
        pages->updateGeometry();
        layout()->invalidate();
        adjustSize();
    });

    auto *codeStep = addSetupStep(content, 2, tr("Verify your authenticator"),
        tr("Enter the current six-digit code shown in your app."));
    setupCode = new QLineEdit(page);
    setupCode->setObjectName("totpSetupCode");
    setupCode->setPlaceholderText(tr("6-digit code"));
    setupCode->setAccessibleName(tr("Six-digit authenticator code"));
    setupCode->setAlignment(Qt::AlignCenter);
    setupCode->setMinimumHeight(48);
    setupCode->setMaximumWidth(200);
    setupCode->setMaxLength(6);
    setupCode->setInputMethodHints(Qt::ImhDigitsOnly);
    setupCode->setValidator(new QRegularExpressionValidator(
        QRegularExpression("[0-9]{0,6}"), setupCode));
    codeStep->addWidget(setupCode, 0, Qt::AlignLeft);
    QWidget::setTabOrder(manualKeyToggle, copyKey);
    QWidget::setTabOrder(copyKey, setupCode);

    const auto actions = addActionRow(content, tr("Enable 2FA"), tr("Cancel setup"));
    connect(actions.primary, &QPushButton::clicked, this, &AccountSecurityDialog::confirmSetup);
    connect(actions.secondary, &QPushButton::clicked, this, &AccountSecurityDialog::cancelSetup);
    connect(setupCode, &QLineEdit::returnPressed, this, &AccountSecurityDialog::confirmSetup);
    return page;
}

QWidget *AccountSecurityDialog::createRecoveryPage() {
    auto *page = new QWidget(pages);
    auto *content = pageLayout(page);
    addHeading(content, tr("Save your recovery codes"),
        tr("Two-factor authentication is now enabled. Each recovery code can replace "
           "your phone code once. Save them somewhere private; this list is shown only now."));

    recoveryDisplay = new QPlainTextEdit(page);
    recoveryDisplay->setObjectName("recoveryCodes");
    recoveryDisplay->setReadOnly(true);
    recoveryDisplay->setFixedHeight(220);
    recoveryDisplay->setLineWrapMode(QPlainTextEdit::NoWrap);
    content->addWidget(recoveryDisplay);
    const auto saveActions = addActionRow(content, tr("Save codes to a file"), tr("Copy codes"));
    saveActions.primary->setProperty("role", "secondary");
    connect(saveActions.primary, &QPushButton::clicked,
            this, &AccountSecurityDialog::saveRecoveryCodes);
    connect(saveActions.secondary, &QPushButton::clicked, this, [this] {
        QApplication::clipboard()->setText(recoveryCodes.join('\n'));
    });

    codesSaved = new QCheckBox(tr("I have saved or written down my recovery codes."), page);
    content->addWidget(codesSaved);
    auto *done = new QPushButton(tr("Done"), page);
    done->setProperty("role", "primary");
    done->setMinimumHeight(44);
    done->setEnabled(false);
    content->addWidget(done);
    connect(codesSaved, &QCheckBox::toggled, done, &QPushButton::setEnabled);
    connect(done, &QPushButton::clicked, this, [this] {
        recoveryDisplay->clear();
        recoveryCodes.clear();
        accept();
    });
    return page;
}

QWidget *AccountSecurityDialog::createDisablePage() {
    auto *page = new QWidget(pages);
    auto *content = pageLayout(page);
    addHeading(content, tr("Disable two-factor authentication"),
        tr("Your password will be enough to sign in again. Confirm with your current "
           "password and a fresh authenticator code or an unused recovery code."));

    addText(content, tr("Current password"))->setProperty("role", "stepHeading");
    disablePassword = new QLineEdit(page);
    disablePassword->setEchoMode(QLineEdit::Password);
    disablePassword->setPlaceholderText(tr("Enter your current password"));
    disablePassword->setAccessibleName(tr("Current password"));
    disablePassword->setMinimumHeight(48);
    content->addWidget(disablePassword);

    addText(content, tr("Verification code"))->setProperty("role", "stepHeading");
    disableCode = new QLineEdit(page);
    disableCode->setMaxLength(64);
    disableCode->setPlaceholderText(tr("Authenticator code or unused recovery code"));
    disableCode->setAccessibleName(tr("Authenticator code or recovery code"));
    disableCode->setMinimumHeight(48);
    content->addWidget(disableCode);

    const auto actions = addActionRow(content, tr("Disable 2FA"), tr("Cancel"));
    actions.primary->setProperty("role", "danger");
    connect(actions.primary, &QPushButton::clicked, this, &AccountSecurityDialog::disableMfa);
    connect(actions.secondary, &QPushButton::clicked, this, [this] {
        disablePassword->clear();
        disableCode->clear();
        error->hide();
        refreshStatus();
    });
    return page;
}

void AccountSecurityDialog::showError() {
    error->setText(manager->lastError());
    error->show();
}

void AccountSecurityDialog::refreshStatus() {
    pages->setCurrentIndex(0);
    const auto reply=manager->securityRequest("mfa_status");
    if(!reply["ok"].toBool()) {
        status->setText(tr("Could not load your account security settings."));
        setupButton->hide();disableButton->hide();showError();return;
    }
    const auto result=reply["result"].toObject();
    const bool enabled=result["enabled"].toBool();
    status->setText(enabled?tr("Two-factor authentication: Enabled\nUnused recovery codes: %1").arg(result["recovery_codes_remaining"].toInt())
                           :tr("Two-factor authentication: Not enabled"));
    setupButton->setVisible(!enabled);
    disableButton->setVisible(enabled);
}

void AccountSecurityDialog::beginSetup() {
    bool ok=false;
    QString password=QInputDialog::getText(this,tr("Confirm your password"),
        tr("Enter your current password to start setting up two-factor authentication:"),QLineEdit::Password,{},&ok);
    if(!ok)return;
    if(password.isEmpty()){error->setText(tr("Enter your current password."));error->show();return;}
    const auto reply=manager->securityRequest("mfa_setup_begin",{{"password",password}});
    password.clear();
    if(!reply["ok"].toBool()){showError();return;}
    const auto result=reply["result"].toObject();
    if(result["secret"].toString().isEmpty()){
        manager->securityRequest("mfa_setup_cancel");
        error->setText(tr("The server returned no setup key. Try setup again."));error->show();return;
    }
    setupPending=true;
    error->hide();setupCode->clear();
    clearSetupDisplay();
    setupKey->setText(result["secret"].toString());
    const auto qr = AuthenticatorQr::render(result["uri"].toString(), devicePixelRatioF());
    if (!qr.isNull()) {
        setupQr->setPixmap(qr);
        const auto displaySize = qr.deviceIndependentSize();
        setupQr->setFixedSize(qCeil(displaySize.width()), qCeil(displaySize.height()));
        setupQr->show();
    } else {
        manualKeyToggle->setChecked(true);
        manualKeyToggle->setEnabled(false);
        manualKeyToggle->setText(tr("QR unavailable. Enter the key manually."));
    }
    pages->setCurrentIndex(1);
    setupCode->setFocus();
}

void AccountSecurityDialog::clearSetupDisplay() {
    setupKey->clear();
    setupQr->clear();
    setupQr->hide();
    manualKeyPanel->hide();
    const QSignalBlocker blockToggle(manualKeyToggle);
    manualKeyToggle->setChecked(false);
    manualKeyToggle->setEnabled(true);
    manualKeyToggle->setText(tr("Can't scan? Enter key manually"));
    setupInstructions->setText(
        tr("In your authenticator app, choose Add account, then Scan a QR code."));
}

void AccountSecurityDialog::confirmSetup() {
    if(setupCode->text().size()!=6){error->setText(tr("Enter the six-digit code from your authenticator app."));error->show();return;}
    const auto reply=manager->securityRequest("mfa_setup_confirm",{{"code",setupCode->text()}});
    setupCode->clear();
    if(!reply["ok"].toBool()){showError();return;}
    setupPending=false;
    clearSetupDisplay();error->hide();
    for(const auto &value:reply["result"].toObject()["recovery_codes"].toArray())recoveryCodes.append(value.toString());
    recoveryDisplay->setPlainText(recoveryCodes.join('\n'));
    codesSaved->setChecked(false);
    pages->setCurrentIndex(2);
}

void AccountSecurityDialog::cancelSetup() {
    if(setupPending)manager->securityRequest("mfa_setup_cancel");
    setupPending=false;clearSetupDisplay();setupCode->clear();error->hide();
    refreshStatus();
}

void AccountSecurityDialog::disableMfa() {
    if(disablePassword->text().isEmpty()||disableCode->text().trimmed().isEmpty()){
        error->setText(tr("Enter your current password and an authenticator or recovery code."));error->show();return;
    }
    const auto reply=manager->securityRequest("mfa_disable",{{"password",disablePassword->text()},{"code",disableCode->text().trimmed()}});
    disableCode->clear();
    if(!reply["ok"].toBool()){showError();return;}
    disablePassword->clear();error->hide();
    refreshStatus();
}

void AccountSecurityDialog::saveRecoveryCodes() {
    const QString path=QFileDialog::getSaveFileName(this,tr("Save recovery codes somewhere private"),{},tr("Text files (*.txt)"));
    if(path.isEmpty())return;
    QSaveFile file(path);
    const QByteArray bytes=(tr("Discard recovery codes\nKeep these private. Each code can be used once.\n\n")+recoveryCodes.join('\n')+'\n').toUtf8();
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::warning(this,tr("Codes not saved"),file.errorString());return;
    }
    if(!file.setPermissions(QFileDevice::ReadOwner|QFileDevice::WriteOwner)){
        QMessageBox::warning(this,tr("Codes not saved"),tr("Could not set private file permissions. Choose another location."));return;
    }
    if(file.write(bytes)!=bytes.size()||!file.commit()){
        QMessageBox::warning(this,tr("Codes not saved"),file.errorString());return;
    }
    codesSaved->setChecked(true);
}

void AccountSecurityDialog::reject() {
    if(pages->currentIndex()==2&&!codesSaved->isChecked()){
        error->setText(tr("Save or write down your recovery codes, then confirm the checkbox before closing. Two-factor authentication is already enabled."));
        error->show();return;
    }
    if(setupPending)manager->securityRequest("mfa_setup_cancel");
    clearSetupDisplay();setupCode->clear();disablePassword->clear();disableCode->clear();
    recoveryDisplay->clear();recoveryCodes.clear();
    QDialog::reject();
}
