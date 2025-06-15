#include "outgoingpage.h"
#include "ui_outgoingpage.h"

OutgoingPage::OutgoingPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::OutgoingPage)
    , m_manager(Manager::get())
{
    ui->setupUi(this);
    ui->listLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
}

OutgoingPage::~OutgoingPage()
{
    delete ui;
}

void OutgoingPage::setRequests(int userID) {
    // clear existing
    QLayoutItem *child;
    while ((child = ui->listLayout->takeAt(0))) {
        delete child->widget();
        delete child;
    }
    m_outgoingRows = m_manager->fetchRequests(userID, requestType::user);
    // 2) Add each widget to your layout
    for (auto& uptr : m_outgoingRows) {
        ui->listLayout->addWidget(uptr.get());
    }
}


void OutgoingPage::reload(int clientId) {

}
