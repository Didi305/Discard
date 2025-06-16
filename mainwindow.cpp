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
#include "row.h"
#include "ui_MainWindow.h"
#include "MainWindow.h"
#include "helpers.h"
#include "manager.h"
#include "login.h"
#include "addfriendpage.h"
#include "outgoingpage.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , userPanel(new QFrame)
    , avatarButton(new QPushButton(userPanel))
    , nameLabel(new QLabel(userPanel))
    , manager(Manager::get())
    , client(manager->getClient())
    , m_addFriendPage(new AddFriendPage(this))
    , m_outgoingPage(new OutgoingPage(this))
{
    ui->setupUi(this);
    ui->ADD_OUTGOING->setAttribute(Qt::WA_StyledBackground, true);
    ui->ADD_OUTGOING->addWidget(m_addFriendPage);
    ui->ADD_OUTGOING->addWidget(m_outgoingPage); // index 1

    m_addFriendPage->load(client->getClientID());
    m_outgoingPage->setRequests(client->getClientID());

    userPanel->setParent(ui->mainContainer);
    userPanel->setObjectName("userPanel");
    userPanel->setFixedHeight(60);          // 50 px tall
    userPanel->setFixedWidth(335);

    // Give it a horizontal layout to host avatarButton + username
    auto *lay = new QHBoxLayout(userPanel);
    lay->setContentsMargins(8, 4, 8, 4);
    lay->setSpacing(6);

    // avatarButton
    auto* disconnectButton = new QPushButton(userPanel);
    disconnectButton->setFixedSize(35, 35);
    disconnectButton->setObjectName("disconnectButton");
    disconnectButton->setIcon(
        this->style()->standardIcon(QStyle::SP_TitleBarCloseButton)
        );
    disconnectButton->setCursor(Qt::PointingHandCursor);
    avatarButton->setFixedSize(48, 48);
    avatarButton->setIconSize({48, 48});
    avatarButton->setFlat(true);
    avatarButton->setCursor(Qt::PointingHandCursor);
    avatarButton->setStyleSheet("background-color: transparent");        // no border


    QPixmap placeholder(48, 48);
    placeholder.fill(Qt::transparent);
    {
        QPainter p(&placeholder);
        p.setRenderHint(QPainter::Antialiasing);
        p.setBrush(Qt::gray);
        p.setPen(Qt::NoPen);
        p.drawEllipse(0, 0, 48, 48);
    }
    avatarButton->setIcon(QIcon(placeholder));

    // Username QLabel
    QFont f = nameLabel->font();
    f.setPointSize(11);
    f.setBold(true);
    nameLabel->setFont(f);
    nameLabel->setStyleSheet("color:white;");

    lay->addWidget(avatarButton);
    lay->addWidget(nameLabel);
    lay->addWidget(disconnectButton);
    nameLabel->setText(client->getClientName());
    updateAvatar(client->getClientAvatar());

    userPanel->show();
    connect(avatarButton, &QPushButton::clicked, this, &MainWindow::onAvatarButtonClicked);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    connect(client, &Client::connected, this, &::MainWindow::onConnectionSuccess);
    connect(ui->addFriend, &QPushButton::clicked, this, [=]() { ui->ADD_OUTGOING->setCurrentIndex(0); m_addFriendPage->load(client->getClientID());});
    connect(ui->outgoing, &QPushButton::clicked, this, [=]() { ui->ADD_OUTGOING->setCurrentIndex(1);});
    connect(ui->ADD_OUTGOING,
            &QStackedWidget::currentChanged,
            this,
            [=](int idx){
                // if we just switched _to_ your outgoing-page…
                if (idx == ui->ADD_OUTGOING->indexOf(m_outgoingPage))
                    m_outgoingPage->setRequests(client->getClientID());
            });
    connect(m_addFriendPage, &AddFriendPage::addFriendRequested,
            this, [this](const QString &nick){
            QString status = manager->checkFriendship(nick);
            if(status.isEmpty() || status == "PENDING"){
                QMessageBox::warning(this, "User not found", " or request is Pending");
                }
            });
    updateUserPanelPosition();

}



void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // Each time the window resizes, move the userPanel
    updateUserPanelPosition();
}



void MainWindow::updateUserPanelPosition()
{
    // We want it 8 px from left edge, 8 px above the statusBar (if you have one)
    const int margin = 8;
    int sbHeight = 0;
    int x = margin;
    int y = this->height()
            - userPanel->height()
            - sbHeight
            - margin;

    userPanel->move(x, y);
}


void MainWindow::updateAvatar(const QString &fileName)
{
    QPixmap pix(fileName);
    if (pix.isNull()) {
        qWarning() << "Failed to load avatar from" << fileName;
        return;
    }

    avatarButton->setIcon(QIcon(makeRoundedAvatar(pix, 48)));
}



void MainWindow::onAvatarButtonClicked()
{
    // 1) Open file dialog to let the user pick an image
    QString srcFile = QFileDialog::getOpenFileName(
        this,
        tr("Choose avatar image"),
        QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)")
        );
    if (srcFile.isEmpty())
        return;  // user canceled
    // 2) Update avatarButton’s icon to the chosen file
    QString userUploadFolder = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Chat/uploads/" + client->getClientName() + "/avatar.png";
    if(QFile::exists(userUploadFolder)){
        QFile::remove(userUploadFolder);
    }
    QFile file(userUploadFolder);
    if(file.copy(srcFile, userUploadFolder)){
        qDebug() << "copy done success";
    }
    else{
        qDebug() << "could not copy";
    }
    manager->updateUserAvatar(userUploadFolder, client->getClientName());
    updateAvatar(srcFile);
}

void MainWindow::refreshForUser(){
    updateAvatar(client->getClientAvatar());
    nameLabel->setText(client->getClientName());
    ui->ADD_OUTGOING->setCurrentWidget(m_addFriendPage);
    m_addFriendPage->load(client->getClientID());

}

void MainWindow::onDisconnectClicked(){
    qCritical() << "User li kan: " << client->getClientName();
    client->disconnectFromServer();
    Login h;
    this->hide();
    if (h.exec() == QDialog::Rejected) {
        this->close();      // Exit application if dialog is rejected
    }
    refreshForUser();
    this->show();
}
void MainWindow::onConnectionSuccess()
{

    qDebug() << "User: " << client->getClientName();
    qDebug() << "ID: " << client->getClientID();
}


