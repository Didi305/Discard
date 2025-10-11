#include "conversationrow.h"
#include <qstyleoption.h>
#include "manager.h"
ConversationRow::ConversationRow(int id, Data d, Message lastMessage, QWidget *parent)
    : Row{d, parent}
    , m_id(id)
    , m_lastMessage(lastMessage)
{
    drawLine = false;

    QString appendedMessage = (m_lastMessage.senderID == m_manager->getClient()->getClientID()) ? "You: " + m_lastMessage.body : m_lastMessage.body;
    QLabel* msg = new QLabel(appendedMessage);
    msg->setObjectName("lastMessage");
    horiz = new QVBoxLayout;
    h->takeAt(1);
    horiz->addWidget(name, 0);
    horiz->addWidget(msg, 0);
    h->addLayout(horiz, 1);
}

