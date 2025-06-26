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
