#ifndef CONVERSATIONROW_H
#define CONVERSATIONROW_H

#include "row.h"
#include <QWidget>

class ConversationRow : public Row
{
    Q_OBJECT
public:
    explicit ConversationRow(int id, Data d, Message lastMessage, QWidget *parent = nullptr);
    int getConvID() const {
        return m_id;
    }
    Message getlastMessage(){
        return m_lastMessage;
    }
protected:
    void mousePressEvent(QMouseEvent *e) override {
        if (e->button() == Qt::LeftButton)
            emit clicked(m_data);
        QFrame::mousePressEvent(e);
    }
signals:
    void clicked(Data d);


private:
    int m_id;
    Message m_lastMessage;
    QVBoxLayout* horiz;
};

#endif // CONVERSATIONROW_H
