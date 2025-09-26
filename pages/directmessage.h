#ifndef DIRECTMESSAGE_H
#define DIRECTMESSAGE_H
#include "attachmentchip.h"
#include "dragbanner.h"
#include "manager.h"
#include "utils.h"
#include "emojipopup.h"
#include <QShortcut>
#include <QFileDialog>
#include <QWidget>
#include <qfileinfo.h>
#include <QTextBrowser>
#include <QTextCursor>
#include <QTextDocument>
#include <QScrollBar>
#include <QAbstractTextDocumentLayout>
#include <QMenu>
#include <QMimeData>
#include <QTimer>
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
    void appendMessage(QString name,
                       QString text,
                       QPixmap avatar,
                       QString time,
                       bool withAvatar);
    void adjustInputHeight();
    void insertEmoji(const QString& e);
    void syncInputDocWidth();
    void autosizeInput();
    void handleAttachment(const QString &path);
    void addAttachment(const QString& path);
    void removeAttachment(AttachmentChip* chip);
    void updateAttachmentsHeight();
    void appendImageToView(const QString &filePath);
    void appendFileChipToView(const QFileInfo &fi);
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
private: signals:
    void dragging(int n);
    void notdragging();
private:
    Ui::DirectMessage *ui;
    Manager* m_manager;
    EmojiPopup *m_emojiPopup = nullptr;
    StringContainer messages;
    QVector<AttachmentChip*> m_pending;
    int height;
    int receiverID;
    int conversaionID;
    int lastMessageSender = -1;
    bool m_agReady = false;
    int  m_minInputH = 0;
    int  m_maxInputH = 500;
    int m_attachMaxH = 160;
};

#endif // DIRECTMESSAGE_H
