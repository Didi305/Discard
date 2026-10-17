#pragma once

#include <QPixmap>
#include <QString>

namespace AuthenticatorQr {
// An empty result lets the setup dialog fall back to manual key entry.
QPixmap render(const QString &uri, qreal devicePixelRatio = 1.0);
}
