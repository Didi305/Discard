#include "mainwindow.h"
#include "client.h"
#include "manager.h"
#include "login.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>

int main(int argc,char *argv[]) {
    QApplication app(argc,argv);
    QCoreApplication::setApplicationName("Discard-M2");
    app.setApplicationDisplayName("Discard");
    QCommandLineParser parser;parser.addHelpOption();
    parser.addOption({"host","ChatServer hostname or IP address.","host","127.0.0.1"});
    parser.addOption({"port","ChatServer TCP port.","port","41234"});
    parser.process(app);
    bool valid=false;const int port=parser.value("port").toInt(&valid);
    if(!valid||port<1||port>65535||parser.value("host").isEmpty())return 1;
    Client client;client.setEndpoint(parser.value("host"),port);
    Manager::get()->setClient(&client);
    Login login;if(login.exec()!=QDialog::Accepted)return 0;
    MainWindow window;window.show();
    return app.exec();
}
