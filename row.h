#ifndef ROW_H
#define ROW_H
#include <QWidget>
#include <QPixmap>
#include <qframe.h>

struct Data         // feed your real data into this
{
    int user_id;
    QString avatar;        // 40×40 image – can be empty
    QString displayName;   // “Greedoux”           // “@greedoux”  (optional)
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
