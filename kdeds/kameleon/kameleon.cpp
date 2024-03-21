/*
    SPDX-FileCopyrightText: 2024 Natalie Clarius <natalie.clarius@kde.org>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kameleon.h"
#include "kameleon_debug.h"

#include <KConfigGroup>
#include <KPluginFactory>
#include <KSharedConfig>

#include <KAuth/Action>
#include <KAuth/ActionReply>
#include <KAuth/ExecuteJob>
#include <KAuth/HelperSupport>

#include <QCollator>
#include <QColor>
#include <QDir>
#include <QFileInfo>

K_PLUGIN_CLASS_WITH_JSON(Kameleon, "kameleon.json")

Kameleon::Kameleon(QObject *parent, const QList<QVariant> &)
    : KDEDModule(parent)
    , m_config(KSharedConfig::openConfig("kdeglobals"))
    , m_configWatcher(KConfigWatcher::create(m_config))
{
    findRgbLedDevices();
    if (!isSupported()) {
        qCInfo(KAMELEON) << "found no RGB LED devices";
        return;
    }

    loadColorConfig();
    connect(m_configWatcher.get(), &KConfigWatcher::configChanged, this, &Kameleon::updateAccentColor);
}

void Kameleon::findRgbLedDevices()
{
    QDir ledsDir(LED_SYSFS_PATH);
    ledsDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    auto ledDevices = ledsDir.entryList();

    // sort to ensure keys light up nicely one next to the other
    QCollator coll;
    coll.setNumericMode(true);
    std::sort(ledDevices.begin(), ledDevices.end(), [&](const QString &s1, const QString &s2) {
        return coll.compare(s1, s2) < 0;
    });

    for (const QString &ledDevice : ledDevices) {
        QFile ledFile(LED_SYSFS_PATH + ledDevice + LED_RGB_FILE);
        if (QFileInfo(ledFile).exists()) {
            qCDebug(KAMELEON) << "found RGB LED device" << ledDevice;
            m_rgbLedDevices.append(ledDevice);

            if (!ledFile.open(QIODevice::ReadOnly)) {
                qCWarning(KAMELEON) << "failed to read from" << ledFile.fileName() << ledFile.error() << ledFile.errorString();
                continue;
            }
            QTextStream stream(&ledFile);
            QString deviceColorStr;
            stream >> deviceColorStr;
            ledFile.close();

            QColor deviceColor(deviceColorStr);
            if (!deviceColor.isValid()) {
                qCWarning(KAMELEON) << "invalid color" << deviceColorStr << "for device" << ledDevice;
                continue;
            }
            if (!m_activeColor.isValid()) {
                qCInfo(KAMELEON) << "found device led color" << deviceColor.name();
                m_activeColor = deviceColor;
            } else if (m_activeColor != deviceColor) {
                qCWarning(KAMELEON) << "different colors found on multiple devices; defaulting to white";
                m_activeColor = QColor(QColorConstants::White);
            }
        }
    }
}

bool Kameleon::isSupported()
{
    return !m_rgbLedDevices.isEmpty();
}

void Kameleon::loadColorConfig()
{
    qCDebug(KAMELEON) << "load color config";

    bool accent = m_config->group("General").readEntry<bool>(CONFIG_KEY_ACCENT, true);
    if (accent != m_accent) {
        qCInfo(KAMELEON) << "accent syncing configured" << accent;
        m_accent = accent;
    }
    if (m_accent) {
        updateAccentColor();
        return;
    }

    QColor color = m_config->group("General").readEntry<QColor>(CONFIG_KEY_COLOR, QColor(QColorConstants::White));
    if (color != m_customColor) {
        qCInfo(KAMELEON) << "color configured" << color.name();
        m_customColor = color;
    }
    // Don't enforce applying custom color on startup, to provide a way of opting out of Plasma meddling with device LEDs.
    // If nothing else changed it in the meantime, the last set custom color will still be in effect after a reboot.
    // If it did get changed by an external source, leave it until a custom color is set again explicitly through the frontend.
}

void Kameleon::updateAccentColor()
{
    if (!m_accent) {
        return;
    }
    qCDebug(KAMELEON) << "load accent color";
    QColor customAccentColor = m_config->group("General").readEntry<QColor>("AccentColor", QColor::Invalid);
    QColor schemeAccentColor = m_config->group("Colors::View").readEntry<QColor>("ForegroundActive", QColor::Invalid);
    m_accentColor = customAccentColor.isValid() ? customAccentColor : schemeAccentColor.isValid() ? schemeAccentColor : QColor(QColorConstants::White);

    if (m_accentColor != m_activeColor) {
        qCInfo(KAMELEON) << "accent color changed";
        applyColor(m_accentColor);
    }
}

void Kameleon::updateCustomColor()
{
    if (m_accent) {
        return;
    }
    if (m_customColor != m_activeColor) {
        qCInfo(KAMELEON) << "custom color changed";
        applyColor(m_customColor);
    }
}

QString Kameleon::activeColor()
{
    return m_customColor.name();
}

bool Kameleon::isAccent()
{
    return m_accent;
}

void Kameleon::setAccent()
{
    if (!m_accent) {
        qCInfo(KAMELEON) << "setting accent syncing enabled";
        m_config->group("General").writeEntry<bool>(CONFIG_KEY_ACCENT, true);
        m_config->sync();
        m_accent = true;
        updateAccentColor();
    }
}

void Kameleon::setColor(QString colorName)
{
    QColor color = QColor(colorName);
    if (!color.isValid()) {
        qCWarning(KAMELEON) << "invalid color" << colorName;
        return;
    }

    if (m_accent) {
        qCInfo(KAMELEON) << "setting accent syncing disabled";
        m_config->group("General").writeEntry<bool>(CONFIG_KEY_ACCENT, false);
        m_config->sync();
        m_accent = false;
    }

    if (color != m_customColor) {
        qCInfo(KAMELEON) << "setting color" << colorName;
        m_config->group("General").writeEntry<QColor>(CONFIG_KEY_COLOR, colorName);
        m_config->sync();
        m_customColor = color;
    }
    updateCustomColor();
}

void Kameleon::applyColor(QColor color)
{
    KAuth::Action action("org.kde.kameleonhelper.writecolor");
    action.setHelperId("org.kde.kameleonhelper");
    action.addArgument("color", color.name());
    action.addArgument("devices", m_rgbLedDevices);
    auto *job = action.execute();

    connect(job, &KAuth::ExecuteJob::result, this, [this, job, color]() {
        if (job->error()) {
            qCWarning(KAMELEON) << "Failed to write color to devices" << job->errorText();
            return;
        }
        m_activeColor = color;
    });
    job->start();
}

#include "kameleon.moc"
