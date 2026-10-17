#ifndef OUTGOINGPAGE_H
#define OUTGOINGPAGE_H

#include "manager.h"
#include "row.h"
#include "ui_outgoingpage.h"
#include <QWidget>
#include <qlayoutitem.h>

namespace Ui {
class OutgoingPage;
}

class OutgoingPage : public QWidget
{
    Q_OBJECT

public:
    explicit OutgoingPage(QWidget *parent = nullptr);
    ~OutgoingPage();
    QVBoxLayout* outgoingList();
    void setRequests(int userID);
public slots:
    void update(int clientId);
private:
    Ui::OutgoingPage *ui;
    Manager *m_manager {nullptr};
};
#endif // OUTGOINGPAGE_H
