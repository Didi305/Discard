// Lightbox.h
#pragma once
#include <QWidget>
#include <qmediaplayer.h>
#include <qmovie.h>
#include "mediaUtils.h"
class QLabel; class QToolButton; class QVideoWidget; class QMediaPlayer; class QStackedLayout; class QAudioOutput;

class Lightbox : public QWidget {
    Q_OBJECT
public:
    explicit Lightbox(QWidget* parent=nullptr);
    void openOver(QWidget* anchor, const QString& path);
    void setItems(const QStringList& files, int startIndex = 0);    // images or videos

protected:
    bool eventFilter(QObject*, QEvent*) override;
    void keyPressEvent(QKeyEvent*) override;
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void resizeEvent(QResizeEvent*ev) override;
    void showEvent(QShowEvent *ev) override;
    void changeEvent(QEvent* e) override;
    void closeEvent(QCloseEvent*) override {
        if (movie) { movie->stop(); movie->deleteLater(); movie = nullptr; }
        if (player) { player->stop(); player->deleteLater(); player = nullptr; }
    }

private:
    void refit();
    void showImage(const QString& path);
    void showGif(const QString& path);
    void showVideo(const QString& path);
    void showIndex(int i);                  // <— add this
    void loadItem(const QString& path);
    void showCurrent();
    void setImage(const QString& path);
    void setAnimated(const QString& path);
    void setVideo(const QString& path);
    void clearMedia();
    QSize naturalMediaSize();
    QWidget* m_host = nullptr;
    QRect contentRect;
    QWidget*      chrome{};       // top-right controls
    QToolButton*  btnClose{};
    QToolButton*  btnDownload{};
    QStackedLayout* stack{};
    QToolButton*  btnOpen{};
    QToolButton*  btnPrev{};
    QToolButton*  btnNext{};
    QLabel*       img{};          // for static / animated
    QMovie*       movie{};        // animated gif/webp
    QWidget*      holder{};       // centers content
    QMediaPlayer* player{};       // for videos
    QVideoWidget* video{};
    QPixmap     m_imgOrig;        // original still image
    QSize       m_movieNat;       // natural size of animated GIF/WEBP
    QSize       m_videoNat;
    QPixmap  m_srcPx;         // ORIGINAL, never mutated
    bool     m_refitting = false;
    QStringList   items;
    int           index = 0;
    QSize lastVideoSize;
    QAudioOutput* audio{};
};
