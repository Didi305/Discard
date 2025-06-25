#ifndef FRIENDS_H
#define FRIENDS_H

#include "manager.h"
#include <QWidget>

namespace Ui {
class Friends;
}

class Friends : public QWidget
{
    Q_OBJECT

public:
    explicit Friends(QWidget *parent = nullptr);
    ~Friends();
    void load(int user_id);
private:
    Ui::Friends *ui;
    Manager* m_manager;
    std::vector<std::unique_ptr<Row>> friendRows;
};

#endif // FRIENDS_H
