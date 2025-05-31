#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void setClient(Client *client);

private slots:
    void onSendMessageButtonclicked();
    void onConnectionSuccess();
    void onMessageReceived(QString &str);
private:
    Ui::MainWindow *ui;
    Client *client;
};
#endif // MAINWINDOW_H
