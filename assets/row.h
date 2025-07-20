#ifndef ROW_H
#define ROW_H
#include <QMouseEvent>
#include <QWidget>
#include <QPixmap>
#include <QFrame>
#include <qboxlayout.h>
#include "utils.h"
const int S = 32;
enum class RequestType{ Pending, Outgoing};
class Manager;
class Row : public QFrame
{
    Q_OBJECT
public:
    Row(const Data &d, QWidget *parent = nullptr);
    const Data& data() const { return m_data; }
signals:

protected:
    virtual void paintEvent(QPaintEvent *e) override;   // hover + separator
    Data m_data;
    QLabel* ava;
    QLabel* name;
    Manager* m_manager;
    QHBoxLayout* h;
    QVBoxLayout* v;
};
#endif
