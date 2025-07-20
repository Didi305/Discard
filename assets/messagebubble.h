// chatbubble.h ------------------------------------------------------
#pragma once
#include <QTextBrowser>

class MessageBubble : public QTextBrowser
{
    Q_OBJECT
public:
    explicit MessageBubble(QWidget *parent = nullptr)
        : QTextBrowser(parent)
    {
        setReadOnly(true);
        setFrameStyle(QFrame::NoFrame);
        setOpenLinks(false);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

        connect(document(), &QTextDocument::contentsChanged,
                this, &QWidget::updateGeometry);      // notify the layout
    }

    // ------- geometry ----------------------------------------------
    bool hasHeightForWidth() const override { return true; }

    int heightForWidth(int w) const override
    {
        QTextDocument doc;
        doc.setDefaultFont(font());        // clone without touching live doc
        doc.setHtml(toHtml());
        doc.setTextWidth(w);
        return int(doc.size().height()) + frameWidth()*2;
    }

    QSize sizeHint() const override
    {
        return { int(document()->idealWidth()),
                heightForWidth(width() > 0 ? width()
                                           : int(document()->idealWidth())) };
    }
};
