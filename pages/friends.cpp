#include "friends.h"
#include "ui_friends.h"
#include <QElapsedTimer>
Friends::Friends(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::amigos)
    , m_manager(Manager::get())
{
    ui->setupUi(this);
    ui->offlineFriends->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->onlineFriends->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    connect(m_manager->getClient(), &Client::friendsReady,
            this,   &Friends::load);
    connect(m_manager->getClient(), &Client::updateFriends, this, &Friends::update);
    connect(m_manager->getClient(), &Client::addNewFriendRow, this, [this](Data d){
        FriendRow* newRow = new FriendRow(d);
        connect(newRow, &FriendRow::clicked, this, [this](Data friendData){
            emit friendClicked(friendData);
        });
        if (m_manager->getClient()->getFriends().value(d, 0)) {
            ui->onlineFriends->addWidget(newRow);
        } else {
            ui->offlineFriends->addWidget(newRow);
        }
    });
}

Friends::~Friends()
{
    delete ui;
}

void Friends::load(QHash<Data, int> friends) {
    auto children = ui->friendsScrollAreaContent->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for(auto* w : children){
        w->deleteLater();
    }

    for(auto it = friends.constBegin(); it != friends.constEnd(); ++it){
        FriendRow* friendRow = new FriendRow(it.key());
        connect(friendRow, &FriendRow::clicked, this, [this](Data d){
            emit friendClicked(d);});
        if(it.value() == 0){
            ui->offlineFriends->addWidget(friendRow);
        }
        else{
            ui->onlineFriends->addWidget(friendRow);
        }
    }
}

void Friends::update(int id, bool online){
    Row* row;
    if(online)
    {
        for(int i = 0; i < ui->offlineFriends->count(); i++)
        {
            row = static_cast<FriendRow*>(ui->offlineFriends->itemAt(i)->widget());
            if(row->data().user_id == id)
            {
                Data d = row->data();
                m_manager->getClient()->getFriends()[d] = 1;
                FriendRow* replacement = new FriendRow(d);
                ui->onlineFriends->addWidget(replacement);
                connect(replacement, &FriendRow::clicked, this, [this](Data d){
                    emit friendClicked(d);});
                row->deleteLater();
            }
        }
    }
    else
    {
        for(int i = 0; i < ui->onlineFriends->count(); i++)
        {
            row = static_cast<FriendRow*>(ui->onlineFriends->itemAt(i)->widget());
            if(row->data().user_id == id)
            {
                Data d = row->data();
                m_manager->getClient()->getFriends()[d] = 0;
                FriendRow* replacement = new FriendRow(d);
                connect(replacement, &FriendRow::clicked, this, [this](Data d){
                    emit friendClicked(d);});
                ui->offlineFriends->addWidget(replacement);
                row->deleteLater();
            }
        }
    }
}
