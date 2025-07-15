#include "directmessage.h"
#include "ui_directmessage.h"

DirectMessage::DirectMessage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DirectMessage)
    , m_manager(Manager::get())
{
    ui->setupUi(this);
    ui->messageInputLine->setAlignment(Qt::AlignVCenter);
    qDebug() << "DM object: " << objectName();
}

DirectMessage::~DirectMessage()
{
    delete ui;
}

void DirectMessage::load(Data d){
    ui->receiverUsername->setText(d.displayName);
    QPixmap avatar = Utils::makeRoundedAvatar(d.avatar, 56);
    ui->receiverAvatar->setPixmap(avatar);
}
