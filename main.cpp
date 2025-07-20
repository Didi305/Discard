#include "mainwindow.h"
#include "chatserver.h"
#include "client.h"
#include "manager.h"
#include "login.h"
#include "utils.h"
#include "register.h"
#include <QJsonObject>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>
#include <QFile>

QString loadStyle(const QString &path)
{
    QFile f(path);
    if (!f.open(QFile::ReadOnly | QFile::Text))
        return {};
    return QTextStream(&f).readAll();
}

int main(int argc, char *argv[])
{
    qSetMessagePattern("%{time hh:mm:ss.zzz} %{message}");
    QApplication a(argc, argv);
    a.setStyleSheet(loadStyle(path + "/styles/dark.qss"));
    a.setApplicationDisplayName("Discard");
    Manager *mgr = Manager::get();
    ChatServer server;
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "probe");
    QJsonObject DBConfig;
    mgr->init(DBConfig, db);
    Client client;
    mgr->setClient(&client);
    Login h;
    if (h.exec() == QDialog::Rejected) {
        return 0;  // Exit application if dialog is rejected
    }
    MainWindow w;
    w.show();
    return a.exec();
}
