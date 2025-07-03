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
        qDebug() << Utils::stringFromStatus(raw->data().status);
        ui->listLayout->addWidget(raw);
        connect(raw, &Row::accepted, this, [this, raw](Data d){
            m_manager->acceptRequest(d);
            raw->deleteLater();
        });
        connect(raw, &Row::declined, this, [this, raw](Data d){
            m_manager->declineRequest(d.user_id);
            raw->deleteLater();
        });
    }

}

void AddFriendPage::update(QJsonObject& obj, QString type) {
    Data d = {obj["id"].toInt(), obj["name"].toString(), obj["avatar"].toString(), Utils::statusFromString(obj["status"].toString())};
    QLayoutItem *child;
    qDebug() << "ID of the user to be updated: " << d.user_id;
    qDebug() << "his name is: " << d.displayName;
    qDebug() << "request type: " << type;
    if(type == "add_request"){
        Row* friendRequestRow = new Row(d, RowType::Pending);
        ui->listLayout->addWidget(friendRequestRow);
        connect(friendRequestRow, &Row::accepted, this, [this, friendRequestRow](Data d){
            m_manager->acceptRequest(d);
            friendRequestRow->deleteLater();
        });
        connect(friendRequestRow, &Row::declined, this, [this, friendRequestRow](Data d){
            m_manager->declineRequest(d.user_id);
            friendRequestRow->deleteLater();
        });
    }
    else{
        for (int i = 0; i < ui->listLayout->count(); ++i) {
            child = ui->listLayout->itemAt(i);
            Row *row = qobject_cast<Row *>(child->widget());
            if (!row) continue;
            int rowID = row->data().user_id;
            qDebug() << "Row ID: " << rowID << " and user to be updated: " << d.user_id;
            if(rowID == d.user_id) delete child->widget();
        }
    }
}

void AddFriendPage::onAddFriendButtonClicked(){
    const QString nick = ui->addFriendInput->text().trimmed();
    if (!nick.isEmpty()) emit addFriendRequested(nick);
}
