#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , manager(Manager::get())
    , client(manager->getClient())
    , conversations(manager->fetchConversations(client->getClientData()))
    , m_addFriendPage(new AddFriendPage(this))
    , m_outgoingPage(new OutgoingPage(this))
    , m_FriendsPage(new Friends(this))
    , m_DirectMessagePage(new DirectMessage(this))
{
    ui->setupUi(this);
    setStyleSheet(Utils::readText(":/styles/dark.qss", "dark.qss"));
    m_banner = new DragBanner(this);
    m_banner->hide();
    ui->conversationList->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    ui->verticalLayoutMid->addWidget(m_DirectMessagePage);
    ui->verticalLayoutMid->setCurrentIndex(0);
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
    connect(m_DirectMessagePage, &DirectMessage::newMessage, this, &MainWindow::updateConversations);
    connect(manager->getClient(), &Client::updateConversations, this, &MainWindow::updateConversations);

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
    manager->updateUserAvatar(srcFile, client->getClientName());
    const QString cachedAvatar = manager->avatarPathForUser(client->getClientID());
    client->setClientAvatar(cachedAvatar);
    client->getClientData().avatar = cachedAvatar;
    updateAvatar(cachedAvatar);
}

void MainWindow::refreshForUser(){
    updateAvatar(client->getClientAvatar());
    ui->userNameLabel->setText(client->getClientName());
    ui->ADD_OUTGOING->setCurrentWidget(m_addFriendPage);
    m_addFriendPage->load(client->getClientID());
    m_FriendsPage->load(client->getFriends());
    loadConversations();
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
    loadConversations();
}

void MainWindow::loadConversations(){
    QLayoutItem *child;
    while ((child = ui->conversationList->takeAt(0))) {
        delete child->widget();
        delete child;
    }
    conversations = manager->fetchConversations(client->getClientData());
    qDebug() << "tried loading convs";
    qDebug() << "convs: " << conversations.size();
    for (auto& conv : conversations){
        ConversationRow* row = new ConversationRow(conv->getConvID(), conv->data(), conv->getlastMessage());
        connect(row, &ConversationRow::clicked, this, [this](Data d){
            ui->verticalLayoutMid->setCurrentIndex(1);
            m_DirectMessagePage->load(d);
        });
        ui->conversationList->addWidget(row);
    }
}

void MainWindow::updateConversations(Message& lastMessage, Data& partner){
    qDebug() << "tried updating convs";
    qDebug() << "partner: " << partner.displayName << ", " << partner.user_id << ", " << Utils::stringFromStatus(partner.status);
    ConversationRow* row;
    for(int i = 0; i < ui->conversationList->count(); i++)
    {
        row = static_cast<ConversationRow*>(ui->conversationList->itemAt(i)->widget());
        qDebug() << row->getConvID() << " and " << lastMessage.sentIn;
        if (row->getConvID() == lastMessage.sentIn) {

            auto *replacement =
                new ConversationRow(lastMessage.sentIn, partner, lastMessage, /*parent=*/row->parentWidget());

            // put the new row at the same index
            ui->conversationList->insertWidget(0, replacement);

            // detach and delete the old one
            QLayoutItem *li = ui->conversationList->takeAt(i + 1); // old row moved to next slot
            if (li) {
                if (auto *old = li->widget()) old->deleteLater();   // or delete old;
                delete li;                                          // delete the layout item wrapper
            }

            // your connections...
            connect(replacement, &ConversationRow::clicked, this, [=](Data partner){
                ui->verticalLayoutMid->setCurrentIndex(1);
                m_DirectMessagePage->load(partner);
            });
            return;
        }
    }
    ConversationRow* newConvRow = new ConversationRow(lastMessage.sentIn, partner, lastMessage);
    ui->conversationList->addWidget(newConvRow);
}
void MainWindow::onItemDragging(int n)
{
    m_banner->showBanner(tr("Drop to attach"), tr("Release to add files"), n);
}

void MainWindow::onItemNotDragged()
{
    m_banner->hideBanner();
}


