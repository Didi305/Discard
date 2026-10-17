#include <QStatusBar>
#include <QCryptographicHash>
#include <QSettings>
#include <QTimer>
#include "mainwindow.h"
#include "twofactor.h"

namespace {
QString twoFactorReminderKey(const Client *client, int userId) {
    const auto serverHash = QCryptographicHash::hash(client->serverEndpoint().toUtf8(),
                                                    QCryptographicHash::Sha256).toHex();
    return QStringLiteral("twoFactorReminder/%1/%2/hidden")
        .arg(QString::fromLatin1(serverHash))
        .arg(userId);
}
}

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

    twoFactorButton = new QPushButton(this);
    twoFactorButton->setObjectName("twoFactorButton");
    twoFactorButton->setMinimumHeight(45);
    twoFactorButton->setFlat(true);
    twoFactorButton->setCursor(Qt::PointingHandCursor);

    auto *twoFactorLayout = new QHBoxLayout(twoFactorButton);
    twoFactorLayout->setContentsMargins(8, 0, 10, 0);
    twoFactorLayout->setSpacing(8);

    auto *shieldIcon = new QLabel(twoFactorButton);
    shieldIcon->setPixmap(QIcon(":/icons/twofactor.svg").pixmap(QSize(24, 24), devicePixelRatioF()));
    shieldIcon->setFixedSize(24, 24);
    shieldIcon->setAttribute(Qt::WA_TransparentForMouseEvents);
    twoFactorLayout->addWidget(shieldIcon);

    auto *twoFactorTitle = new QLabel(tr("Two-factor Authentication"), twoFactorButton);
    twoFactorTitle->setObjectName("twoFactorTitle");
    twoFactorTitle->setAttribute(Qt::WA_TransparentForMouseEvents);
    twoFactorLayout->addWidget(twoFactorTitle, 1);

    twoFactorStatusLabel = new QLabel(twoFactorButton);
    twoFactorStatusLabel->setObjectName("twoFactorStatus");
    twoFactorStatusLabel->setFixedSize(44, 24);
    twoFactorStatusLabel->setAlignment(Qt::AlignCenter);
    twoFactorStatusLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    twoFactorLayout->addWidget(twoFactorStatusLabel);

    ui->verticalLayout_2->addWidget(twoFactorButton);
    updateTwoFactorIndicator(TwoFactorState::Unknown);
    connect(twoFactorButton, &QPushButton::clicked, this, &MainWindow::onAccountSecurityClicked);


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

    m_FriendsPage->load(client->getFriends());
    loadConversations();
    connect(client, &Client::relationshipsChanged, this, [this] {
        manager->fetchFriends(client->getClientID(), client->getFriends());
        m_FriendsPage->load(client->getFriends());
        m_addFriendPage->load(client->getClientID());
        m_outgoingPage->setRequests(client->getClientID());
    });
    connect(client, &Client::disconnected, this, [this] {
        ++reminderGeneration;
        updateTwoFactorIndicator(TwoFactorState::Unknown);
        statusBar()->showMessage(tr("Disconnected. Sign in again to continue."));
    });
    const double margin = 26;
    int upHeight = ui->userPanel->height();
    int x = 9;
    int y = this->height() - 1.5 * upHeight - margin;
    ui->userPanel->move(x, y);
    scheduleTwoFactorReminder();
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
    if (!manager->updateUserAvatar(srcFile, client->getClientName())) {
        QMessageBox::warning(this, tr("Avatar not updated"), manager->lastError());
        return;
    }
    const QString cachedAvatar = manager->avatarPathForUser(client->getClientID());
    client->setClientAvatar(cachedAvatar);
    client->getClientData().avatar = cachedAvatar;
    updateAvatar(cachedAvatar);
}

void MainWindow::onAccountSecurityClicked()
{
    openTwoFactorSettings();
}

void MainWindow::updateTwoFactorIndicator(TwoFactorState state)
{
    QString stateName;
    QString statusText;
    QString description;
    if (state == TwoFactorState::Enabled) {
        stateName = QStringLiteral("enabled");
        statusText = tr("ON");
        description = tr("Two-factor authentication is on.");
    } else if (state == TwoFactorState::Disabled) {
        stateName = QStringLiteral("disabled");
        statusText = tr("OFF");
        description = tr("Two-factor authentication is off. Click to set it up.");
    } else {
        stateName = QStringLiteral("unknown");
        statusText = QStringLiteral("?");
        description = tr("Two-factor authentication status is unavailable.");
    }
    twoFactorStatusLabel->setText(statusText);
    twoFactorStatusLabel->setProperty("state", stateName);
    twoFactorStatusLabel->style()->unpolish(twoFactorStatusLabel);
    twoFactorStatusLabel->style()->polish(twoFactorStatusLabel);
    twoFactorStatusLabel->update();
    twoFactorButton->setToolTip(description);
    twoFactorButton->setAccessibleName(tr("Two-factor Authentication: %1").arg(statusText));
    twoFactorButton->setAccessibleDescription(description);
}

MainWindow::TwoFactorState MainWindow::refreshTwoFactorStatus()
{
    const int userId = client->getClientID();
    if (userId <= 0) {
        updateTwoFactorIndicator(TwoFactorState::Unknown);
        return TwoFactorState::Unknown;
    }

    const auto reply = manager->securityRequest("mfa_status");
    const auto enabled = reply["result"].toObject().value("enabled");
    if (!reply["ok"].toBool() || !enabled.isBool() || client->getClientID() != userId) {
        updateTwoFactorIndicator(TwoFactorState::Unknown);
        return TwoFactorState::Unknown;
    }

    const auto state = enabled.toBool() ? TwoFactorState::Enabled : TwoFactorState::Disabled;
    updateTwoFactorIndicator(state);
    return state;
}

void MainWindow::scheduleTwoFactorReminder()
{
    const int userId = client->getClientID();
    const quint64 generation = ++reminderGeneration;
    updateTwoFactorIndicator(TwoFactorState::Unknown);
    if (userId <= 0) {
        return;
    }
    QTimer::singleShot(0, this, [this, userId, generation] {
        if (generation == reminderGeneration && client->getClientID() == userId) {
            showTwoFactorReminder(userId);
        }
    });
}

void MainWindow::showTwoFactorReminder(int userId)
{
    if (twoFactorDialogOpen || !isVisible() || refreshTwoFactorStatus() != TwoFactorState::Disabled) {
        return;
    }

    QSettings settings(QStringLiteral("Discard"), QStringLiteral("Discard-M2"));
    const auto preferenceKey = twoFactorReminderKey(client, userId);
    if (settings.value(preferenceKey, false).toBool()) {
        return;
    }

    twoFactorDialogOpen = true;
    TwoFactorReminderDialog reminder(client->getClientName(), this);
    const auto result = reminder.exec();
    twoFactorDialogOpen = false;
    if (client->getClientID() != userId) {
        return;
    }
    settings.setValue(preferenceKey, reminder.remindersDisabled());
    if (result == QDialog::Accepted) {
        openTwoFactorSettings(true);
    }
}

void MainWindow::openTwoFactorSettings(bool startSetup)
{
    if (twoFactorDialogOpen || client->getClientID() <= 0) {
        return;
    }
    twoFactorDialogOpen = true;
    AccountSecurityDialog security(manager, this, startSetup);
    security.exec();
    twoFactorDialogOpen = false;
    refreshTwoFactorStatus();
}

void MainWindow::refreshForUser(){
    statusBar()->clearMessage();
    updateAvatar(client->getClientAvatar());
    ui->userNameLabel->setText(client->getClientName());
    ui->ADD_OUTGOING->setCurrentWidget(m_addFriendPage);
    m_addFriendPage->load(client->getClientID());
    m_FriendsPage->load(client->getFriends());
    loadConversations();
    scheduleTwoFactorReminder();
}

void MainWindow::onDisconnectClicked(){
    qCritical() << "User li kan: " << client->getClientName();
    client->disconnectFromServer();
    Login h;
    this->hide();
    if (h.exec() == QDialog::Rejected) {
        this->close();      // Exit application if dialog is rejected
        return;
    }
    refreshForUser();
    this->show();
}
void MainWindow::onConnectionSuccess()
{
    // A TCP connection alone does not complete password/second-factor sign-in.
    if (client->getClientID() <= 0) return;
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
    const auto summaries = manager->fetchConversations();
    qDebug() << "tried loading convs";
    qDebug() << "convs: " << summaries.size();
    for (const auto& summary : summaries){
        ConversationRow* row = new ConversationRow(
            summary.conversationId, summary.peer, summary.lastMessage);
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


