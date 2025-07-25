#include "directmessage.h"
#include "ui_directmessage.h"
#include "manager.h"
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextDocument>
#include <QScrollBar>



DirectMessage::DirectMessage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DirectMessage)
    , m_manager(Manager::get())
{
    ui->setupUi(this);


    /* --------------- QTextBrowser instead of ScrollArea ---------------- */
    auto browser = ui->messageView;                              // keep a pointer

    /* --------------- document‑wide stylesheet ---------------- */
    const QString css = QStringLiteral(R"(
.name  { font:650 16px "Segoe UI",sans-serif; color:white; margin-right:20px; }
.time  { color:silver; font:11px "Segoe UI"; }
.body  { font:625 16px "Segoe UI"; color: silver;
         white-space:pre-wrap; word-wrap:break-word; margin-top: 8px;}
.placeholder { width: 56px; height: 56px; display: table-cell; }

table.msg { border-collapse:collapse; margin:4px 0; width:100%; table-layout:fixed; padding-bottom: 25px; background: blue;}
td.ava   { width:32px; padding-right:8px; vertical-align:top; }
td.ava img { width:32px; height:32px; border-radius:16px; }

div.msgB { margin:2px 0 0 40px; }   /* 32 avatar + 8 gap */

/* NEW — make pictures behave */
.body img { display:block; max-width:100%; height:auto; margin-top:4px; }
)");
    ui->messageView->document()->setDefaultStyleSheet(css);

    /* --------------- wrap‑mode so long words break ------------- */
    QTextOption opt = browser->document()->defaultTextOption();
    opt.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    browser->document()->setDefaultTextOption(opt);

    /* --------------- other wiring (input box etc.) ------------- */
    ui->messageInputLine->installEventFilter(this);
    connect(new QShortcut(QKeySequence(Qt::Key_Return), this),
            &QShortcut::activated,
            this, [&]{ ui->messageInputLine->setFocus(); });

    connect(m_manager->getClient(), &Client::newMessage,
            this, &DirectMessage::update);
}

DirectMessage::~DirectMessage() { delete ui; }

/* ------------------------------------------------------------------ */
/*  Helpers                                                           */
/* ------------------------------------------------------------------ */
static QString toHtml(const QString &plain)
{
    return plain.toHtmlEscaped().replace('\n', "<br/>");
}

static QString embedAvatar(QTextDocument *doc, const QPixmap &px)
{
    const QString id = QStringLiteral("ava_%1").arg((qulonglong)px.cacheKey());
    if (!doc->resource(QTextDocument::ImageResource, QUrl(id)).isValid())
        doc->addResource(QTextDocument::ImageResource, QUrl(id), px);
    return id;
}

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
        const QString imgId = embedAvatar(ui->messageView->document(), rounded);
        avatarHtml =
            QStringLiteral(R"(<img src="%1" width="56" height="56"
                                style="border-radius:28px; display:inline-block;">)")
                .arg(imgId);
    }
    const QString headerHtml = QStringLiteral(R"(
<div style="
      padding:12px;
      display:flex;
      align-items:center;
      gap:12px;
      background:transparent;
    ">
  %1
<div style="font:625 20px 'Segoe UI'; color:#fff; margin-top: 30px; margin-bottom: 30px;">
%2
</div>
</div>
<div style="
      padding:8px 12px;
      font:625 16px 'Segoe UI';
      color: white;
      margin-bottom: 20px;
    ">
  This is the start of the conversation
</div>
)").arg(avatarHtml, toHtml(recipientName));

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
    QString avaUrl = embedAvatar(ui->messageView->document(), avatar);

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
)").arg(tableStyle, avatarCell, toHtml(name), toHtml(time), toHtml(text));
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
        auto *ke = static_cast<QKeyEvent*>(ev);
        if (ke->key() == Qt::Key_Return && !ke->modifiers()) {

            QString txt = ui->messageInputLine->toPlainText().trimmed();
            if (txt.isEmpty()) return true;
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
            return true;
        }
    }
    return QWidget::eventFilter(obj, ev);
}
