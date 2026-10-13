#include <QCoreApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QDebug>
#include "manager.h"
#include "chatserver.h"
int main(int argc,char *argv[]) {
    QCoreApplication app(argc,argv);
    QCoreApplication::setApplicationName("DiscardChatServer");
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption({"config","External PostgreSQL configuration file.","path",
        QDir::homePath()+"/.config/discard-server/database.json"});
    parser.addOption({"bind","Listen address.","address","127.0.0.1"});
    parser.addOption({"port","Listen TCP port.","port","1234"});
    parser.process(app);
    bool valid=false; const auto port=parser.value("port").toUShort(&valid);
    QHostAddress address(parser.value("bind"));
    if(!valid || port==0 || address.isNull()) {qCritical()<<"Invalid listen address or port.";return 1;}
    Manager manager; QString error;
    if(!manager.init(parser.value("config"),error)) {qCritical().noquote()<<error;return 1;}
    ChatServer server(manager);
    if(!server.listen(address,port)) {qCritical()<<"Could not listen on requested address/port.";return 1;}
    qInfo()<<"Discard ChatServer listening on"<<address.toString()<<port;
    return app.exec();
}
