#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QFrame>
#include <QMainWindow>
#include <qpushbutton.h>
#include <QLabel>
#include "client.h"
#include "manager.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;
    void setClient(Client *client);

private slots:
    void onConnectionSuccess();
    void onAvatarButtonClicked();
private:
    Ui::MainWindow *ui;
    Client *client;
    Manager *manager;
    QFrame         *userPanel;
    void updateUserPanelPosition();
    void resizeEvent(QResizeEvent *event);
    QPushButton *avatarButton;
    QLabel *nameLabel;
    void updateAvatar(const QString &fileName);
};
#endif // MAINWINDOW_H
