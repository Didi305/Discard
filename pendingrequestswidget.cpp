// pendingrequestswidget.cpp
#include "pendingrequestswidget.h"
#include "requestrow.h"
#include <QVBoxLayout>

PendingRequestsWidget::PendingRequestsWidget(QWidget *parent)
    : QScrollArea(parent)
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet(R"(
        QScrollArea { background:#313338; }      /* page background */
        QScrollBar:vertical {
            background:transparent; width:8px; margin:0;
        }
        QScrollBar::handle:vertical {
            background:#202225; border-radius:4px;
        }
        QScrollBar::handle:vertical:hover { background:#5865f2; }
    )");

    content = new QWidget;
    rowsLayout = new QVBoxLayout(content);
    rowsLayout->setAlignment(Qt::AlignTop);
    rowsLayout->setContentsMargins(0,0,0,0);
    rowsLayout->setSpacing(0);

    setWidget(content);
}

void PendingRequestsWidget::setRequests(const std::vector<RequestRow*> &list)
{
    // remove old rows -------------------------------------------------
    while (auto *w = content->findChild<RequestRow*>())
        delete w;

    // add new rows ----------------------------------------------------
    for (const auto &row : list)
    {
        rowsLayout->addWidget(row);

        // When × is clicked remove the row *and* notify whoever owns the list.
        connect(row, &RequestRow::declined, this, [this, row]{
            row->deleteLater();
            // TODO: emit something like requestCancelled(id);
        });
    }
}
