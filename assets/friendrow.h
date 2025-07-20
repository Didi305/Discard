#ifndef FRIENDROW_H
#define FRIENDROW_H

#include "row.h"
#include <QWidget>

class FriendRow : public Row
{
    Q_OBJECT
public:
    explicit FriendRow(Data d, QWidget *parent = nullptr);

signals:
    void clicked(const Data &d);
protected:
    void mousePressEvent(QMouseEvent *e) override {
        if (e->button() == Qt::LeftButton)
            emit clicked(m_data);
        QFrame::mousePressEvent(e);
    }
private:

};
#endif // FRIENDROW_H
