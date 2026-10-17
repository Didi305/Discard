#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include "register.h"
#include "manager.h"
QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void onLoginButtonClicked();
    void onCreateAccountButtonClicked();

private:
    bool connectToServer();
    void showError(const QString &message);
    void showMessage(const QString &message, bool success);
    Ui::Login *ui {nullptr};
    Manager *manager {nullptr};
};

#endif // LOGIN_DIALOG_H
