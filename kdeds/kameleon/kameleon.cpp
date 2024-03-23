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
#include <qcontainerfwd.h>

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

    loadConfig();
    connect(m_configWatcher.get(), &KConfigWatcher::configChanged, this, &Kameleon::updateAccentColor);
}

void Kameleon::findRgbLedDevices()
{
    QDir ledsDir(LED_SYSFS_PATH);
    ledsDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot | QDir::Readable);
    auto ledDevices = ledsDir.entryList();

    // Sort to ensure keys light up nicely one next to the other
    QCollator coll;
    coll.setNumericMode(true);
    std::sort(ledDevices.begin(), ledDevices.end(), [&](const QString &s1, const QString &s2) {
        return coll.compare(s1, s2) < 0;
    });

    for (const QString &ledDevice : ledDevices) {
        // Get multicolor index (= RGB capability with order of colors in format {"red", "green", "blue"})
        QFile indexFile(LED_SYSFS_PATH + ledDevice + LED_INDEX_FILE);
        if (!QFileInfo(indexFile).exists()) {
            // Not a RGB capable device
            continue;
        }
        if (!indexFile.open(QIODevice::ReadOnly)) {
            qCWarning(KAMELEON) << "failed to open" << indexFile.fileName() << indexFile.error() << indexFile.errorString();
            continue;
        }
        QTextStream indexFileStream(&indexFile);
        QStringList colorIndex = indexFileStream.readAll().trimmed().split(" ");
        indexFile.close();
        if (!(colorIndex.length() == 3 && colorIndex.contains("red") && colorIndex.contains("green") && colorIndex.contains("blue"))) {
            qCWarning(KAMELEON) << "invalid color index" << colorIndex.join(" ") << "for device" << ledDevice;
            continue;
        }

        qCInfo(KAMELEON) << "found RGB LED device" << ledDevice;
        m_rgbLedDevices.append(ledDevice);
        m_deviceRgbIndices.append(colorIndex);

        // Get current color
        // TODO: Monitor color changes continiously rather than only checking once on startup?
        QFile intensityFile(LED_SYSFS_PATH + ledDevice + LED_RGB_FILE);
        if (!QFileInfo(intensityFile).exists()) {
            qCWarning(KAMELEON) << "failed to read from" << intensityFile.fileName() << "file does not exist";
            continue;
        }
        if (!intensityFile.open(QIODevice::ReadOnly)) {
            qCWarning(KAMELEON) << "failed to read from" << intensityFile.fileName() << intensityFile.error() << intensityFile.errorString();
            continue;
        }
        QTextStream intensityFileStream(&intensityFile);
        QStringList deviceColorStr = intensityFileStream.readAll().trimmed().split(" ");
        intensityFile.close();
        int red = deviceColorStr[colorIndex.indexOf("red")].toInt();
        int green = deviceColorStr[colorIndex.indexOf("green")].toInt();
        int blue = deviceColorStr[colorIndex.indexOf("blue")].toInt();
        QColor deviceColor = QColor::fromRgb(qRgb(red, green, blue));
        if (!deviceColor.isValid()) {
            qCWarning(KAMELEON) << "invalid color" << deviceColorStr.join(" ") << "for device" << ledDevice;
            continue;
        }
        if (!m_activeColor.isValid()) {
            m_activeColor = deviceColor;
        } else if (m_activeColor != deviceColor) {
            qCWarning(KAMELEON) << "different colors found on multiple devices; treating as white";
            m_activeColor = QColor(QColorConstants::White);
        }
    }
}

bool Kameleon::isSupported()
{
    return !m_rgbLedDevices.isEmpty();
}

void Kameleon::loadConfig()
{
    qCDebug(KAMELEON) << "load color config";

    qCInfo(KAMELEON) << "current color" << m_activeColor.name();

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
        qCInfo(KAMELEON) << "accent color changed" << m_accentColor.name();
        applyColor(m_accentColor);
    }
}

void Kameleon::updateCustomColor()
{
    if (m_accent) {
        return;
    }
    if (m_customColor != m_activeColor) {
        qCInfo(KAMELEON) << "custom color changed" << m_customColor.name();
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

void Kameleon::setAccent(bool enabled)
{
    if (enabled != !m_accent) {
        qCInfo(KAMELEON) << "setting accent syncing" << enabled;
        m_config->group("General").writeEntry<bool>(CONFIG_KEY_ACCENT, enabled);
        m_config->sync();
        m_accent = enabled;
    }
    if (enabled) {
        updateAccentColor();
    } else {
        setColor(QColorConstants::White.name());
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
        qCInfo(KAMELEON) << "setting accent syncing false";
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
    QStringList entries;
    for (int i = 0; i < m_rgbLedDevices.length(); ++i) {
        QString deviceName = m_rgbLedDevices[i];
        if (i >= m_deviceRgbIndices.length()) {
            qCWarning(KAMELEON) << "lists of devices and entires do not match in length";
            continue;
        }
        QStringList colorIndex = m_deviceRgbIndices[i];
        QStringList colorBytesList = {"", "", ""};
        colorBytesList[colorIndex.indexOf("red")] = QString::number(color.red());
        colorBytesList[colorIndex.indexOf("green")] = QString::number(color.green());
        colorBytesList[colorIndex.indexOf("blue")] = QString::number(color.blue());
        entries.append(colorBytesList.join(" "));
    }

    KAuth::Action action("org.kde.kameleonhelper.writecolor");
    action.setHelperId("org.kde.kameleonhelper");
    action.addArgument("devices", m_rgbLedDevices);
    action.addArgument("entries", entries);
    auto *job = action.execute();

    connect(job, &KAuth::ExecuteJob::result, this, [this, job, color]() {
        if (job->error()) {
            qCWarning(KAMELEON) << "failed to write color to devices" << job->errorText();
            return;
        }
        m_activeColor = color;
        qCInfo(KAMELEON) << "wrote color" << color.name() << "to LED devices";
    });
    job->start();
}

#include "kameleon.moc"
