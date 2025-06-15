#include "addfriendpage.h"
#include "ui_addfriendpage.h"
AddFriendPage::AddFriendPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddFriendPage)
    , manager(Manager::get())
{
    ui->setupUi(this);
    ui->listLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    connect(ui->addFriendButton, &QPushButton::clicked,
            this, &AddFriendPage::onAddFriendButtonClicked);
}

AddFriendPage::~AddFriendPage()
{
    delete ui;
}

void AddFriendPage::load(int userId) {
    // 1) Fetch into your member container
    m_pendingRows = manager->fetchRequests(userId, requestType::amigo);
    // 2) Add each widget to your layout
    for (auto& uptr : m_pendingRows) {
        ui->listLayout->addWidget(uptr.get());
        qDebug() << uptr.get()->data().displayName;
    }
}

void AddFriendPage::onAddFriendButtonClicked(){
    const QString nick = ui->addFriendInput->text().trimmed();
    if (!nick.isEmpty()) emit addFriendRequested(nick);
}
