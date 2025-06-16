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
    QLayoutItem *child;
    while ((child = ui->listLayout->takeAt(0))) {
        delete child->widget();
        delete child;
    }
    m_pendingRows = manager->fetchRequests(userId, RowType::Pending);
    // 2) Add each widget to your layout
    for (auto& uptr : m_pendingRows) {
        Row* raw = uptr.release();
        ui->listLayout->addWidget(raw);
        connect(raw, &Row::accepted, this, [this, raw](Data d){
            manager->acceptRequest(d.user_id);
            raw->deleteLater();
        });
        connect(raw, &Row::declined, this, [this, raw](Data d){
            manager->declineRequest(d.user_id);
            raw->deleteLater();
        });
    }

}

void AddFriendPage::onAddFriendButtonClicked(){
    const QString nick = ui->addFriendInput->text().trimmed();
    if (!nick.isEmpty()) emit addFriendRequested(nick);
}
