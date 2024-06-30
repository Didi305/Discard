#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "client.h"
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    client = new Client(this);

    connect(ui->sendMessageButton, &QPushButton::clicked, this, &MainWindow::on_sendMessageButton_clicked);
    connect(ui->connectButton, &QPushButton::clicked, this, &MainWindow::on_connectButton_clicked);
    connect(client, &Client::connected, this, &MainWindow::on_connectionSuccess);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sendMessageButton_clicked()
{
    QString message = ui->messageBox->text();
    client->sendMessage(message);
    ui->chatBox->append("Client: " + message);
    ui->messageBox->clear();
}

void MainWindow::on_connectButton_clicked() {
    QString host = "127.0.0.1";
    int port = 1234;
    client->connectToServer(host, port);
}
void MainWindow::on_connectionSuccess()
{
    QMessageBox::information(this, "Connection", "Successfully connected to the server!");
}
