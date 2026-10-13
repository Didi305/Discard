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
    connect(m_manager->getClient(), &Client::updateAcceptRequest, this, &::AddFriendPage::update);
}

AddFriendPage::~AddFriendPage()
{
    delete ui;
}

void AddFriendPage::load(int userID) {
    QLayoutItem *child;
    while ((child = ui->listLayout->takeAt(0))) {
        delete child->widget();
        delete child;
    }
    m_pendingRows = m_manager->fetchPendingRequests(userID);
    // 2) Add each widget to your layout
    for (auto& uptr : m_pendingRows) {
        PendingRow* pRow = new PendingRow(uptr.release()->data());
        ui->listLayout->addWidget(pRow);
        connect(pRow, &PendingRow::accepted, this, [this, pRow](Data d){
            if (!m_manager->acceptRequest(d)) return;

            pRow->deleteLater();
        });
        connect(pRow, &PendingRow::declined, this, [this, pRow](Data d){
            if (!m_manager->declineRequest(d.user_id)) return;
            pRow->deleteLater();
        });
    }

}

void AddFriendPage::update(QJsonObject& obj, QString type) {
    Data d = {obj["id"].toInt(), obj["name"].toString(), obj["avatar"].toString(), Utils::statusFromString(obj["status"].toString())};
    QLayoutItem *child;
    if(type == "add_request"){
        PendingRow* pRow = new PendingRow(d);
        ui->listLayout->addWidget(pRow);
        connect(pRow, &PendingRow::accepted, this, [this, pRow](Data d){
            if (!m_manager->acceptRequest(d)) return;

            pRow->deleteLater();
        });
        connect(pRow, &PendingRow::declined, this, [this, pRow](Data d){
            if (!m_manager->declineRequest(d.user_id)) return;
            pRow->deleteLater();
        });
    }
    else{
        for (int i = 0; i < ui->listLayout->count(); ++i) {
            child = ui->listLayout->itemAt(i);
            Row *row = qobject_cast<Row *>(child->widget());
            if (!row) continue;
            int rowID = row->data().user_id;
            if(rowID == d.user_id) delete child->widget();
        }
    }
}

void AddFriendPage::onAddFriendButtonClicked(){
    const QString nick = ui->addFriendInput->text().trimmed();
    if (!nick.isEmpty()) emit addFriendRequested(nick);
}
