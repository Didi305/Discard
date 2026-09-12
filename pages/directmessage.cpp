#include "directmessage.h"
#include "ui_directmessage.h"
#include <QDesktopServices>
#include <qurlquery.h>
DirectMessage::DirectMessage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DirectMessage)
    , m_manager(Manager::get())
    , dpr(devicePixelRatioF())
{
    ui->setupUi(this);
    ui->messageView->setOpenLinks(false);
    ui->messageView->setOpenExternalLinks(false);
    doc = ui->messageView->document();
    setAcceptDrops(true);
    ui->messageView->viewport()->setAcceptDrops(false);
    ui->messageInputLine->viewport()->setAcceptDrops(false);
    Q_ASSERT(ui->scrollArea->widget() == ui->inputAttachements);
    if (!ui->inputAttachements->layout()) {
        auto row = new QHBoxLayout(ui->inputAttachements);
        row->setContentsMargins(8,8,8,8);
        row->setSpacing(8);
        row->setAlignment(Qt::AlignLeft);
        ui->inputAttachements->setLayout(row);
    }
    ui->scrollArea->setVisible(false);
    ui->inputAttachements->installEventFilter(this);
    ui->scrollArea->viewport()->installEventFilter(this);

    QString css = Utils::readText(":/styles/messageView.css", "messageView.css");
    ui->messageView->document()->setDefaultStyleSheet(css);


    QTextOption opt = doc->defaultTextOption();
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    doc->setDefaultTextOption(opt);


    ui->messageInputLine->installEventFilter(this);
    ui->messageInputLine->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    connect(new QShortcut(QKeySequence(Qt::Key_Return), this),
            &QShortcut::activated,
            this, [&]{ ui->messageInputLine->setFocus(); });

    connect(m_manager->getClient(), &Client::newMessage,
            this, &DirectMessage::update);
    connect(ui->addAttachButton, &QToolButton::clicked, this, &DirectMessage::onAddClicked);
    connect(ui->emotesButton, &QToolButton::clicked, this, &DirectMessage::onEmojiClicked);
    connect(ui->GIFButton,   &QPushButton::clicked, this, &DirectMessage::onGifClicked);
    connect(ui->messageView, &QTextBrowser::anchorClicked,this,  &DirectMessage::onAnchorClicked);
    auto te = ui->messageInputLine;



    QTimer::singleShot(0, this, [this]{
        m_agReady = true;
        autosizeInput();
    });


    connect(te->document()->documentLayout(),
            &QAbstractTextDocumentLayout::documentSizeChanged,
            this, [this](const QSizeF&){ if (m_agReady) autosizeInput(); });


    te->viewport()->installEventFilter(this);
    ui->rightButtonsLayout->setAlignment(Qt::AlignTop);
    ui->leftButtonLayout->setAlignment(Qt::AlignTop);
    QIcon emo;
    emo.addFile(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Discard/resources/icons/emoteee.svg",  QSize(), QIcon::Normal);
    emo.addFile(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/C++/Discard/resources/icons/emote_active.svg", QSize(), QIcon::Active);
    ui->emotesButton->setIcon(emo);
}

DirectMessage::~DirectMessage() { delete ui; }


void DirectMessage::insertConversationHeader(const QString &recipientName,
                                             const QPixmap  &avatarOrig)
{

    ui->messageView->clear();


    QString avatarHtml;
    if (!avatarOrig.isNull()) {
        // round it:
        const QPixmap rounded = Utils::makeRoundedAvatar(avatarOrig, 56);
        // embed into the document’s resource cache:
        const QString imgId = Utils::embedAvatar(ui->messageView->document(), rounded);
        avatarHtml =
            QStringLiteral(R"(<img src="%1" width="56" height="56"
                                style="border-radius:28px; display:inline-block;">)")
                .arg(imgId);
    }
    const QString headerHtml = QStringLiteral(R"(
<div id = "avatar">
  %1
<div style="font:625 20px 'Segoe UI'; color:#fff; margin-top: 30px; margin-bottom: 30px;">
%2
</div>
</div>
<div id = "recipient">
  This is the start of the conversation
</div>
)").arg(avatarHtml, Utils::toHtml(recipientName));


    ui->messageView->append(headerHtml);
}

void DirectMessage::appendMessage(QString name, QString text, QVector<Attachement> attachs, QPixmap avatarOrig, QString time, bool withAvatar)
{

    const QPixmap avatar = Utils::makeRoundedAvatar(avatarOrig, 48);
    QString avaUrl = Utils::embedAvatar(ui->messageView->document(), avatar);


    QString avatarCell =  QString("<td >""<img src=\"%1\"""</td>").arg(avaUrl);

    if(!withAvatar){
        avatarCell = QString("<td width=\"48\" height=\"0\" valign=\"top\"></td>");
        name = "";
        time = "";
    }
    const QString tableStyle = withAvatar
                                   ? "style=\"margin-top:24px;\""
                                   : QString();
    const QString bodyEscaped = Utils::toHtml(text);      // escape first
    const QString bodyHtml    = (!text.isEmpty()) ? Utils::wrapEmojisForHtml(bodyEscaped) : ""; // then wrap
    std::println("attach paths: ", attachs[0].)
    const QString attHtml = mediaUtils::attachementsHtmlFor(attachs, doc, dpr);
    const QString msgHtml = QString(R"(
<table class="msg" width="100%%" cellspacing="0" cellpadding="0">
  <tr>%1
    <td class="cont" style="padding-left:8px;">
      <div class="name">%2 <span class="time">%3</span></div>
      <div class="body">%4</div>
      %5
    </td>
  </tr>
</table>)").arg(avatarCell, Utils::toHtml(name), Utils::toHtml(time), bodyHtml, attHtml);
    ui->messageView->append(msgHtml);
}


void DirectMessage::load(Data partner)
{
    lastMessageSender = -1;
    ui->messageView->clear();
    insertConversationHeader(partner.displayName, partner.avatar);

    int ownID   = m_manager->getClient()->getClientID();
    conversationID   = m_manager->checkConversation(ownID, partner.user_id);
    m_partner      = partner;
    qDebug() << "conv ID opened: " << conversationID;
    qDebug() << "partner ID opened: " << partner.user_id;
    messages        = m_manager->fetchMessages(conversationID);
    for (auto &m : messages.get<BySeq>()) {
        Data d = (m.senderID == ownID)
        ? m_manager->getClient()->getClientData()
        : m_partner;
        d.message = m;                           // embed message into Data
        bool first = (m.senderID != lastMessageSender);
        appendMessage(d.displayName, m.body, m.attachs, d.avatar, m.sentAt, first);
        lastMessageSender = m.senderID;
    }
}

void DirectMessage::update(Data d)
{
    appendMessage(d.displayName, d.message.body, d.message.attachs, d.avatar, d.message.sentAt, lastMessageSender != d.user_id);
    lastMessageSender = d.user_id;
    messages.push_back(d.message);
}

/* ------------------------------------------------------------------ */
/*  Enter‑to‑send                                                     */
/* ------------------------------------------------------------------ */
bool DirectMessage::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == ui->messageInputLine && ev->type() == QEvent::KeyPress) {
        auto v = ui->messageView->verticalScrollBar();
        auto *ke = static_cast<QKeyEvent*>(ev);
        if (ke->key() == Qt::Key_Return && !ke->modifiers()) {

            QString txt = ui->messageInputLine->toPlainText().trimmed();
            if (txt.isEmpty() && m_pending.empty()) return true;
            ui->messageInputLine->clear();

            Data self = m_manager->getClient()->getClientData();
            self.message = { txt, self.user_id, conversationID, mediaUtils::chipToData(m_pending), QDateTime::currentDateTimeUtc().toString() };
            m_manager->getClient()->setClientData(self);

            appendMessage(self.displayName, self.message.body, self.message.attachs, self.avatar, self.message.sentAt, lastMessageSender != self.user_id);
            lastMessageSender = self.user_id;
            messages.push_back(self.message);

            auto message_id = m_manager->postMessage(txt, conversationID, m_manager->getClient()->getClientID(), m_partner.user_id);
            QVector<Attachement> sharedAttachments;
            for (auto *c : m_pending){
                Attachement stored = m_manager->postAttachement(c->info(), message_id);
                if (!stored.path.isEmpty()) sharedAttachments.push_back(stored);
            }

            // Only portable database references go over the socket. The receiver
            // resolves them to files in its own local cache.
            Data outbound = self;
            outbound.message.attachs = sharedAttachments;
            outbound.user_id = m_partner.user_id;
            m_manager->getClient()->sendMessage(outbound, "message");

            emit newMessage(self.message, m_partner);
            v->setValue(v->maximum());
            ui->scrollArea->setVisible(false);
            if (!m_pending.empty()) Utils::deleteChildren(ui->inputAttachements->layout());
            m_pending.clear();
            return true;
        }
    }
    if (obj == ui->messageInputLine->viewport() && ev->type() == QEvent::Resize) {
        autosizeInput();       // your height adjust
        return false;          // don't consume resize
    }
    return QWidget::eventFilter(obj, ev);
}


void DirectMessage::autosizeInput()
{
    auto te = ui->messageInputLine;

    if (!m_minInputH) { // compute once
        m_minInputH = qMax(te->fontMetrics().lineSpacing() + 2*te->frameWidth(),
                           te->minimumHeight());
        if (te->maximumHeight() > 0 && te->maximumHeight() < 16777215)
            m_maxInputH = te->maximumHeight();
    }

    const int frame = 2 * te->frameWidth();
    int h = int(std::ceil(te->document()->size().height())) + frame;
    h = std::clamp(h, m_minInputH, m_maxInputH);

    te->setFixedHeight(h);
    te->setVerticalScrollBarPolicy(h >= m_maxInputH
                                       ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
}


void DirectMessage::onAddClicked()
{
    QMenu m(this);
    QAction *pickImages = m.addAction("Image / GIF…");
    QAction *pickFiles  = m.addAction("File…");

    QAction *act = m.exec(ui->addAttachButton->mapToGlobal(
        QPoint(0, ui->addAttachButton->height())));
    if (!act) return;

    QStringList paths;
    if (act == pickImages) {
        paths = QFileDialog::getOpenFileNames(
            this, "Choose images",
            {}, "Images (*.png *.jpg *.jpeg *.webp *.gif);;All files (*.*)");
    } else {
        paths = QFileDialog::getOpenFileNames(this, "Choose files");
    }

    for (const QString &p : paths) addAttachement(p);
}

void DirectMessage::onEmojiClicked()
{
    if (!m_emojiPopup) {
        m_emojiPopup = new EmojiPopup(this, /*cols*/ 10);
        connect(m_emojiPopup, &EmojiPopup::chosen, this, &DirectMessage::insertEmoji);
    }

    m_emojiPopup->adjustSize();
    const QRect scr = QGuiApplication::screenAt(QCursor::pos())
                          ->availableGeometry();


    const QPoint btnTL = ui->emotesButton->mapToGlobal(QPoint(0, 0));
    QPoint pos = btnTL - QPoint(0, m_emojiPopup->height() + 4);


    pos.setX(std::clamp(pos.x(), scr.left(),
                        scr.right() - m_emojiPopup->width()));
    if (pos.y() < scr.top())
        pos = btnTL + QPoint(0, ui->emotesButton->height() + 4);

    m_emojiPopup->move(pos);
    m_emojiPopup->show();
}
void DirectMessage::onGifClicked(){

}

void DirectMessage::insertEmoji(const QString& e)
{
    auto te  = ui->messageInputLine;
    QTextCursor cur = te->textCursor();
    const QTextCharFormat prevFmt = cur.charFormat();

    QTextCharFormat emojiFmt = prevFmt;
    emojiFmt.setFontPointSize(20);
#if defined(Q_OS_WIN)
    emojiFmt.setFontFamily("Segoe UI Emoji");
#elif defined(Q_OS_MAC)
    emojiFmt.setFontFamily("Apple Color Emoji");
#else
    emojiFmt.setFontFamily("Noto Color Emoji");
#endif

    cur.insertText(e, emojiFmt);
    cur.setCharFormat(prevFmt);
    te->setTextCursor(cur);
    te->setFocus();
}

void DirectMessage::dragEnterEvent(QDragEnterEvent* e)
{
    if (!e->mimeData()->hasUrls()) { e->ignore(); return; }
    const auto urls = e->mimeData()->urls();
    bool anyLocal = std::any_of(urls.begin(), urls.end(),
                                [](const QUrl& u){ return u.isLocalFile(); });
    if (!anyLocal) { e->ignore(); return; }

    e->setDropAction(Qt::CopyAction);
    e->accept();

    const int n = urls.size();
    emit dragging(n);
}

void DirectMessage::dropEvent(QDropEvent* e)
{
    emit notdragging();
    if (!e->mimeData()->hasUrls()) { e->ignore(); return; }
    e->acceptProposedAction();

    for (const QUrl& u : e->mimeData()->urls()) {
        const QString p = u.toLocalFile();
        if (!p.isEmpty()) addAttachement(p);
    }

}
void DirectMessage::dragLeaveEvent(QDragLeaveEvent*)
{
    emit notdragging();
}

void DirectMessage::addAttachement(const QString& path)
{
    if (path.isEmpty()) return;
    qDebug() << "tried adding an Attachement";
    auto chip = new AttachementChip(path, ui->inputAttachements);
    connect(chip, &AttachementChip::removeMe, this, &DirectMessage::removeAttachement);
    chip->setMinimumHeight(52);
    ui->inputAttachements->layout()->addWidget(chip);
    m_pending.push_back(chip);

    ui->scrollArea->setVisible(true);
    ui->inputAttachements->layout()->activate();
    updateAttachementsHeight();
}

void DirectMessage::removeAttachement(AttachementChip* chip)
{
    if (!chip) return;
    m_pending.erase(std::remove(m_pending.begin(), m_pending.end(), chip), m_pending.end());
    ui->inputAttachements->layout()->removeWidget(chip);
    chip->deleteLater();
    if(m_pending.size() == 0) ui->scrollArea->setVisible(false);
}


void DirectMessage::onAnchorClicked(const QUrl& url)
{
    qDebug() << "[anchor] url =" << url;
    ui->messageView->clearFocus();
    if (url.scheme() == "att" && url.host() == "open") {
        QString path;

        // Prefer query ?path=...
        if (url.hasQuery()) {
            QUrlQuery q(url);
            path = QUrl::fromPercentEncoding(q.queryItemValue("path").toUtf8());
        } else {
            // Fallback for att://open/C:/... form
            path = url.path();
#ifdef Q_OS_WIN
            if (path.startsWith('/')) path.remove(0, 1);
#endif
        }
        qDebug() << "[anchor] decoded path =" << path;

        if (!path.isEmpty()) {
            auto* lb = new Lightbox;
            lb->openOver(this, path);
        }
        return;
    }

    // For any other link, let OS handle it
    QDesktopServices::openUrl(url);
}
void DirectMessage::updateAttachementsHeight()
{
    auto sa   = ui->scrollArea;
    auto cont = ui->inputAttachements;

    int contentH = 0;
    if (auto lay = cont->layout()) {
        lay->activate();
        contentH = lay->sizeHint().height();
    } else {
        contentH = cont->sizeHint().height();
    }

    if (contentH <= 0) { ui->scrollArea->setVisible(false); return; }
    sa->setVisible(true);
}
