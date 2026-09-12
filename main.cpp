#include "mainwindow.h"

#include "client.h"
#include "manager.h"
#include "login.h"
#include <QJsonObject>
#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QStandardPaths>
#include <QFile>
#include <QSslSocket>
#include <QDebug>
#include <qlibraryinfo.h>
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
    QCoreApplication::setApplicationName(QStringLiteral("Discard"));
    qDebug() << "SQL drivers:" << QSqlDatabase::drivers();
    qDebug() << "QPSQL available:"
             << QSqlDatabase::isDriverAvailable("QPSQL");
    qDebug() << "Application dir:"
             << QCoreApplication::applicationDirPath();

    qDebug() << "Qt plugin path:"
             << QLibraryInfo::path(QLibraryInfo::PluginsPath);

    qDebug() << "Library paths:"
             << QCoreApplication::libraryPaths();

    qDebug() << "Formats:"
             << QImageReader::supportedImageFormats();
    a.setApplicationDisplayName("Discard");
    Manager *mgr = Manager::get();
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", "probe");
    QJsonObject DBConfig;
    if (!mgr->init(DBConfig, db)) return 1;

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
