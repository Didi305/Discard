#ifndef OUTGOINGROW_H
#define OUTGOINGROW_H
#include "utils.h"
#include "row.h"
#include <QWidget>

class OutgoingRow : public Row
{
    Q_OBJECT
public:
    explicit OutgoingRow(Data d, QWidget *parent = nullptr);

signals:
    void canceled(const Data &);
private:
};
#endif // OUTGOINGROW_H
