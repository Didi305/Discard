#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , client(nullptr)
{
    ui->setupUi(this);
    connect(ui->sendMessageButton, &QPushButton::clicked, this, &::MainWindow::onSendMessageButtonclicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setClient(Client *client){
    this->client = client;
    connect(client, &Client::connected, this, &::MainWindow::onConnectionSuccess);
    connect(client, &Client::messageReceived, this, &::MainWindow::onMessageReceived);
}
void MainWindow::onSendMessageButtonclicked()
{
    QString message = ui->messageBox->text();
    client->sendMessage(client->getClientName() + ": " + message);
    ui->messageBox->clear();
}


void MainWindow::onConnectionSuccess()
{

    qDebug() << "Nickname? :" << client->getClientName();
    QMessageBox::information(this, "Connection", "Successfully connected to the server!");
}

void MainWindow::onMessageReceived(QString &message)
{
    ui->chatBox->append(message);
}

