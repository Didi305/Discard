// Lightbox.cpp
#include "lightbox.h"
#include <QBoxLayout>
#include <QLabel>
#include <QStackedLayout>
#include <QToolButton>
#include <QStyle>
#include <QFileInfo>
#include <QMovie>
#include <QImageReader>
#include <QDesktopServices>
#include <QMimeDatabase>
#include <QPropertyAnimation>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMouseEvent>
#include <qapplication.h>
#include <qpainter.h>
#include <qvideosink.h>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QScopedValueRollback>
#include "attachmentactions.h"
static bool isVideoFile(const QString& p) {
    QMimeDatabase db; return db.mimeTypeForFile(p).name().startsWith("video/");
}
static bool isAnimCapable(const QString& p) {
    const auto s = QFileInfo(p).suffix().toLower();
    return (s=="gif" || s=="webp" || s=="apng");

}
static QRect fitIntoCenter(const QSize& media, const QSize& view, qreal maxFrac = 0.9)
{
    if (media.isEmpty() || view.isEmpty()) return QRect();

    const QSizeF lim(view.width() * maxFrac, view.height() * maxFrac);
    const qreal sx = lim.width()  / media.width();
    const qreal sy = lim.height() / media.height();
    const qreal s  = std::min<qreal>({sx, sy, 1.0});           // never upscale

    const QSize target( int(std::round(media.width()  * s)),
                       int(std::round(media.height() * s)) );

    const int x = (view.width()  - target.width())  / 2;
    const int y = (view.height() - target.height()) / 2;
    return { QPoint(x, y), target };
}
static QSize scaledFit(QSize src, QSize box)
{
    if (!src.isValid() || !box.isValid()) return {};
    const double sx = double(box.width())  / src.width();
    const double sy = double(box.height()) / src.height();
    const double s  = std::min(sx, sy);
    return QSize(int(src.width()*s), int(src.height()*s));
}

static QSize fitInBox(QSize src, QSize box) {
    if (!src.isValid() || !box.isValid()) return {};
    const double sx = double(box.width())  / src.width();
    const double sy = double(box.height()) / src.height();
    const double s  = std::min(sx, sy);
    return QSize(int(src.width()*s), int(src.height()*s));
}

static QRect centerRect(const QSize& sz, const QSize& box) {
    const int x = (box.width()  - sz.width())  / 2;
    const int y = (box.height() - sz.height()) / 2;
    return { QPoint(x, y), sz };
}

Lightbox::Lightbox(QWidget* parent) : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Widget);
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_TranslucentBackground, false);

    // The holder is fitted manually; a root layout must not override its geometry.
    holder = new QWidget(this);
    holder->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Stack on holder
    stack = new QStackedLayout(holder);
    stack->setContentsMargins(0,0,0,0);

    // Image page
    img = new QLabel;
    img->setScaledContents(false);           // we scale the pixmap ourselves
    img->setAlignment(Qt::AlignCenter);
    img->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    img->setMinimumSize(0,0);
    stack->addWidget(img);

#ifdef QT_MULTIMEDIAWIDGETS_LIB
    video  = new QVideoWidget;
    video->setAspectRatioMode(Qt::KeepAspectRatio);
    video->setMinimumSize(0,0);
    video->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    stack->addWidget(video);

    player = new QMediaPlayer(this);
    audio  = new QAudioOutput(this);     // <—
    player->setVideoOutput(video);
    player->setAudioOutput(audio);       // <—
    audio->setVolume(0.9);               // 0.0 – 1.0
    audio->setMuted(false);
#endif

    // Simple header bar
    chrome = new QWidget(this);
    auto* h = new QHBoxLayout(chrome);
    h->setContentsMargins(0,0,0,0);
    h->setSpacing(8);

    auto mk = [&](QStyle::StandardPixmap sp){
        auto* b = new QToolButton(chrome);
        b->setIcon(style()->standardIcon(sp));
        b->setIconSize(QSize(22,22));
        b->setAutoRaise(true);
        b->setStyleSheet("QToolButton{color:white;}"); // visible on dark
        h->addWidget(b);
        return b;
    };

    btnClose = mk(QStyle::SP_TitleBarCloseButton);
    btnPrev  = mk(QStyle::SP_ArrowBack);
    btnNext  = mk(QStyle::SP_ArrowForward);
    btnDownload = mk(QStyle::SP_DialogSaveButton);
    btnDownload->setToolTip(tr("Save attachment as…"));
    btnClose->setToolTip(tr("Close"));
    btnPrev->setToolTip(tr("Previous"));
    btnNext->setToolTip(tr("Next"));
    btnPrev->hide();
    btnNext->hide();

    chrome->raise();

    // Wire actions
    connect(btnClose, &QToolButton::clicked, this, &QWidget::close);
    connect(btnPrev,  &QToolButton::clicked, this, [this]{ showIndex(index-1); });
    connect(btnNext,  &QToolButton::clicked, this, [this]{ showIndex(index+1); });
    connect(btnDownload, &QToolButton::clicked, this, [this]{
        AttachmentActions::saveAs(this, currentPath, currentName);
    });
}


void Lightbox::openOver(QWidget* host, const QString& path, const QString& displayName)
{
    QWidget* top = host ? host->window() : qApp->activeWindow();
    if (!top) { deleteLater(); return; }
    m_host = top;                               // track the real window

    setParent(top);                             // CHILD, not a tool window
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, true);

    setGeometry(top->rect());                   // local coords work now
    raise();
    show();

    top->installEventFilter(this);              // watch the window itself
    loadItem(path);                            // or setVideo()
    if (!displayName.isEmpty()) currentName = displayName;
    QTimer::singleShot(0, this, &Lightbox::refit);
}


void Lightbox::showImage(const QString& path)
{
    qDebug() << "[lb] showImage" << path;
    if (!img) img = new QLabel(this);
    img->setPixmap(QPixmap(path));
    img->setScaledContents(false);
    img->show();
}

void Lightbox::showGif(const QString& path)
{
    if (!img) img = new QLabel(this);
    movie = new QMovie(path, QByteArray(), this);
    img->setMovie(movie);
    movie->start();
    img->show();
}

void Lightbox::showVideo(const QString& path)
{
    qDebug() << "[lb] showVideo" << path;
    if (!video) video = new QVideoWidget(this);
    if (!player)  player  = new QMediaPlayer(this);
    player->setVideoOutput(video);
    player->setSource(QUrl::fromLocalFile(path));
    video->show();
    player->play();         // you can gate this behind a “play” button if you want
}
void Lightbox::clearMedia() {
    if (movie) { movie->stop(); img->clear(); delete movie; movie=nullptr; }
    if (player) player->stop();
    m_srcPx = QPixmap();
    m_movieNat = QSize();
    img->clear();
}

void Lightbox::setImage(const QString& path)
{
    QImageReader r(path);
    r.setDecideFormatFromContent(true);
    r.setAutoTransform(true);

    const QImage im = r.read();
    if (im.isNull()) {
        img->setText(tr("Cannot open image."));
        img->setPixmap(QPixmap());
        m_srcPx = QPixmap();
        stack->setCurrentWidget(img);
        QTimer::singleShot(0, this, &Lightbox::refit);
        return;
    }

    m_srcPx = QPixmap::fromImage(im);       // keep pristine
    img->setMovie(nullptr);                 // ensure still mode
    img->setPixmap(m_srcPx);                // set once; refit() will scale per size
    img->setScaledContents(false);
    img->setAlignment(Qt::AlignCenter);

    stack->setCurrentWidget(img);
    QTimer::singleShot(0, this, &Lightbox::refit);
}
void Lightbox::setAnimated(const QString& path)
{
    QImageReader reader(path);
    if (!reader.supportsAnimation()) { setImage(path); return; }
    m_movieNat = reader.size();
    movie = new QMovie(path, QByteArray(), this);
    img->setMovie(movie);
    stack->setCurrentWidget(img);
    connect(movie, &QMovie::frameChanged, this, [this](int) {
        if (!m_movieNat.isValid()) m_movieNat = movie->currentImage().size();
        refit();
    });
    movie->start();
    refit();
}
void Lightbox::setVideo(const QString& path)
{
    player->setSource(QUrl::fromLocalFile(path));
    if (audio) { audio->setMuted(false); audio->setVolume(0.9); }
    stack->setCurrentWidget(video);
    player->play();

    // Refits when the real resolution becomes available
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus s){
        if (s == QMediaPlayer::LoadedMedia || s == QMediaPlayer::BufferedMedia)
            refit();
    });
}

void Lightbox::resizeEvent(QResizeEvent*) {
    QTimer::singleShot(0, this, &Lightbox::refit);
}

bool Lightbox::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == m_host) {
        if (ev->type() == QEvent::Resize || ev->type() == QEvent::Move) {
            if (m_host && m_host->window()) {
                setGeometry(m_host->rect());
                refit();
            }
        }
    }
    return QWidget::eventFilter(obj, ev);
}
void Lightbox::showEvent(QShowEvent*) {
    QTimer::singleShot(0, this, &Lightbox::refit);
}

void Lightbox::changeEvent(QEvent* ev) {
    switch (ev->type()) {
    case QEvent::WindowStateChange:
    case QEvent::ScreenChangeInternal:
    case QEvent::DevicePixelRatioChange:
        QTimer::singleShot(0, this, &Lightbox::refit);
        break;
    default: break;
    }
    QWidget::changeEvent(ev);
}

void Lightbox::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    // 0..255 alpha: lower = lighter. Try 160–190.
    p.fillRect(rect(), QColor(0, 0, 0, 210));
}

void Lightbox::mousePressEvent(QMouseEvent* ev)
{
    // Let the button bar receive clicks
    if (chrome->geometry().contains(ev->pos())) {
        QWidget::mousePressEvent(ev);
        return;
    }

    // Click outside the content? Close.
    if (!contentRect.contains(ev->pos())) {
        close();
        return;
    }

    QWidget::mousePressEvent(ev);
}

// Nice-to-have: ESC to close
void Lightbox::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape) { close(); return; }
    QWidget::keyPressEvent(e);
}

void Lightbox::setItems(const QStringList& files, int startIndex)
{
    items = files;
    btnPrev->setVisible(items.size() > 1);
    btnNext->setVisible(items.size() > 1);
    index = qBound(0, startIndex, files.size() - 1);
}

void Lightbox::showIndex(int i)
{
    if (items.isEmpty()) return;
    // wrap around
    index = (i % items.size() + items.size()) % items.size();
    loadItem(items.at(index));
}

void Lightbox::loadItem(const QString& path)
{
    QMimeDatabase db;
    clearMedia();
    currentPath = path;
    currentName = QFileInfo(path).fileName();
    const QString mt = db.mimeTypeForFile(path).name();

    if (mt.startsWith("image/")) {
        if (isAnimCapable(path))  setAnimated(path);   // GIF/WEBP/APNG -> movie
        else                      setImage(path);      // PNG/JPG/…
        return;
    }
#ifdef QT_MULTIMEDIAWIDGETS_LIB
    if (mt.startsWith("video/")) {
        setVideo(path);
        return;
    }
#endif
    // fallback: show file name
    img->setText(QFileInfo(path).fileName());
    img->setAlignment(Qt::AlignCenter);
    stack->setCurrentWidget(img);
    contentRect = {};
}

void Lightbox::refit()
{
    if (m_refitting) return;
    QScopedValueRollback<bool> guard(m_refitting, true);
    chrome->adjustSize();
    chrome->move(qMax(8, width() - chrome->width() - 16), 12);
    chrome->raise();

    const QRect viewport = rect().adjusted(24, qMax(56, chrome->height() + 24), -24, -24);
    if (!viewport.isValid()) return;
    QSize natural;
    if (stack->currentWidget() == img)
        natural = movie ? m_movieNat : m_srcPx.deviceIndependentSize().toSize();
#ifdef QT_MULTIMEDIAWIDGETS_LIB
    else if (stack->currentWidget() == video) {
        natural = player->metaData().value(QMediaMetaData::Resolution).toSize();
        if (!natural.isValid()) natural = QSize(1280,720);
    }
#endif
    QRect fitted = natural.isValid() ? fitIntoCenter(natural, viewport.size(), 1.0) : QRect(QPoint(), viewport.size());
    fitted.translate(viewport.topLeft());
    holder->setGeometry(fitted);
    stack->setGeometry(holder->rect());
    contentRect = fitted;

    if (stack->currentWidget() == img) {
        if (movie) {
            if (movie->scaledSize() != fitted.size()) movie->setScaledSize(fitted.size());
        } else if (!m_srcPx.isNull()) {
            const qreal dpr = devicePixelRatioF();
            QPixmap scaled = m_srcPx.scaled((QSizeF(fitted.size()) * dpr).toSize(),
                Qt::KeepAspectRatio, Qt::SmoothTransformation);
            scaled.setDevicePixelRatio(dpr);
            img->setPixmap(scaled);
        }
    }
}