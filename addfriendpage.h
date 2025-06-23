#ifndef ADDFRIENDPAGE_H
#define ADDFRIENDPAGE_H

#include "manager.h"
#include "ui_addfriendpage.h"
#include <QWidget>
#include <qboxlayout.h>
#include "row.h"
namespace Ui {
class AddFriendPage;
}

class AddFriendPage : public QWidget
{
    Q_OBJECT

public:
    explicit AddFriendPage(QWidget *parent = nullptr);
    ~AddFriendPage();
    QVBoxLayout* pendingListLayout() const;
    void load(int userId);
    QPushButton* addButton() const {
        return ui->addFriendButton;    // use the exact objectName from your .ui
    }
signals:
    void addFriendRequested(const QString &nick);
private slots:
    void onAddFriendButtonClicked();
    void update(Data &d, QString type);
private:
    Ui::AddFriendPage *ui;
    Manager *m_manager;
    std::vector<std::unique_ptr<Row>>  m_pendingRows;
};

#endif // ADDFRIENDPAGE_H
