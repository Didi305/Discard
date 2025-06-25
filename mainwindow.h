#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QFrame>
#include <QMainWindow>
#include <qpushbutton.h>
#include <QLabel>
#include "client.h"
#include "friends.h"
#include "manager.h"
#include "helpers.h"
#include "ui_MainWindow.h"
#include "login.h"
#include "outgoingpage.h"
#include "addfriendpage.h"
#include <QFileDialog>
#include <QWidget>
#include <QPainterPath>
#include <QFrame>
#include <QMessageBox>
#include <QStandardPaths>
#include <QStyle>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
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
    void refreshForUser();
private slots:
    void onConnectionSuccess();
    void onAvatarButtonClicked();
    void onDisconnectClicked();
private:
    Ui::MainWindow   *ui;
    AddFriendPage    *m_addFriendPage;
    OutgoingPage     *m_outgoingPage;
    Friends          *m_FriendsPage;
    Manager          *manager;
    Client           *client;
    QFrame           *userPanel;
    QPushButton      *avatarButton;
    QLabel           *nameLabel;
};
#endif // MAINWINDOW_H
