#include "mainwindow.h"
#include "chatserver.h"
#include "client.h"
#include "manager.h"
#include "login.h"
#include "register.h"
#include <QJsonObject>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Manager *mgr = Manager::get();
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "probe");
    QJsonObject DBConfig;
    mgr->init(DBConfig, db);
    Client client;
    Login h;
    MainWindow w;
    h.setClient(&client);
    if (h.exec() == QDialog::Rejected) {
        return 0;  // Exit application if dialog is rejected
    }
    w.setClient(&client);
    w.show();
    ChatServer server;

    // Send a test message after connecting
    QObject::connect(&client, &Client::connected, [&client]() {
        client.sendMessage("Hello, Server!");
    });
    return a.exec();
}
