/*
    SPDX-FileCopyrightText: 2024 Natalie Clarius <natalie.clarius@kde.org>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <KConfigWatcher>
#include <kdedmodule.h>

#include <QColor>

#define DEFAULT_ACCENT_COLOR "#3daee9" // Plasma Blue
#define LED_SYSFS_PATH "/sys/class/leds/"
#define LED_RGB_FILE "multi_intensity"

using namespace Qt::Literals::StringLiterals;

// DEFAULT_SINK_NAME in module-always-sink.c
constexpr QLatin1String DUMMY_OUTPUT_NAME = "auto_null"_L1;

class Kameleon : public KDEDModule
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.kameleon")
public:
    Kameleon(QObject *parent, const QList<QVariant> &);

    /**
     * Returns whether there are any RGB cabaple LED devices.
     */
    Q_SCRIPTABLE bool isSupported();

    /**
     * Returns whether accent color syncing is enabled.
     */
    Q_SCRIPTABLE bool isEnabled();

    /**
     * Enables or disables accent color syncing.
     */
    Q_SCRIPTABLE void setEnabled(bool enabled);

private:
    bool m_enabled = false;
    QColor m_accentColor = QColor(DEFAULT_ACCENT_COLOR);

    KConfigWatcher::Ptr m_configWatcher;
    QStringList m_rgbLedDevices;

    void loadConfig();
    void findRgbLedDevices();
    void applyColor(QColor color);
};
