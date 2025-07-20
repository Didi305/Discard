#include "directmessage.h"
#include "messagerow.h"
#include "ui_directmessage.h"

DirectMessage::DirectMessage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DirectMessage)
    , m_manager(Manager::get())
{
    ui->setupUi(this);
    ui->messageContainer->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    ui->messageInputLine->installEventFilter(this);
    ui->messageInputLine->setAlignment(Qt::AlignVCenter);
    auto* enter = new QShortcut(QKeySequence(Qt::Key_Return), this);
    enter->setContext(Qt::WidgetWithChildrenShortcut);
    connect(enter, &QShortcut::activated, this, [this](){ if(!ui->messageInputLine->hasFocus()){
                                                                ui->messageInputLine->setFocus();}});
    connect(m_manager->getClient(), &Client::newMessage, this, &DirectMessage::update);
};


DirectMessage::~DirectMessage()
{
    delete ui;
}

void DirectMessage::load(Data d){
    QLayoutItem *child;
    while ((child = ui->messageContainer->takeAt(0))) {
        delete child->widget();
        delete child;
    }
    conversaionID = m_manager->checkConversation(m_manager->getClient()->getClientID(), d.user_id);
    receiverID = d.user_id;
    messages = m_manager->fetchMessages(conversaionID);
    ui->receiverUsername->setText(d.displayName);
    ui->receiverAvatar->setPixmap(Utils::makeRoundedAvatar(d.avatar, 56));
    lastMessageSender = -1;
    for (auto& m : messages.get<BySeq>()){
        if(m.senderID == d.user_id){
            MessageRow* msg = new MessageRow(m.body, d, m.senderID != lastMessageSender, m.sentAt);
            ui->messageContainer->addWidget(msg);
        }
        else{
            MessageRow* msg = new MessageRow(m.body, m_manager->getClient()->getClientData(), m.senderID != lastMessageSender, m.sentAt);
            ui->messageContainer->addWidget(msg);
        }
        lastMessageSender = m.senderID;
    }
}

void DirectMessage::update(Data d)
{
    MessageRow* msg = new MessageRow(d.message.body, d, d.user_id != lastMessageSender, d.message.sentAt);
    ui->messageContainer->addWidget(msg);
}

bool DirectMessage::eventFilter(QObject *object, QEvent *event){
    if (object == ui->messageInputLine && (event->type() == QEvent::KeyPress)){
        auto *kev = static_cast<QKeyEvent*>(event);
        if(kev->key() == Qt::Key_Return){
            Data d = m_manager->getClient()->getClientData();
            d.message = {ui->messageInputLine->toPlainText(), d.user_id, QDateTime::currentDateTimeUtc().toString()};
            ui->messageInputLine->clear();
            MessageRow* msg = new MessageRow(d.message.body, d, d.user_id != lastMessageSender, d.message.sentAt);
            ui->messageContainer->addWidget(msg);
            qDebug() << "message is: " << d.message.body;
            d.user_id = receiverID;
            m_manager->getClient()->sendMessage(d, "message");
            m_manager->postMessage(d.message.body, this->conversaionID, m_manager->getClient()->getClientID());
            return true;
        }
    }
    return false;
}
