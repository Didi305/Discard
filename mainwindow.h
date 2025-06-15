#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QFrame>
#include <QMainWindow>
#include <qpushbutton.h>
#include <QLabel>
#include "client.h"
#include "manager.h"
#include "outgoingpage.h"
#include "addfriendpage.h"
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
    void updateUserPanelPosition();
    void resizeEvent(QResizeEvent *event);
    void updateAvatar(const QString &fileName);
private slots:
    void onConnectionSuccess();
    void onAvatarButtonClicked();
private:
    Ui::MainWindow *ui;
    AddFriendPage*    m_addFriendPage;
    OutgoingPage*   m_outgoingPage;
    Manager *manager;
    Client *client;
    QFrame         *userPanel;
    QPushButton *avatarButton;
    QLabel *nameLabel;
};
#endif // MAINWINDOW_H
