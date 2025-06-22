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
    m_outgoingRows = m_manager->fetchRequests(userID, RowType::Outgoing);
    // 2) Add each widget to your layout
    for (auto& uptr : m_outgoingRows) {
        Row* raw = uptr.release();
        ui->listLayout->addWidget(raw);
        connect(raw, &Row::canceled, this,
                [this, raw](Data d) {
                    m_manager->cancelRequest(d.user_id);
                    raw->deleteLater();
                });
    }
}


void OutgoingPage::update(QString friend_id) {
    QLayoutItem *child;
    for (int i = 0; i < ui->listLayout->count(); ++i) {
        child = ui->listLayout->itemAt(i);
        Row *row = qobject_cast<Row *>(child->widget());  // ✅ Safely gets the widget
        if (!row) continue;
        int rowID = row->data().user_id;
        if(QString::number(rowID) == friend_id) delete child->widget();
    }
}
