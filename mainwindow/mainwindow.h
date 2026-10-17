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
#include "conversationrow.h"
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
    void resizeEvent(QResizeEvent *event) override;
    void loadConversations();
private slots:
    void onConnectionSuccess();
    void onAvatarButtonClicked();
    void onAccountSecurityClicked();
    void onDisconnectClicked();
    void onItemDragging(int n);
    void onItemNotDragged();
    void updateConversations(Message& lastMessage, Data& partner);
private:
    enum class TwoFactorState { Unknown, Disabled, Enabled };
    void updateTwoFactorIndicator(TwoFactorState state);
    TwoFactorState refreshTwoFactorStatus();
    void scheduleTwoFactorReminder();
    void showTwoFactorReminder(int userId);
    void openTwoFactorSettings(bool startSetup = false);

    Ui::MainWindow   *ui;
    AddFriendPage    *m_addFriendPage;
    OutgoingPage     *m_outgoingPage;
    Friends          *m_FriendsPage;
    DirectMessage    *m_DirectMessagePage;
    Manager          *manager;
    Client           *client;
    QPushButton      *avatarButton;
    QPushButton      *twoFactorButton;
    QLabel           *twoFactorStatusLabel;
    QLabel           *nameLabel;
    DragBanner       * m_banner = nullptr;
    bool twoFactorDialogOpen = false;
    quint64 reminderGeneration = 0;
};
#endif // MAINWINDOW_H
