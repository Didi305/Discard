#ifndef ROW_H
#define ROW_H
#include <QWidget>
#include <QPixmap>
#include <qframe.h>

struct Data
{
    int user_id;
    QString avatar;
    QString displayName;
};

enum class RowType { Pending, Outgoing, Friend};

class Row : public QFrame
{
    Q_OBJECT
public:
    explicit Row(const Data &d, RowType type, QWidget *parent = nullptr);
    const Data& data() const { return m_data; }
signals:
    void accepted(const Data &);
    void declined(const Data &);
    void canceled(const Data &);
protected:
    Data m_data;
    void paintEvent(QPaintEvent *e) override;   // hover + separator
    RowType type;
private:
};
#endif
