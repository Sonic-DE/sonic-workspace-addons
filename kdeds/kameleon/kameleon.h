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
     * Returns whether accent color syncing is enabled.
     */
    Q_SCRIPTABLE bool isAccent();

    /**
     * Returns the currently set color as a hex string.
     */
    Q_SCRIPTABLE QString activeColor();

    /**
     * Enables following the accent color.
     */
    Q_SCRIPTABLE void setAccent();

    /**
     * Sets a custom color given as a hex string.
     */
    Q_SCRIPTABLE void setColor(QString colorName);

private:
    KSharedConfig::Ptr m_config;
    KConfigWatcher::Ptr m_configWatcher;
    QStringList m_rgbLedDevices;
    QColor m_activeColor;

    bool m_accent = true;
    QColor m_accentColor = QColor(QColorConstants::White);
    QColor m_customColor = QColor(QColorConstants::White);

    void findRgbLedDevices();
    void loadColorConfig();
    void updateAccentColor();
    void updateCustomColor();
    void applyColor(QColor color);
};
