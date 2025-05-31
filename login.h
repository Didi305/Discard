#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include "client.h"              // forward declaration of your network class
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

    void setClient(Client *client);          // optional helper

private slots:
    void onLoginButtonClicked();             // called when user presses “Login”
    void onCreateAccountButtonClicked();            // closes dialog

private:
    Ui::Login *ui           {nullptr};       // generated from login.ui
    Client    *client      {nullptr};
    Register  *reg {nullptr};
    Manager *manager {nullptr};
};

#endif // LOGIN_DIALOG_H
