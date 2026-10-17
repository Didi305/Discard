#include "authenticatorqr.h"
#include "third_party/qrcodegen/qrcodegen.hpp"
#include <QImage>
#include <QPainter>
#include <QtMath>
#include <exception>

QPixmap AuthenticatorQr::render(const QString &uri, qreal devicePixelRatio) {
    if (!uri.startsWith("otpauth://totp/") || uri.contains(QChar::Null) || uri.size() > 4096 ||
        !qIsFinite(devicePixelRatio) || devicePixelRatio <= 0) {
        return {};
    }

    try {
        const auto payload = uri.toUtf8();
        const auto qr = qrcodegen::QrCode::encodeText(payload.constData(),
                                                     qrcodegen::QrCode::Ecc::MEDIUM);
        constexpr int QuietZoneModules = 4;
        constexpr int ReferenceDisplaySize = 220;
        constexpr qreal DisplaySizeFactor = 0.7;
        const int moduleCount = qr.getSize() + QuietZoneModules * 2;
        const int referenceScale = qMax(1, qCeil(ReferenceDisplaySize * devicePixelRatio / moduleCount));
        const int scale = qMax(1, qRound(referenceScale * DisplaySizeFactor));
        const int imageSize = moduleCount * scale;

        QImage image(imageSize, imageSize, QImage::Format_RGB32);
        image.fill(Qt::white);
        QPainter painter(&image);
        // Keep modules square and sharp, with a white quiet zone on every side.
        painter.setRenderHint(QPainter::Antialiasing, false);
        for (int y = 0; y < qr.getSize(); ++y) {
            for (int x = 0; x < qr.getSize(); ++x) {
                if (qr.getModule(x, y)) {
                    painter.fillRect((x + QuietZoneModules) * scale,
                                     (y + QuietZoneModules) * scale,
                                     scale, scale, Qt::black);
                }
            }
        }
        painter.end();
        auto pixmap = QPixmap::fromImage(image);
        pixmap.setDevicePixelRatio(devicePixelRatio);
        return pixmap;
    } catch (const std::exception &) {
        return {};
    }
}
