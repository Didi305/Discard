#ifndef ROW_H
#define ROW_H
#include <QMouseEvent>
#include <QWidget>
#include <QPixmap>
#include <QFrame>
#include <qboxlayout.h>
#include "utils.h"

enum class RowType { Pending, Outgoing, Friend};
class Manager;
class Row : public QFrame
{
    Q_OBJECT
public:
    Row(const Data &d, RowType type, QWidget *parent = nullptr);
    Row(Row* row = nullptr, QWidget *parent = nullptr);
    const Data& data() const { return m_data; }
signals:
    void accepted(const Data &);
    void declined(const Data &);
    void canceled(const Data &);
    void clicked(const Data &d);
protected:
    void mousePressEvent(QMouseEvent *e) override {
        if (e->button() == Qt::LeftButton)
            emit clicked(m_data);
        QFrame::mousePressEvent(e);
    }
    void paintEvent(QPaintEvent *e) override;   // hover + separator
private:
    RowType type;
    Data m_data;
    Manager* m_manager;
    QHBoxLayout* h;
};
#endif
