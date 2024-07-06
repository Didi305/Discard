#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "client.h"
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    void setClient(Client *client);
    ~Dialog();
private slots:
    void on_cancelButton_clicked();
    void on_okButton_clicked();

private:
    Ui::Dialog *ui;
    Client *client;
};

#endif // DIALOG_H
