#ifndef ROW_H
#define ROW_H
#include <QMouseEvent>
#include <QWidget>
#include <QPixmap>
#include <qframe.h>

enum class Status { ONLINE, HIDDEN, DO_NOT_DISTURB};
enum class RowType { Pending, Outgoing, Friend};
struct Data
{
    int user_id;
    QString avatar;
    QString displayName;
    QHash<Data, int> friends;
    Status status;
    bool operator==(Data const &other) const {
        return user_id == other.user_id;
    }
};
inline uint qHash(Data const &d, uint seed = 0) {
    // combine the hash of user_id (you can mix in other fields if needed)
    return ::qHash(d.user_id, seed);
}

class Manager;
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
};
#endif
