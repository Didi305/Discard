#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H

#include <QDialog>
#include "manager.h"// if you need it for immediate sign-in
QT_BEGIN_NAMESPACE
namespace Ui { class Register; }
QT_END_NAMESPACE

class Register : public QDialog
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();
private slots:
    void onUsernameEditingFinished();
    void onNicknameEditingFinished();
    void onRegisterButtonClicked();
    void onCancelButtonClicked();

private:
    Ui::Register *ui        {nullptr};
    Manager *manager {nullptr};
};

#endif // REGISTER_DIALOG_H
