#ifndef FRIENDS_H
#define FRIENDS_H

#include "manager.h"
#include <QWidget>
#include "chatserver.h"
namespace Ui {
class amigos;
}

class Friends : public QWidget
{
    Q_OBJECT

public:
    explicit Friends(QWidget *parent = nullptr);
    ~Friends();
    void load(QHash<Data, int> friends);
public slots:
    void update(int id, bool online);
private:
    Ui::amigos *ui;
    Manager* m_manager;
    std::vector<std::unique_ptr<Row>> friendRows;
};

#endif // FRIENDS_H
