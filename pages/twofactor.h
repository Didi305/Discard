#pragma once

#include <QDialog>
#include <QStringList>

class Manager;
class QLabel;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QPlainTextEdit;
class QCheckBox;

// Password verification has succeeded, but the socket is not authenticated yet.
class SecondFactorDialog : public QDialog {
public:
    explicit SecondFactorDialog(Manager *manager, QWidget *parent=nullptr);
protected:
    void reject() override;
private:
    Manager *manager;
    QLineEdit *code;
    QLabel *error;
    void verify();
};

class AccountSecurityDialog : public QDialog {
public:
    explicit AccountSecurityDialog(Manager *manager,
                                   QWidget *parent = nullptr,
                                   bool startSetup = false);
protected:
    void reject() override;
private:
    Manager *manager;
    QStackedWidget *pages;
    QLabel *status;
    QLabel *error;
    QPushButton *setupButton;
    QPushButton *disableButton;
    QLabel *setupQr;
    QLabel *setupInstructions;
    QPushButton *manualKeyToggle;
    QWidget *manualKeyPanel;
    QLineEdit *setupKey;
    QLineEdit *setupCode;
    QLineEdit *disablePassword;
    QLineEdit *disableCode;
    QPlainTextEdit *recoveryDisplay;
    QCheckBox *codesSaved;
    QStringList recoveryCodes;
    bool setupPending=false;
    QWidget *createStatusPage();
    QWidget *createSetupPage();
    QWidget *createRecoveryPage();
    QWidget *createDisablePage();
    void refreshStatus();
    void beginSetup();
    void clearSetupDisplay();
    void confirmSetup();
    void cancelSetup();
    void disableMfa();
    void saveRecoveryCodes();
    void showError();
};
