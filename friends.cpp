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
}

Friends::~Friends()
{
    delete ui;
}

void Friends::load(QHash<Data, int> friends) {
    qDebug() << "tried loading friend rows";
    auto children = ui->scrollAreaWidgetContents->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for(auto* w : children){
        w->deleteLater();
    }

    for(auto it = friends.constBegin(); it != friends.constEnd(); ++it){
        Row* friendRow = new Row(it.key(), RowType::Friend);
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
    qDebug() << "ta finek a zebi";
    if(online)
    {
        qDebug() << "tcoonecta khouna dir chi 9alwa";
        for(int i = 0; i < ui->offlineFriends->count(); i++)
        {
            row = static_cast<Row*>(ui->offlineFriends->itemAt(i)->widget());
            if(row->data().user_id == id)
            {
                Data d = row->data();
                m_manager->getClient()->getFriends()[d] = 1;
                Row* replacement = new Row(d, RowType::Friend);
                ui->onlineFriends->addWidget(replacement);
                row->deleteLater();
            }
        }
    }
    else
    {
        qDebug() << "mcha khouna dir chi 9alwa";
        for(int i = 0; i < ui->onlineFriends->count(); i++)
        {
            row = static_cast<Row*>(ui->onlineFriends->itemAt(i)->widget());
            if(row->data().user_id == id)
            {
                Data d = row->data();
                m_manager->getClient()->getFriends()[d] = 0;
                Row* replacement = new Row(d, RowType::Friend);
                ui->offlineFriends->addWidget(replacement);
                row->deleteLater();
            }
        }
    }
}
