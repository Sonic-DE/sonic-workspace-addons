/*
    SPDX-FileCopyrightText: 2023 Bharadwaj Raju <bharadwaj.raju777@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kameleon.h"
#include "kameleon_debug.h"

#include <KConfigGroup>
#include <KMessageBox>
#include <KPluginFactory>
#include <KSharedConfig>

#include <QColor>
#include <QDir>
#include <qfileinfo.h>

#define DEFAULT_ACCENT_COLOR "#3daee9" // Plasma Blue
#define LED_SYSFS_PATH "/sys/class/leds/"

K_PLUGIN_CLASS_WITH_JSON(Kameleon, "kameleon.json")

Kameleon::Kameleon(QObject *parent, const QList<QVariant> &)
    : KDEDModule(parent)
    , m_configWatcher(KConfigWatcher::create(KSharedConfig::openConfig("kdeglobals")))
{
    loadConfig();
    connect(m_configWatcher.get(), &KConfigWatcher::configChanged, this, [this]() {
        loadConfig();
    });
}

void Kameleon::loadConfig()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig("kdeglobals");

    m_enabled = config->group("General").readEntry<bool>("AccentColorDeviceLeds", true);
    if (!m_enabled) {
        return;
    }

    findRgbLedDevices();
    if (m_ledDevices.isEmpty()) {
        qCInfo(KAMELEON) << "No RGB LED devices found";
        return;
    }

    QColor customAccentColor = config->group("General").readEntry<QColor>("AccentColor", QColor::Invalid);
    QColor schemeAccentColor = config->group("Colors::View").readEntry<QColor>("ForegroundActive", QColor::Invalid);
    QColor activeAccentColor = customAccentColor.isValid() ? customAccentColor : schemeAccentColor.isValid() ? schemeAccentColor : QColor(DEFAULT_ACCENT_COLOR);

    if (activeAccentColor != m_accentColor) {
        m_accentColor = activeAccentColor;
        writeColorToDevices();
    }
}

void Kameleon::findRgbLedDevices()
{
    QDir ledsDir(LED_SYSFS_PATH);
    ledsDir.setFilter(QDir::Dirs | QDir::NoDot | QDir::NoDotDot | QDir::NoDotAndDotDot | QDir::Readable);
    for (const QString &ledDevice : ledsDir.entryList()) {
        QFile rgbFile(LED_SYSFS_PATH + ledDevice + QLatin1String("/multi_intensity"));
        if (QFileInfo(rgbFile).exists()) {
            qCInfo(KAMELEON) << "found RGB LED device" << ledDevice;
            m_ledDevices.append(rgbFile.fileName());
        }
    }
}

void Kameleon::writeColorToDevices()
{
    qCInfo(KAMELEON) << "writing accent color to LED devices";

    const QByteArray accentColor =
        QByteArray::number(m_accentColor.red()) + " " + QByteArray::number(m_accentColor.green()) + " " + QByteArray::number(m_accentColor.blue());
    KMessageBox::information(nullptr, accentColor);

    for (const QString &device : m_ledDevices) {
        QFile file(device);
        if (!QFileInfo(file).exists()) {
            qCWarning(KAMELEON) << "writing to " << device << "failed:"
                                << "file does not exist";
            continue;
        }
        if (!QFileInfo(file).isWritable()) {
            qCWarning(KAMELEON) << "writing to " << device << "failed:"
                                << "file is not writable";
            continue;
        }
        if (!file.open(QIODevice::WriteOnly)) {
            qCWarning(KAMELEON) << "writing to " << device << "failed with error code " << file.error() << file.errorString();
            continue;
        }
        const int bytesWritten = file.write(accentColor);
        if (bytesWritten == -1) {
            qCWarning(KAMELEON) << "writing to " << device << "failed with error code " << file.error() << file.errorString();
            continue;
        }
    }
}

#include "kameleon.moc"
