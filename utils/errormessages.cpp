#include "errormessages.h"
#include "inputlimits.h"
#include <QCoreApplication>
#include <QHash>

QString ErrorMessages::message(const QString &operation, const QString &code, const QString &fallback) {
    // Preserve the existing translation context when extracting Manager's messages.
    if (code == "invalid_mfa_code") {
        return operation == "mfa_setup_confirm"
            ? QCoreApplication::translate("Manager", "That code is incorrect. Check the setup key and your phone's automatic time setting, then try the app's current six-digit code.")
            : QCoreApplication::translate("Manager", "That code is incorrect or has already been used. Try a new authenticator code or an unused recovery code.");
    }

    const QHash<QString, QString> messages{
        {"username_too_long", QCoreApplication::translate("Manager", "Username must be %1 characters or fewer.").arg(InputLimits::MaxUsernameLength)},
        {"too_many_attachments", QCoreApplication::translate("Manager", "You can attach at most eight files to one message.")},
        {"attachment_read_failed", QCoreApplication::translate("Manager", "Could not read a selected attachment. Check that the file exists and can be opened.")},
        {"attachments_too_large", QCoreApplication::translate("Manager", "The selected attachments exceed the total limit of 8 MiB.")},
        {"mfa_not_enabled", QCoreApplication::translate("Manager", "Two-factor authentication is not enabled for this account.")},
        {"mfa_already_enabled", QCoreApplication::translate("Manager", "Two-factor authentication is already enabled. Close this window and open Account security again.")},
        {"mfa_setup_expired", QCoreApplication::translate("Manager", "This setup has expired. Cancel it and start again to get a new setup key.")},
        {"authentication_expired", QCoreApplication::translate("Manager", "This sign-in has expired. Cancel and enter your username and password again.")},
        {"rate_limited", QCoreApplication::translate("Manager", "Too many attempts. Wait a minute before trying again.")},
        {"mfa_locked", QCoreApplication::translate("Manager", "Too many incorrect verification codes. Wait five minutes, then sign in again.")},
        {"authentication_required", QCoreApplication::translate("Manager", "Your session has ended. Sign in again to continue.")},
        {"invalid_credentials", QCoreApplication::translate("Manager", "The username or password is incorrect.")},
        {"invalid_request", QCoreApplication::translate("Manager", "The request was not accepted. Check the entered information and try again.")}
    };
    return messages.value(code, code.isEmpty() ? fallback : code);
}
