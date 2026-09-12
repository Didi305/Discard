#include <QCoreApplication>
#include <QDebug>

#include "chatserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    ChatServer server;

    qInfo() << "Discard ChatServer started on port 1234";

    return app.exec();
}