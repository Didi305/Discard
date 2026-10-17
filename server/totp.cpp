#include "totp.h"
#include <QCryptographicHash>
#include <QMessageAuthenticationCode>
#include <QRandomGenerator>
#include <QUrl>

namespace {
constexpr char Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

QByteArray randomBytes(int size) {
    QByteArray bytes(size, '\0');
    for (int i = 0; i < size;) {
        quint64 word = QRandomGenerator::system()->generate64();
        for (int j = 0; j < 8 && i < size; ++j, ++i) {
            bytes[i] = char(word & 0xff);
            word >>= 8;
        }
    }
    return bytes;
}

QString base32(const QByteArray &bytes) {
    QString result;
    quint32 buffer = 0;
    int bits = 0;
    for (unsigned char byte : bytes) {
        buffer = (buffer << 8) | byte;
        bits += 8;
        while (bits >= 5) {
            bits -= 5;
            result += QLatin1Char(Alphabet[(buffer >> bits) & 31]);
        }
    }
    if (bits) result += QLatin1Char(Alphabet[(buffer << (5 - bits)) & 31]);
    return result;
}

QByteArray decodeSecret(const QString &secret) {
    // Every generated seed is 160 bits, represented by exactly 32 characters.
    if (secret.size() != 32) return {};
    QByteArray result;
    quint32 buffer = 0;
    int bits = 0;
    for (QChar character : secret) {
        const ushort c = character.unicode();
        const int value = (c >= 'A' && c <= 'Z') ? c - 'A' :
                          (c >= '2' && c <= '7') ? c - '2' + 26 : -1;
        if (value < 0) return {};
        buffer = (buffer << 5) | quint32(value);
        bits += 5;
        if (bits >= 8) {
            bits -= 8;
            result.append(char((buffer >> bits) & 255));
        }
    }
    return result;
}

QString codeForCounter(const QByteArray &key, qint64 counter) {
    if (key.size() != 20 || counter < 0) return {};
    QByteArray message(8, '\0');
    quint64 value = quint64(counter);
    for (int i = 7; i >= 0; --i) {
        message[i] = char(value & 255);
        value >>= 8;
    }
    const auto digest = QMessageAuthenticationCode::hash(message, key, QCryptographicHash::Sha1);
    const int offset = static_cast<unsigned char>(digest.at(19)) & 15;
    quint32 truncated = static_cast<unsigned char>(digest.at(offset)) & 127;
    for (int i = 1; i < 4; ++i)
        truncated = (truncated << 8) | static_cast<unsigned char>(digest.at(offset + i));
    return QString::number(truncated % 1000000).rightJustified(6, QLatin1Char('0'));
}

bool equalCode(const QString &left, const QString &right) {
    if (left.size() != 6 || right.size() != 6) return false;
    unsigned int difference = 0;
    for (int i = 0; i < 6; ++i)
        difference |= left.at(i).unicode() ^ right.at(i).unicode();
    return difference == 0;
}
}

QString Totp::newSecret() { return base32(randomBytes(20)); }

QString Totp::provisioningUri(const QString &secret, const QString &account) {
    return QStringLiteral("otpauth://totp/%1?secret=%2&issuer=Discard&algorithm=SHA1&digits=6&period=30")
        .arg(QString::fromLatin1(QUrl::toPercentEncoding("Discard:" + account)), secret);
}

QString Totp::codeAt(const QString &secret, qint64 unixSeconds) {
    if (unixSeconds < 0) return {};
    return codeForCounter(decodeSecret(secret), unixSeconds / 30);
}

qint64 Totp::matchingCounter(const QString &secret, const QString &code, qint64 unixSeconds) {
    if (unixSeconds < 0 || code.size() != 6) return -1;
    for (QChar c : code) if (c < QLatin1Char('0') || c > QLatin1Char('9')) return -1;
    const auto key = decodeSecret(secret);
    const qint64 current = unixSeconds / 30;
    qint64 matched = -1;
    for (int drift = -1; drift <= 1; ++drift) {
        const qint64 counter = current + drift;
        if (equalCode(codeForCounter(key, counter), code)) matched = counter;
    }
    return matched;
}

QString Totp::recoveryCode() {
    const auto hex = QString::fromLatin1(randomBytes(16).toHex().toUpper());
    QString result;
    for (int i = 0; i < hex.size(); i += 4) {
        if (i) result += QLatin1Char('-');
        result += hex.mid(i, 4);
    }
    return result;
}

QString Totp::recoveryHash(const QString &code) {
    QString normalized;
    for (QChar c : code) {
        if (c.isSpace() || c == QLatin1Char('-')) continue;
        normalized += c.toUpper();
    }
    if (normalized.size() != 32) return {};
    for (QChar c : normalized)
        if (!((c >= QLatin1Char('0') && c <= QLatin1Char('9')) ||
              (c >= QLatin1Char('A') && c <= QLatin1Char('F')))) return {};
    return QString::fromLatin1(QCryptographicHash::hash(normalized.toLatin1(), QCryptographicHash::Sha256).toHex());
}
