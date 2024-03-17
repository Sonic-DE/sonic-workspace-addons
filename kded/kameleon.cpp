/*
    SPDX-FileCopyrightText: 2023 Bharadwaj Raju <bharadwaj.raju777@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kameleon.h"
#include "kameleon_debug.h"

#include <KMessageBox>

#include <KPluginFactory>

#include <KConfigGroup>
#include <KSharedConfig>

#include <KAuth/Action>
#include <KAuth/ActionReply>
#include <KAuth/ExecuteJob>
#include <KAuth/HelperSupport>

#include <QColor>
#include <QDir>
#include <QFileInfo>

#define DEFAULT_ACCENT_COLOR "#3daee9" // Plasma Blue
#define LED_SYSFS_PATH "/sys/class/leds/"
#define LED_RGB_FILE "multi_intensity"

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
        qCInfo(KAMELEON) << "no RGB LED devices found";
        return;
    }

    QColor customAccentColor = config->group("General").readEntry<QColor>("AccentColor", QColor::Invalid);
    QColor schemeAccentColor = config->group("Colors::View").readEntry<QColor>("ForegroundActive", QColor::Invalid);
    QColor activeAccentColor = customAccentColor.isValid() ? customAccentColor : schemeAccentColor.isValid() ? schemeAccentColor : QColor(DEFAULT_ACCENT_COLOR);

    if (activeAccentColor != m_accentColor) {
        m_accentColor = activeAccentColor;

        const QByteArray accentColorStr =
            QByteArray::number(m_accentColor.red()) + " " + QByteArray::number(m_accentColor.green()) + " " + QByteArray::number(m_accentColor.blue());
        KMessageBox::information(nullptr, accentColorStr);

        KAuth::Action action("org.kde.kameleon.kameleonhelper.writecolor");
        action.setHelperId("org.kde.kameleonhelper");
        action.addArgument("color", accentColorStr);
        action.addArgument("devices", m_ledDevices);
        auto *job = action.execute();

        connect(job, &KAuth::ExecuteJob::result, this, [job] {
            if (job->error()) {
                qCWarning(KAMELEON) << "Failed to write color to devices" << job->errorText();
                return;
            }
        });
        job->start();
    }
}

void Kameleon::findRgbLedDevices()
{
    QDir ledsDir(LED_SYSFS_PATH);
    ledsDir.setFilter(QDir::Dirs | QDir::NoDot | QDir::NoDotDot | QDir::NoDotAndDotDot | QDir::Readable);
    for (const QString &ledDevice : ledsDir.entryList()) {
        QFile rgbFile(LED_SYSFS_PATH + ledDevice + QLatin1String("/") + LED_RGB_FILE);
        if (QFileInfo(rgbFile).exists()) {
            qCInfo(KAMELEON) << "found RGB LED device" << ledDevice;
            m_ledDevices.append(rgbFile.fileName());
        }
    }
}

#include "kameleon.moc"
