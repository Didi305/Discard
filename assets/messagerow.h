#ifndef MESSAGEROW_H
#define MESSAGEROW_H
#include "utils.h"
#include "row.h"
#include <QWidget>

class MessageRow : public Row
{
    Q_OBJECT
public:
    explicit MessageRow(QString body, Data d, bool withAvatar, QString time, QWidget *parent = nullptr);
    QSize sizeHint() const override;
private:
    void paintEvent(QPaintEvent *e) override{

    }

private:
    QHBoxLayout* nameDateLayout;
};

#endif // MESSAGEROW_H
