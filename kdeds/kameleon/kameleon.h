/*
    SPDX-FileCopyrightText: 2024 Natalie Clarius <natalie.clarius@kde.org>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <KConfigWatcher>
#include <kdedmodule.h>

#include <QColor>
#include <qnamespace.h>

#define LED_SYSFS_PATH "/sys/class/leds/"
#define LED_RGB_FILE "/multi_intensity"

#define CONFIG_KEY_ACCENT "DeviceLedsAccentColored"
#define CONFIG_KEY_COLOR "DeviceLedsColor"

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
     * Returns the currently active color as a hex string.
     */
    Q_SCRIPTABLE QString colorName();

    /**
     * Returns whether accent color syncing is enabled.
     */
    Q_SCRIPTABLE bool isAccent();

    /**
     * Sets a custom color given as a hex string.
     */
    Q_SCRIPTABLE void setColor(QString colorName);

    /**
     * Enables following the accent color.
     */
    Q_SCRIPTABLE void setAccent();

private:
    KSharedConfig::Ptr m_config;
    KConfigWatcher::Ptr m_configWatcher;
    QStringList m_rgbLedDevices;

    QColor m_color = QColor(QColorConstants::White);
    bool m_accent = true;

    void findRgbLedDevices();
    void updateLedColor();
    void updateAccentColor();
    void applyColor(QColor color);
};
