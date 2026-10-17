#ifndef REGISTER_DIALOG_H
#define REGISTER_DIALOG_H

#include <QDialog>
#include "manager.h"
class QLineEdit;
QT_BEGIN_NAMESPACE
namespace Ui { class Register; }
QT_END_NAMESPACE

class Register : public QDialog
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    ~Register();
    QString registeredUsername() const;
private slots:
    void onRegisterButtonClicked();

private:
    void showError(const QString &message, QLineEdit *field = nullptr);
    void clearError();
    Ui::Register *ui        {nullptr};
    Manager *manager {nullptr};
};

#endif // REGISTER_DIALOG_H
