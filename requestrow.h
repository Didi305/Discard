#ifndef REQUESTROW_H
#define REQUESTROW_H
#include <QWidget>
#include <QPixmap>

struct RequestData         // feed your real data into this
{
    QString avatar;        // 40×40 image – can be empty
    QString displayName;   // “Greedoux”           // “@greedoux”  (optional)
};

class RequestRow : public QWidget
{
    Q_OBJECT
public:
    explicit RequestRow(const RequestData &d, QWidget *parent = nullptr);
    const RequestData& data() const { return m_data; }
signals:
    void accepted(const RequestData &);
    void declined(const RequestData &);

protected:
    RequestData m_data;
    void paintEvent(QPaintEvent *e) override;   // hover + separator

private:
};
#endif
