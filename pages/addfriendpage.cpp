#include "addfriendpage.h"
#include "ui_addfriendpage.h"
#include "pendingrow.h"
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
    // The layout's parent widget takes ownership of each displayed row.
    for (const auto& data : m_manager->fetchPendingRequests(userID)) {
        PendingRow* pRow = new PendingRow(data);
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
