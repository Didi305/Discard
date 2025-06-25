#include "friends.h"
#include "ui_friends.h"

Friends::Friends(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Friends)
    , m_manager(Manager::get())
{
    ui->setupUi(this);
    ui->offlineFriends->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->onlineFriends->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

Friends::~Friends()
{
    delete ui;
}

void Friends::load(int userId) {
    qDebug() << "tried loading friend rows";
    auto friends = m_manager->getClient()->getFriends();
    auto children = ui->scrollAreaWidgetContents->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
    for(auto* w : children){
        w->deleteLater();
    }

    for(auto it = friends.constBegin(); it != friends.constEnd(); ++it){
        Row* friendRow = new Row(it.key(), RowType::Friend);
        if(it.value() == 0){
            ui->offlineFriends->addWidget(friendRow);
            qDebug() << "count =" << ui->offlineFriends->count();
        }
        else{
            ui->onlineFriends->addWidget(friendRow);
            qDebug() << "count =" << ui->onlineFriends->count();
        }
    }

}
