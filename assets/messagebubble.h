#pragma once

#include <QLabel>
#include <qpainter.h>
#include <qstyleoption.h>

class WrapLabel : public QLabel {
    Q_OBJECT
public:
    explicit WrapLabel(QWidget *parent = nullptr)
        : QLabel(parent)
    {
        // we do all our drawing in paintEvent, so clear the built‑in text
        // behaviour. Still allow selecting text if you want:
        setTextInteractionFlags(textInteractionFlags() | Qt::TextSelectableByMouse);
    }

protected:
    void paintEvent(QPaintEvent *) {
        QStyleOption opt;
        opt.initFrom(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

        // compute the inner rect
        QRect r = rect();
        r = r.marginsRemoved(contentsMargins());

        // combine both alignment and wrapping into an 'int'
        int flags =
            Qt::AlignLeft
            | Qt::AlignTop
            | Qt::TextWordWrap    // wrap at word boundaries
            | Qt::TextWrapAnywhere; // break long words anywhere

        // this overload takes an 'int' for flags
        style()->drawItemText(
            &p,
            r,
            flags,
            palette(),
            isEnabled(),
            text(),
            QPalette::WindowText
            );
    }

    QSize sizeHint() const override {
        // we should also override sizeHint so layouts can query
        // “how tall am I when wrapped to width W”?
        // But for simplicity, we’ll fall back on QLabel’s default.
        return QLabel::sizeHint();
    }
};
