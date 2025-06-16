#ifndef PENDINGREQUESTWIDGET_H
#define PENDINGREQUESTWIDGET_H
#include <QScrollArea>
#include <QVBoxLayout>
#include "row.h"
#include <vector>

class PendingRequestsWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit PendingRequestsWidget(QWidget *parent = nullptr);

    // call this whenever your backend delivers new data
    void setRequests(const std::vector<Row*> &list);

private:
    QWidget *content;
    Row *r;    // the container inside the scroll area
    QVBoxLayout *rowsLayout;    // vertical layout that holds Row widgets
};
#endif
