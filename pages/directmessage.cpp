#include "directmessage.h"
#include "ui_directmessage.h"

DirectMessage::DirectMessage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DirectMessage)
    , m_manager(Manager::get())
{
    ui->setupUi(this);
    ui->messageView->setOpenExternalLinks(true);
    setAcceptDrops(true);
    ui->messageView->viewport()->setAcceptDrops(false);
    ui->messageInputLine->viewport()->setAcceptDrops(false);
    qDebug() << QImageReader::supportedImageFormats();
    Q_ASSERT(ui->scrollArea->widget() == ui->inputAttachments);
    if (!ui->inputAttachments->layout()) {                 // ensure it has a layout
        auto row = new QHBoxLayout(ui->inputAttachments);
        row->setContentsMargins(8,8,8,8);
        row->setSpacing(8);
        row->setAlignment(Qt::AlignLeft);
        ui->inputAttachments->setLayout(row);
    }
    ui->scrollArea->setVisible(false);
    ui->inputAttachments->installEventFilter(this);
    ui->scrollArea->viewport()->installEventFilter(this);

    auto browser = ui->messageView;
    QFile f(path + "/styles/messageView.css");
    if (f.open(QIODevice::ReadOnly)) {
        const QString css = QString::fromUtf8(f.readAll());
        ui->messageView->document()->setDefaultStyleSheet(css);
    }

    /* --------------- wrap‑mode so long words break ------------- */
    QTextOption opt = browser->document()->defaultTextOption();
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    browser->document()->setDefaultTextOption(opt);

    /* --------------- other wiring (input box etc.) ------------- */
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

    auto te = ui->messageInputLine;


    // run only after the widget is shown
    QTimer::singleShot(0, this, [this]{
        m_agReady = true;
        autosizeInput();
    });

    // fire when the *document size changes* (i.e., reflow/wrap or content edit)
    connect(te->document()->documentLayout(),
            &QAbstractTextDocumentLayout::documentSizeChanged,
            this, [this](const QSizeF&){ if (m_agReady) autosizeInput(); });

    // keep textWidth == viewport width so wrapping is accurate
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
    // 1) Clear the old document.
    ui->messageView->clear();

    // 2) Build the HTML for the avatar (img or placeholder div).
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

    // 4) Push it into the view.
    ui->messageView->append(headerHtml);
}
void DirectMessage::appendMessage(QString name,
                                  QString text,
                                  QPixmap avatarOrig,
                                  QString time,
                                  bool withAvatar)
{
    // 1 — Make your avatar rounded‑48px as before
    const QPixmap avatar = Utils::makeRoundedAvatar(avatarOrig, 48);
    QString avaUrl = Utils::embedAvatar(ui->messageView->document(), avatar);

    // 2 — Build the avatar/placeholder <td> with HTML width/height + valign:
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
    const QString bodyHtml    = Utils::wrapEmojisForHtml(bodyEscaped); // then wrap
    const QString html = QStringLiteral(R"(
<table class="msg" width="100%%" cellspacing="0" cellpadding="0" %1>
  <tr>
    %2
    <td class="cont" style="padding-left:8px;">
      <div class="name">%3 <span class="time">%4</span></div>
      <div class="body">%5</div>
    </td>
  </tr>
</table>
)").arg(tableStyle, avatarCell, Utils::toHtml(name), Utils::toHtml(time), bodyHtml);
    ui->messageView->append(html);
}

/* ------------------------------------------------------------------ */
/*  Public API                                                        */
/* ------------------------------------------------------------------ */
void DirectMessage::load(Data partner)
{
    // clear old document
    ui->messageView->clear();
    insertConversationHeader(partner.displayName, partner.avatar);

    int ownID   = m_manager->getClient()->getClientID();
    conversaionID   = m_manager->checkConversation(ownID, partner.user_id);
    receiverID      = partner.user_id;
    messages        = m_manager->fetchMessages(conversaionID);

    for (auto &m : messages.get<BySeq>()) {
        Data d = (m.senderID == ownID)
        ? m_manager->getClient()->getClientData()
        : partner;
        d.message = m;                           // embed message into Data
        bool first = (m.senderID != lastMessageSender);
        appendMessage(d.displayName, m.body, d.avatar, m.sentAt, first);
        lastMessageSender = m.senderID;
    }
}

void DirectMessage::update(Data d)
{
    appendMessage(d.displayName, d.message.body, d.avatar, d.message.sentAt, lastMessageSender != d.user_id);
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
            if (txt.isEmpty() && ui->inputAttachments->layout()->isEmpty()) return true;
            ui->messageInputLine->clear();

            Data self = m_manager->getClient()->getClientData();
            self.message = { txt, self.user_id,
                            QDateTime::currentDateTimeUtc().toString() };

            appendMessage(self.displayName, self.message.body, self.avatar, self.message.sentAt, lastMessageSender != self.user_id);
            lastMessageSender = self.user_id;
            messages.push_back(self.message);

            // send over network
            self.user_id = receiverID;
            m_manager->getClient()->sendMessage(self, "message");
            m_manager->postMessage(txt, conversaionID,
                                   m_manager->getClient()->getClientID());
            v->setValue(v->maximum());
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

    for (const QString &p : paths) handleAttachment(p);
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

    // align popup’s bottom-left to the button’s top-left (above the button)
    const QPoint btnTL = ui->emotesButton->mapToGlobal(QPoint(0, 0));
    QPoint pos = btnTL - QPoint(0, m_emojiPopup->height() + 4);

    // keep it on screen; if there’s no room above, show below
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
    emojiFmt.setFontPointSize(20);                 // ↑ make it bigger
#if defined(Q_OS_WIN)
    emojiFmt.setFontFamily("Segoe UI Emoji");      // color emoji font
#elif defined(Q_OS_MAC)
    emojiFmt.setFontFamily("Apple Color Emoji");
#else
    emojiFmt.setFontFamily("Noto Color Emoji");
#endif

    cur.insertText(e, emojiFmt);                   // insert big emoji
    cur.setCharFormat(prevFmt);                    // restore for next typing
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
        if (!p.isEmpty()) addAttachment(p);
    }
    // TODO (next step): put files into your “pending attachments” strip
    // for (const QUrl& u : e->mimeData()->urls()) handleAttachment(u.toLocalFile());
}
void DirectMessage::dragLeaveEvent(QDragLeaveEvent*)
{
    emit notdragging();
}

void DirectMessage::addAttachment(const QString& path)
{
    if (path.isEmpty()) return;
    qDebug() << "tried adding an attachment";
    auto chip = new AttachmentChip(path, ui->inputAttachments);
    connect(chip, &AttachmentChip::removeMe, this, &DirectMessage::removeAttachment);
    chip->setMinimumHeight(52);
    ui->inputAttachments->layout()->addWidget(chip);
    m_pending.push_back(chip);

    ui->scrollArea->setVisible(true);
    ui->inputAttachments->layout()->activate();           // refresh sizeHint
    updateAttachmentsHeight();                        // grow strip as needed
}

void DirectMessage::removeAttachment(AttachmentChip* chip)
{
    if (!chip) return;
    m_pending.erase(std::remove(m_pending.begin(), m_pending.end(), chip), m_pending.end());
    ui->inputAttachments->layout()->removeWidget(chip);
    chip->deleteLater();
    updateAttachmentsHeight();                         // shrink when empty
}

void DirectMessage::handleAttachment(const QString &path)
{
    if (path.isEmpty()) return;
    qDebug() << "handle called";
    QFileInfo fi(path);
    const QString ext = fi.suffix().toLower();

    if (QStringList{"png","jpg","jpeg","webp","gif"}.contains(ext)) {
        appendImageToView(path);                 // GIFs animate automatically
        // TODO: send with your Manager here
    } else {
        appendFileChipToView(fi);
        // TODO: upload/send file
    }
}


void DirectMessage::appendImageToView(const QString &filePath)
{
    // Use a file URL so GIFs animate in QTextBrowser
    const QString url = QUrl::fromLocalFile(filePath).toString();
    const QString html = QStringLiteral(
                             R"(<div class="body" style="margin:6px 0;">
             <img src="%1" style="max-width:100%%;height:auto;border-radius:8px;"/>
           </div>)").arg(url);
    ui->messageView->append(html);
}


void DirectMessage::appendFileChipToView(const QFileInfo &fi)
{
    if (m_pending.empty()) return;
    QString initialHTML = QStringLiteral(R"(
<table class="attachments" width="100%%" cellspacing="0" cellpadding="0">
  <tr>
    <td class="content" style="padding-left:8px;">
)");

    for (auto *attach : m_pending){
        const QString url  = QUrl::fromLocalFile(attach->path()).toString();
        initialHTML += QStringLiteral(R"(       <a href="%1">twimel.txt</a>)").arg(url) + "\n";
    }
    const QString finalHTML = QStringLiteral(R"(
    </td>
  </tr>
</table>)");
    auto html = initialHTML + finalHTML;
    qDebug().noquote() << "HTML is: " << html;
    ui->messageView->append(html);
}

void DirectMessage::updateAttachmentsHeight()
{
    auto sa   = ui->scrollArea;
    auto cont = ui->inputAttachments;

    int contentH = 0;
    if (auto lay = cont->layout()) {
        lay->activate();                                  // ensure up-to-date
        contentH = lay->sizeHint().height();
    } else {
        contentH = cont->sizeHint().height();
    }

    if (contentH <= 0) { ui->scrollArea->setVisible(false); return; }
    sa->setVisible(true);
}
