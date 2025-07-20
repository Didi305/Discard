#ifndef DIRECTMESSAGE_H
#define DIRECTMESSAGE_H
#include "manager.h"
#include "utils.h"
#include <QShortcut>
#include <QWidget>
namespace Ui {
class DirectMessage;
}

class DirectMessage : public QWidget
{
    Q_OBJECT

public:
    explicit DirectMessage(QWidget *parent = nullptr);
    void load(Data d);
    ~DirectMessage();

private slots:
    void update(Data d);
private:
    Ui::DirectMessage *ui;
    bool eventFilter(QObject *object, QEvent *event);
    Manager* m_manager;
    StringContainer messages;
    int receiverID;
    int conversaionID;
    int lastMessageSender = -1;
};

#endif // DIRECTMESSAGE_H
