// SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
// SPDX-FileCopyrightText: SonicDE

#pragma once

#include <QColor>
#include <QDBusContext>
#include <QMap>
#include <QObject>
#include <QString>

class QTimer;

// Standalone D-Bus service object. HasDevices/ApplyColor are Q_SCRIPTABLE slots so that
// registerObject(path, interface, this, ExportScriptableSlots) exports them over the bus.
class QmkDBusHelper : public QObject, public QDBusContext
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kameleon.qmk.helper")

public:
    explicit QmkDBusHelper(QObject *parent = nullptr);
    ~QmkDBusHelper() override;

public Q_SLOTS:
    Q_SCRIPTABLE bool HasDevices();
    Q_SCRIPTABLE void ApplyColor(const QString &color);

private:
    bool checkAuthorization(const QString &actionId, const QMap<QString, QString> &details = {}); // sends AuthFailed error on failure; passes details to polkit
    bool discoverAndCheckSupport(); // true if a device with usable lighting is found
    bool applyColorToKeyboards(const QColor &color);
    void resetIdleTimer(); // re-arms the transient-exit timer (matches Rust idle exit)

    QTimer *m_idleTimer = nullptr; // fires QCoreApplication::quit() after 60s of no D-Bus activity
};
