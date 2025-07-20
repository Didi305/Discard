#include "friendrow.h"

FriendRow::FriendRow(Data d, QWidget *parent) : Row (d, parent)
{
    connect(this, &FriendRow::clicked, this, [&](const Data& d){
        qDebug() << "ID of the Friend in the row is: " << d.user_id << " " << d.displayName;
    });
}
