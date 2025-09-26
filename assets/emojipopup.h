#pragma once
#include <QFrame>
#include <QGridLayout>
#include <QPushButton>

class EmojiPopup : public QFrame {
    Q_OBJECT
public:
    explicit EmojiPopup(QWidget *parent=nullptr, int cols=8)
        : QFrame(parent, Qt::Popup | Qt::FramelessWindowHint), m_cols(cols)
    {
        setObjectName("EmojiPopup");
        setStyleSheet(
            "#EmojiPopup { background:#1e1f22; border-radius:10px; }"
            "QPushButton { border:0; padding:0; min-width:36px; min-height:36px; }"
            "QPushButton:hover { background:rgba(255,255,255,0.08); border-radius:8px; }"
            );
        auto *g = new QGridLayout(this);
        g->setContentsMargins(10,10,10,10);
        g->setHorizontalSpacing(6);
        g->setVerticalSpacing(6);

        // pick a color emoji font if available
        QFont f;
#if defined(Q_OS_WIN)
        f.setFamily("Segoe UI Emoji");
#elif defined(Q_OS_MAC)
        f.setFamily("Apple Color Emoji");
#else
        f.setFamily("Noto Color Emoji");
#endif
        f.setPointSize(22);               // <<< bigger size

        const QStringList emojis = {
            "😀","😁","😂","🤣","😊","😍","😎","😭","😡","👍","👎","🙏",
            "👏","🔥","💯","🎉","❤️","💔","😅","🙃","🤔","😴","😇","🤗"
        };

        for (int i=0;i<emojis.size();++i) {
            auto *b = new QPushButton(emojis[i], this);
            b->setFont(f);
            b->setCursor(Qt::PointingHandCursor);
            int r = i / m_cols, c = i % m_cols;      // grid → horizontal first
            g->addWidget(b, r, c);
            connect(b, &QPushButton::clicked, this, [this, e=emojis[i]]{
                emit chosen(e);
                close();
            });
        }
    }

signals:
    void chosen(const QString &emoji);

private:
    int m_cols;
};
