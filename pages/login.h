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
    void onLoginButtonClicked();             // called when user presses “Login”
    void onCreateAccountButtonClicked();            // closes dialog

private:
    Ui::Login *ui           {nullptr};       // generated from login.ui
    Register  *reg {nullptr};
    Manager *manager {nullptr};
};

#endif // LOGIN_DIALOG_H
