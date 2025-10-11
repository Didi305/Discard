#ifndef DIRECTMESSAGE_H
#define DIRECTMESSAGE_H
#include "attachementchip.h"
#include "lightbox.h"
#include <QBuffer>
#include "manager.h"
#include "utils.h"
#include "mediaUtils.h"
#include "emojipopup.h"
#include <QShortcut>
#include <QFileDialog>
#include <QWidget>
#include <qfileinfo.h>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextDocument>
#include <QScrollBar>
#include <QVideoFrame>
#include <QAbstractTextDocumentLayout>
#include <QMenu>
#include <QMimeData>
#include <QMimeDatabase>
#include <QFileIconProvider>
#include <QTimer>

class VideoPlayerDialog;

namespace Ui {
class DirectMessage;
}

class DirectMessage : public QWidget
{
    Q_OBJECT

public:
    explicit DirectMessage(QWidget *parent = nullptr);
    void load(Data d);
    void growToContent();
    bool eventFilter(QObject *object, QEvent *event) override;
    QString htmlEscaped(const QString &text) const;
    void insertConversationHeader(const QString &recipientName, const QPixmap  &avatarOrig);
    void appendMessage(QString name, QString text, QVector<Attachement> attachs, QPixmap avatar, QString time, bool withAvatar);
    void adjustInputHeight();
    void insertEmoji(const QString& e);
    void syncInputDocWidth();
    void autosizeInput();
    void addAttachement(const QString& path);
    void removeAttachement(AttachementChip* chip);
    void updateAttachementsHeight();
    ~DirectMessage();

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent*) override;
private slots:
    void update(Data d);
    void onAddClicked();
    void onGifClicked();
    void onEmojiClicked();
    void onAnchorClicked(const QUrl& url);
private: signals:
    void dragging(int n);
    void notdragging();
    void newMessage(Message& senderData, Data& partner);
private:
    Ui::DirectMessage *ui;
    Manager* m_manager;
    EmojiPopup *m_emojiPopup = nullptr;
    StringContainer messages;
    QVector<AttachementChip*> m_pending;
    QTextDocument *doc;
    qreal dpr;
    int height;
    Data m_partner;
    int conversationID;
    int lastMessageSender = -1;
    bool m_agReady = false;
    int  m_minInputH = 0;
    int  m_maxInputH = 500;
    int m_attachMaxH = 160;
};

#endif // DIRECTMESSAGE_H
