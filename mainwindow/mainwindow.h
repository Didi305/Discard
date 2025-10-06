#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QFrame>
#include <QMainWindow>
#include <qpushbutton.h>
#include <QLabel>
#include "client.h"
#include "login.h"
#include "directmessage.h"
#include "friends.h"
#include "dragbanner.h"
#include "manager.h"
#include "ui_MainWindow.h"
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
    void updateAvatar(const QString &fileName);
    void refreshForUser();
    void resizeEvent(QResizeEvent *event);
private slots:
    void onConnectionSuccess();
    void onAvatarButtonClicked();
    void onDisconnectClicked();
    void onItemDragging(int n);
    void onItemNotDragged();

private:
    Ui::MainWindow   *ui;
    AddFriendPage    *m_addFriendPage;
    OutgoingPage     *m_outgoingPage;
    Friends          *m_FriendsPage;
    DirectMessage    *m_DirectMessagePage;
    Manager          *manager;
    Client           *client;
    QPushButton      *avatarButton;
    QLabel           *nameLabel;
    DragBanner       * m_banner = nullptr;
};
#endif // MAINWINDOW_H
