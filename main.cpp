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
    Client client;
    Manager *mgr = Manager::get();
    mgr->setClient(&client);
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "probe");
    QJsonObject DBConfig;
    mgr->init(DBConfig, db);
    Login h;
    if (h.exec() == QDialog::Rejected) {
        return 0;  // Exit application if dialog is rejected
    }
    MainWindow w;
    w.show();
    ChatServer server;

    // Send a test message after connecting
    return a.exec();
}
