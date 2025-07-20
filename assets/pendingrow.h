#ifndef PENDINGROW_H
#define PENDINGROW_H

#include "row.h"
#include "utils.h"
#include <QWidget>

class PendingRow : public Row
{
    Q_OBJECT
public:
    explicit PendingRow(Data d, QWidget *parent = nullptr);

signals:
    void accepted(const Data &);
    void declined(const Data &);
private:

};

#endif // PENDINGROW_H
