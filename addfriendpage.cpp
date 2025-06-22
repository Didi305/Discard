#include "addfriendpage.h"
#include "ui_addfriendpage.h"
AddFriendPage::AddFriendPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddFriendPage)
    , m_manager(Manager::get())
{
    ui->setupUi(this);
    ui->listLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    connect(ui->addFriendButton, &QPushButton::clicked,
            this, &AddFriendPage::onAddFriendButtonClicked);
    connect(m_manager->getClient(), &Client::updateAcceptRequests, this, &::AddFriendPage::update);
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
    m_pendingRows = m_manager->fetchRequests(userId, RowType::Pending);
    // 2) Add each widget to your layout
    for (auto& uptr : m_pendingRows) {
        Row* raw = uptr.release();
        ui->listLayout->addWidget(raw);
        connect(raw, &Row::accepted, this, [this, raw](Data d){
            m_manager->acceptRequest(d.user_id);
            raw->deleteLater();
        });
        connect(raw, &Row::declined, this, [this, raw](Data d){
            m_manager->declineRequest(d.user_id);
            raw->deleteLater();
        });
    }

}

void AddFriendPage::update(QString friend_id) {
    QLayoutItem *child;
    for (int i = 0; i < ui->listLayout->count(); ++i) {
        child = ui->listLayout->itemAt(i);
        Row *row = qobject_cast<Row *>(child->widget());
        if (!row) continue;
        int rowID = row->data().user_id;
        if(QString::number(rowID) == friend_id) delete child->widget();
    }
}

void AddFriendPage::onAddFriendButtonClicked(){
    const QString nick = ui->addFriendInput->text().trimmed();
    if (!nick.isEmpty()) emit addFriendRequested(nick);
}
