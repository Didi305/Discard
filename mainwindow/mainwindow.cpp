#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , manager(Manager::get())
    , client(manager->getClient())
    , m_addFriendPage(new AddFriendPage(this))
    , m_outgoingPage(new OutgoingPage(this))
    , m_FriendsPage(new Friends(this))
    , m_DirectMessagePage(new DirectMessage(this))
{
    ui->setupUi(this);
    m_banner = new DragBanner(this);
    m_banner->hide();

    ui->userPanel->setAttribute(Qt::WA_StyledBackground, true);
    ui->ADD_OUTGOING->setAttribute(Qt::WA_StyledBackground, true);
    m_DirectMessagePage->setAttribute(Qt::WA_StyledBackground, true);
    ui->verticalLayoutMid->addWidget(m_DirectMessagePage);
    ui->verticalLayoutMid->setCurrentIndex(1);
    ui->ADD_OUTGOING->addWidget(m_addFriendPage);
    ui->ADD_OUTGOING->addWidget(m_outgoingPage); // index 1
    ui->verticalLayoutRight->addWidget(m_FriendsPage);

    m_addFriendPage->load(client->getClientID());
    m_outgoingPage->setRequests(client->getClientID());
    ui->userNameLabel->setText(client->getClientName());
    ui->userAvatarButton->setIcon(QIcon(Utils::makeRoundedAvatar(client->getClientAvatar(), 48)));
    QIcon icon(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Discard/resources/icons/exit.svg");
    ui->disconnectButton->setIcon(icon);


    connect(ui->userAvatarButton, &QPushButton::clicked, this, &MainWindow::onAvatarButtonClicked);
    connect(ui->disconnectButton, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    connect(client, &Client::connected, this, &::MainWindow::onConnectionSuccess);
    connect(ui->addFriend, &QPushButton::clicked, this, [=]() { ui->verticalLayoutMid->setCurrentIndex(0); ui->ADD_OUTGOING->setCurrentIndex(0); m_addFriendPage->load(client->getClientID());});
    connect(ui->outgoing, &QPushButton::clicked, this, [=]() { ui->verticalLayoutMid->setCurrentIndex(0); ui->ADD_OUTGOING->setCurrentIndex(1);});
    connect(m_FriendsPage, &Friends::friendClicked, this, [=](Data d) {
                ui->verticalLayoutMid->setCurrentIndex(1);
                m_DirectMessagePage->load(d);
            });
    connect(ui->ADD_OUTGOING, &QStackedWidget::currentChanged, this, [=](int idx){
                // if we just switched _to_ your outgoing-page…
                if (idx == ui->ADD_OUTGOING->indexOf(m_outgoingPage))
                    m_outgoingPage->setRequests(client->getClientID());
            });
    connect(m_addFriendPage, &AddFriendPage::addFriendRequested, this, [this](const QString &nick){
            QString status = manager->checkFriendship(nick);
            if(status.isEmpty() || status == "PENDING"){
                QMessageBox::warning(this, "User not found", " or request is Pending");
                }
            });
    connect(m_DirectMessagePage, &DirectMessage::dragging, this, &MainWindow::onItemDragging);
    connect(m_DirectMessagePage, &DirectMessage::notdragging, this, &MainWindow::onItemNotDragged);



    const double margin = 26;
    int upHeight = ui->userPanel->height();
    int x = 9;
    int y = this->height() - 1.5 * upHeight - margin;
    ui->userPanel->move(x, y);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    // Each time the window resizes, move the userPanel
    const double margin = 26;
    int upHeight = ui->userPanel->height();
    int x = 9;
    int y = this->height() - 1.5 * upHeight - margin;
    ui->userPanel->move(x, y);
}



void MainWindow::updateAvatar(const QString &fileName)
{
    QPixmap pix(fileName);
    if (pix.isNull()) {
        qWarning() << "Failed to load avatar from" << fileName;
        return;
    }

    ui->userAvatarButton->setIcon(QIcon(Utils::makeRoundedAvatar(pix, 48)));
}



void MainWindow::onAvatarButtonClicked()
{
    // Open file dialog to let the user pick an image
    QString srcFile = QFileDialog::getOpenFileName(
        this,
        tr("Choose avatar image"),
        QString(),
        tr("Images (*.png *.jpg *.jpeg *.bmp *.gif)")
        );
    if (srcFile.isEmpty())
        return;
    //  Update avatarButton’s icon to the chosen file
    QString userUploadFolder = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Discard/uploads/" + client->getClientName() + "/avatar.png";
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
    ui->userNameLabel->setText(client->getClientName());
    ui->ADD_OUTGOING->setCurrentWidget(m_addFriendPage);
    m_addFriendPage->load(client->getClientID());
    m_FriendsPage->load(client->getFriends());
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

void MainWindow::onItemDragging(int n)
{
    m_banner->showBanner(tr("Drop to attach"), tr("Release to add files"), n);
}

void MainWindow::onItemNotDragged()
{
    m_banner->hideBanner();
}


