#include "totp.h"
#include <QCoreApplication>
#include <QTextStream>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QTextStream output(stdout);
    // RFC 6238 Appendix B's SHA-1 vectors, reduced to the selected six digits.
    const QString secret = QStringLiteral("GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ");
    struct Vector { qint64 time; const char *code; };
    const Vector vectors[] = {
        {59, "287082"}, {1111111109, "081804"}, {1111111111, "050471"},
        {1234567890, "005924"}, {2000000000, "279037"}, {20000000000LL, "353130"}
    };
    int failures = 0;
    for (const auto &vector : vectors) {
        if (Totp::codeAt(secret, vector.time) != QLatin1String(vector.code) ||
            Totp::matchingCounter(secret, QLatin1String(vector.code), vector.time) != vector.time / 30) {
            output << "FAIL: RFC vector at " << vector.time << '\n';
            ++failures;
        }
    }
    if (Totp::matchingCounter(secret, QStringLiteral("12345"), 59) != -1 ||
        Totp::matchingCounter(secret, QStringLiteral("abcdef"), 59) != -1 ||
        Totp::matchingCounter(QStringLiteral("INVALID"), QStringLiteral("287082"), 59) != -1 ||
        Totp::matchingCounter(secret, QStringLiteral("287082"), 180) != -1 ||
        Totp::matchingCounter(secret, QStringLiteral("287082"), 89) != 1) {
        output << "FAIL: malformed, expired, or adjacent-step verification\n";
        ++failures;
    }
    const auto recovery = Totp::recoveryCode();
    const auto hash = Totp::recoveryHash(recovery);
    if (hash.size() != 64 || Totp::recoveryHash(recovery.toLower().remove('-')) != hash ||
        !Totp::recoveryHash(QStringLiteral("not a recovery code")).isEmpty()) {
        output << "FAIL: recovery-code normalization\n";
        ++failures;
    }
    output << (failures ? "TOTP checks FAILED\n" : "TOTP checks passed\n");
    return failures ? 1 : 0;
}
