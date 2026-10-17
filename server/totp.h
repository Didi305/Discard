#pragma once
#include <QString>
#include <QtGlobal>

// RFC 6238: HMAC-SHA1, six digits, a 30-second time step.
// Cryptographic primitives and random bytes come from Qt.
namespace Totp {
QString newSecret();
QString provisioningUri(const QString &secret, const QString &account);
QString codeAt(const QString &secret, qint64 unixSeconds);
qint64 matchingCounter(const QString &secret, const QString &code, qint64 unixSeconds);
QString recoveryCode();
QString recoveryHash(const QString &code);
}
