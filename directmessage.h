#ifndef DIRECTMESSAGE_H
#define DIRECTMESSAGE_H
#include "manager.h"
#include "utils.h"
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

protected:
    void keyPressEvent(QKeyEvent *e) override;

private:
    Ui::DirectMessage *ui;
    Manager* m_manager;
    int receiverID;
};

#endif // DIRECTMESSAGE_H
