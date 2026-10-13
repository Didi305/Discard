#include "outgoingpage.h"
#include "ui_outgoingpage.h"
#include "row.h"
OutgoingPage::OutgoingPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OutgoingPage)
    , m_manager(Manager::get())
{
    ui->setupUi(this);
    ui->listLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    connect(m_manager->getClient(), &Client::updatePendingRequests, this, &::OutgoingPage::update);
}

OutgoingPage::~OutgoingPage()
{
    delete ui;
}

void OutgoingPage::setRequests(int userID) {
    QLayoutItem *child;
    while ((child = ui->listLayout->takeAt(0))) {
        delete child->widget();
        delete child;
    }
    m_outgoingRows = m_manager->fetchOutgoingRequests(userID);
    // 2) Add each widget to your layout
    for (auto& uptr : m_outgoingRows) {
        OutgoingRow* oRow = new OutgoingRow(uptr.release()->data());
        ui->listLayout->addWidget(oRow);
        connect(oRow, &OutgoingRow::canceled, this,
                [this, oRow](Data d) {
                    if (!m_manager->cancelRequest(d.user_id)) return;
                    oRow->deleteLater();
                });
    }
}


void OutgoingPage::update(int friend_id) {
    QLayoutItem *child;
    for (int i = 0; i < ui->listLayout->count(); ++i) {
        child = ui->listLayout->itemAt(i);
        OutgoingRow *row = qobject_cast<OutgoingRow*>(child->widget());  //
        if (!row) continue;
        int rowID = row->data().user_id;
        if(rowID == friend_id) delete child->widget();
    }
}
