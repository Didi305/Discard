#include <QFileDialog>
#include <QWidget>
#include <QPainterPath>
#include <QFrame>
#include <QMessageBox>
#include <QStandardPaths>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include "requestrow.h"
#include "ui_MainWindow.h"
#include "MainWindow.h"
#include "manager.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , userPanel(new QFrame)
    , avatarButton(new QPushButton(userPanel))
    , nameLabel(new QLabel(userPanel))
    , manager(Manager::get())
    , client(manager->getClient())
{
    ui->setupUi(this);
    auto *list = ui->scrollAreaPending->widget()->findChild<QVBoxLayout*>("listLayout");      // scrollAreaWidgetContent
    list->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    std::vector<RequestRow*> pendingRequests = manager->fetchPendingRequests(client->getClientID());
    for (auto row : pendingRequests)           // row is a raw pointer
    {
        row->setParent(ui->scrollAreaWidgetContents);
        list->addWidget(row);
    }

    userPanel->setParent(ui->centralwidget);
    userPanel->setObjectName("userPanel");
    userPanel->setFixedHeight(60);          // 50 px tall
    userPanel->setFixedWidth(335);          // adjust width as needed
    userPanel->setStyleSheet(R"(
        QFrame#userPanel {
            background-color: #32353a;
            border-radius: 4px;
        }
    )");

    // Give it a horizontal layout to host avatarButton + username
    auto *lay = new QHBoxLayout(userPanel);
    lay->setContentsMargins(8, 4, 8, 4);
    lay->setSpacing(6);

    // avatarButton
    avatarButton->setFixedSize(48, 48);
    avatarButton->setIconSize({48, 48});
    avatarButton->setFlat(true);
    avatarButton->setCursor(Qt::PointingHandCursor);
    avatarButton->setStyleSheet("background-color: #2f3136");        // no border


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
    nameLabel->setText(client->getClientName());
    updateAvatar(client->getClientAvatar());

    userPanel->show();
    connect(avatarButton, &QPushButton::clicked, this, &MainWindow::onAvatarButtonClicked);
    connect(client, &Client::connected, this, &::MainWindow::onConnectionSuccess);
    connect(ui->addFriendButton, &QPushButton::clicked, this, &MainWindow::onAddFriendButtonClicked);
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

    // 1) Create a circular mask of size 48×48
    QPixmap circular(48, 48);
    circular.fill(Qt::transparent);
    {
        QPainter painter(&circular);
        painter.setRenderHint(QPainter::Antialiasing);
        QPainterPath path;
        path.addEllipse(0, 0, 48, 48);
        painter.setClipPath(path);

        // Draw the loaded pixmap into that circular area, scaling it as needed
        painter.drawPixmap(
            0, 0, 48, 48,
            pix.scaled(48, 48,
                       Qt::KeepAspectRatioByExpanding,
                       Qt::SmoothTransformation)
            );
    }
    // 2) Set the circular pixmap as the button’s icon
    avatarButton->setIcon(QIcon(circular));
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


void MainWindow::onAddFriendButtonClicked(){
    QString friendNickname = ui->addFriendInput->text();
    QString status = manager->checkFriendship(friendNickname);
    if(status.isEmpty() || status == "PENDING"){
        QMessageBox::warning(this, "User not found", " or request is Pending");
    }
}

void MainWindow::onConnectionSuccess()
{

    qDebug() << "User: " << client->getClientName();
    qDebug() << "ID: " << client->getClientID();
}


