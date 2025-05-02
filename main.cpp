#include "mainwindow.h"
#include "chatserver.h"
#include "client.h"
#include <QApplication>
#include "dialog.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client client;
    Dialog h;
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
