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

private slots:
    void onConnectionSuccess();
    void onAvatarButtonClicked();
    void onAddFriendButtonClicked();
private:
    Ui::MainWindow *ui;
    Manager *manager;
    Client *client;
    QFrame         *userPanel;
    void updateUserPanelPosition();
    void resizeEvent(QResizeEvent *event);
    QPushButton *avatarButton;
    QLabel *nameLabel;
    void updateAvatar(const QString &fileName);
};
#endif // MAINWINDOW_H
