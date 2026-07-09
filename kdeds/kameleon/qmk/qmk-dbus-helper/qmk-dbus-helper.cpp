// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: SonicDE

#include "qmk-dbus-helper.h"
#include "qmk-dbus-helper_debug.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusReply>
#include <QTimer>
#include <QtGlobal>

#ifdef HAVE_VIA_SUPPORT
#include <via/via.h> // ViaDevice, ViaProtocol, ViaDiscovery, LightingChannel
#endif

#ifdef HAVE_POLKIT_SUPPORT
#include <polkit-qt6-1/polkitqt1-authority.h>
#include <polkit-qt6-1/polkitqt1-subject.h>
#endif

QmkDBusHelper::QmkDBusHelper(QObject *parent)
    : QObject(parent)
    , QDBusContext()
{
    // Transient lifetime: quit after 60s of D-Bus inactivity so D-Bus re-activates on the next
    // call (matches the Rust helper's event-listener wait_timeout(60s)). Single-shot timer.
    m_idleTimer = new QTimer(this);
    m_idleTimer->setSingleShot(true);
    m_idleTimer->callOnTimeout(qApp, &QCoreApplication::quit);
    m_idleTimer->start(60000);
}

QmkDBusHelper::~QmkDBusHelper() = default;

void QmkDBusHelper::resetIdleTimer()
{
    if (m_idleTimer) {
        m_idleTimer->start(60000);
    }
}

bool QmkDBusHelper::HasDevices()
{
    resetIdleTimer();
#ifndef HAVE_VIA_SUPPORT
    sendErrorReply(QStringLiteral("org.freedesktop.DBus.Error.NotSupported"), QStringLiteral("VIA support not built in"));
    return false;
#else
    if (!checkAuthorization(QStringLiteral("org.kde.kameleon.qmk.helper.HasDevices"))) {
        // checkAuthorization already sent the AuthFailed error
        return false;
    }
    return discoverAndCheckSupport();
#endif
}

void QmkDBusHelper::ApplyColor(const QString &color)
{
    resetIdleTimer();
#ifndef HAVE_VIA_SUPPORT
    Q_UNUSED(color)
    sendErrorReply(QStringLiteral("org.freedesktop.DBus.Error.NotSupported"), QStringLiteral("VIA support not built in"));
    return;
#else
    if (!checkAuthorization(QStringLiteral("org.kde.kameleon.qmk.helper.ApplyColor"), {{QStringLiteral("color"), color}})) {
        return;
    }

    const QColor qColor(color);
    if (!qColor.isValid()) {
        sendErrorReply(QStringLiteral("org.freedesktop.DBus.Error.InvalidArgs"), QStringLiteral("Invalid color '%1'; expected #RRGGBB").arg(color));
        return;
    }

    if (!applyColorToKeyboards(qColor)) {
        sendErrorReply(QStringLiteral("org.freedesktop.DBus.Error.Failed"), QStringLiteral("Failed to apply color to VIA keyboards"));
    }
#endif
}

bool QmkDBusHelper::checkAuthorization(const QString &actionId, const QMap<QString, QString> &details)
{
#ifndef HAVE_POLKIT_SUPPORT
    Q_UNUSED(actionId)
    Q_UNUSED(details)
    sendErrorReply(QStringLiteral("org.freedesktop.DBus.Error.AuthFailed"), QStringLiteral("PolicyKit support not built in"));
    return false;
#else
    using namespace PolkitQt1;

    if (!calledFromDBus()) {
        sendErrorReply(QStringLiteral("org.freedesktop.DBus.Error.AuthFailed"), QStringLiteral("Not called from D-Bus"));
        return false;
    }

    const QString sender = message().service();
    if (sender.isEmpty()) {
        sendErrorReply(QStringLiteral("org.freedesktop.DBus.Error.AuthFailed"), QStringLiteral("Cannot determine caller bus name"));
        return false;
    }

    const QDBusReply<uint> pidReply = connection().interface()->servicePid(sender);
    if (!pidReply.isValid()) {
        sendErrorReply(QStringLiteral("org.freedesktop.DBus.Error.AuthFailed"),
                       QStringLiteral("Cannot resolve caller PID: %1").arg(pidReply.error().message()));
        return false;
    }

    const UnixProcessSubject subject(static_cast<qint64>(pidReply.value()));

    // WithDetails so the .policy message "Apply color value $(color)..." interpolates the color,
    // matching the Rust helper which passes details={("color", color)} for ApplyColor.
    // DetailsMap is a typedef for QMap<QString,QString>, so `details` passes through directly.
    const Authority::Result result = Authority::instance()->checkAuthorizationSyncWithDetails(actionId, subject, Authority::AllowUserInteraction, details);

    switch (result) {
    case Authority::Yes:
        qCDebug(QMKDBUS) << "Authorization granted for" << actionId;
        return true;
    case Authority::No:
    case Authority::Challenge:
    case Authority::Unknown:
    default:
        sendErrorReply(QStringLiteral("org.freedesktop.DBus.Error.AuthFailed"),
                       QStringLiteral("Authorization denied for %1 (result=%2)").arg(actionId).arg(result));
        return false;
    }
#endif
}

bool QmkDBusHelper::discoverAndCheckSupport()
{
#ifdef HAVE_VIA_SUPPORT
    const auto keyboards = ViaDiscovery::discoverViaKeyboards();
    // Rust has_devices() uses .any(): a device counts when rgblight.is_ok() || matrix.is_ok()
    // (channel reachable), regardless of the effect value — i.e. a device with lighting disabled
    // (effect 0) still reports "has devices". SonicVia::hasLightingSupport() encodes exactly this;
    // do NOT use the effect>0 predicate here (that is applyColor's stricter test).
    for (const auto &info : keyboards) {
        ViaDevice device(info.path);
        if (!device.open()) {
            qCWarning(QMKDBUS) << "Cannot open" << info.path;
            continue; // skip unreachable device (more robust than the Rust .expect() panic)
        }
        // Query the device's VIA protocol version so SonicVia uses the correct command encoding
        // (V3 channel scheme vs legacy single-byte scheme). 0 == unreadable; skip the device.
        const quint16 protoVer = ViaProtocol::queryProtocolVersion(device);
        if (protoVer == 0) {
            qCWarning(QMKDBUS) << "Cannot read protocol version on" << info.path;
            continue;
        }
        ViaProtocol proto(device, protoVer);
        if (proto.hasLightingSupport()) {
            return true;
        }
    }
    return false;
#else
    return false;
#endif
}

bool QmkDBusHelper::applyColorToKeyboards(const QColor &color)
{
#ifdef HAVE_VIA_SUPPORT
    const auto keyboards = ViaDiscovery::discoverViaKeyboards();
    if (keyboards.isEmpty()) {
        qCWarning(QMKDBUS) << "No VIA keyboards found";
        return false;
    }
    // Matches Rust apply_color(): iterate devices; on the FIRST device that cannot be opened or
    // has no active lighting channel (applyColor requires effect>0), abort and return false — the
    // caller emits D-Bus Failed. Devices processed before the failure keep their new color
    // (partial application), exactly like the Rust helper's early `return Err(...)`.
    for (const auto &info : keyboards) {
        ViaDevice device(info.path);
        if (!device.open()) {
            qCWarning(QMKDBUS) << "Failed to open keyboard:" << info.path;
            return false;
        }
        const quint16 protoVer = ViaProtocol::queryProtocolVersion(device);
        if (protoVer == 0) {
            qCWarning(QMKDBUS) << "Cannot read protocol version on" << info.path;
            return false;
        }
        ViaProtocol proto(device, protoVer);
        // applyColor() reproduces the Rust flow: try RGB Matrix (effect>0, V3 only), fall back to
        // RGB Light, sending single-byte hue/saturation (Color value) plus a separate Brightness
        // value. Encoding is selected by the queried protocol version.
        if (!proto.applyColor(color)) {
            qCWarning(QMKDBUS) << "Failed to apply color on" << info.path;
            return false;
        }
    }
    return true;
#else
    return false;
#endif
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("kameleon-qmk-helper"));
    app.setOrganizationDomain(QStringLiteral("kde.org"));

    QmkDBusHelper helper;

    QDBusConnection bus = QDBusConnection::systemBus();
    if (!bus.isConnected()) {
        qCritical() << "Cannot connect to system D-Bus";
        return 1;
    }

    if (!bus.registerObject(QStringLiteral("/org/kde/kameleon/qmk/helper"),
                            QStringLiteral("org.kde.kameleon.qmk.helper"),
                            &helper,
                            QDBusConnection::ExportScriptableSlots)) {
        qCritical() << "Failed to register D-Bus object:" << bus.lastError().message();
        return 1;
    }

    if (!bus.registerService(QStringLiteral("org.kde.kameleon.qmk.helper"))) {
        qCritical() << "Failed to register D-Bus service:" << bus.lastError().message();
        return 1;
    }

    qCInfo(QMKDBUS) << "kameleon-qmk-helper service started";
    return app.exec();
}
