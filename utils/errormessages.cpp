#include "errormessages.h"
#include <QCoreApplication>
#include <QHash>

QString ErrorMessages::message(const QString &operation, const QString &code, const QString &fallback) {
    Q_UNUSED(operation)
    // Preserve the existing translation context when extracting Manager's messages.
    const QHash<QString, QString> messages{
        {"too_many_attachments", QCoreApplication::translate("Manager", "You can attach at most eight files to one message.")},
        {"attachment_read_failed", QCoreApplication::translate("Manager", "Could not read a selected attachment. Check that the file exists and can be opened.")},
        {"attachments_too_large", QCoreApplication::translate("Manager", "The selected attachments exceed the total limit of 8 MiB.")},
        {"rate_limited", QCoreApplication::translate("Manager", "Too many attempts. Wait a minute before trying again.")},
        {"authentication_required", QCoreApplication::translate("Manager", "Your session has ended. Sign in again to continue.")},
        {"invalid_credentials", QCoreApplication::translate("Manager", "The username or password is incorrect.")},
        {"invalid_request", QCoreApplication::translate("Manager", "The request was not accepted. Check the entered information and try again.")}
    };
    return messages.value(code, code.isEmpty() ? fallback : code);
}
