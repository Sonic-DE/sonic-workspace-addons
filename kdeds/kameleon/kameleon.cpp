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

    loadConfig();
    connect(m_configWatcher.get(), &KConfigWatcher::configChanged, this, [this]() {
        loadConfig();
    });
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
        QFile indexFile(LED_SYSFS_PATH + ledDevice + LED_INDEX_FILE);
        QFile intensityFile(LED_SYSFS_PATH + ledDevice + LED_RGB_FILE);
        if (!QFileInfo(indexFile).exists() || !QFileInfo(intensityFile).exists()) {
            continue;
        }

        QString colorIndex;
        if (!indexFile.open(QIODevice::ReadOnly)) {
            qCWarning(KAMELEON) << "failed to open" << indexFile.fileName() << indexFile.error() << indexFile.errorString();
            continue;
        }
        QTextStream stream(&indexFile);
        stream >> colorIndex;
        indexFile.close();

        qCInfo(KAMELEON) << "found RGB LED device" << ledDevice;
        m_rgbLedDevices.insert(ledDevice, colorIndex);
    }
}

bool Kameleon::isSupported()
{
    return !m_rgbLedDevices.isEmpty();
}

bool Kameleon::isEnabled()
{
    return m_enabled;
}

void Kameleon::setEnabled(bool enabled)
{
    if (enabled != m_enabled) {
        qCInfo(KAMELEON) << "enabled changed" << enabled;
        m_enabled = enabled;
        m_config->group("General").writeEntry<bool>("AccentColoredDeviceLeds", enabled);

        if (enabled) {
            applyColor(m_accentColor);
        } else {
            applyColor(QColor(QColorConstants::White));
        }
    }
}

void Kameleon::loadConfig()
{
    m_enabled = m_config->group("General").readEntry<bool>("AccentColoredDeviceLeds", true);

    QColor customAccentColor = m_config->group("General").readEntry<QColor>("AccentColor", QColor::Invalid);
    QColor schemeAccentColor = m_config->group("Colors::View").readEntry<QColor>("ForegroundActive", QColor::Invalid);
    QColor activeAccentColor = customAccentColor.isValid() ? customAccentColor
        : schemeAccentColor.isValid()                      ? schemeAccentColor
                                                           : QColor(QColorConstants::White);

    if (activeAccentColor != m_accentColor) {
        qCInfo(KAMELEON) << "accent color changed" << activeAccentColor;
        m_accentColor = activeAccentColor;
        if (m_enabled) {
            applyColor(m_accentColor);
        }
    }
}

void Kameleon::applyColor(QColor color)
{
    QMap<QString, QVariant> entries;
    for (auto i = m_rgbLedDevices.cbegin(), end = m_rgbLedDevices.cend(); i != end; ++i) {
        QString deviceName = i.key();
        QString colorIndex = i.value();
        QByteArray colorBytes;
        if (colorIndex == "red green blue") {
            colorBytes = QByteArray::fromStdString(QString("%1 %2 %3").arg(color.red()).arg(color.green()).arg(color.blue()).toStdString());
        } else if (colorIndex == "red blue green") {
            colorBytes = QByteArray::fromStdString(QString("%1 %2 %3").arg(color.red()).arg(color.blue()).arg(color.green()).toStdString());
        } else if (colorIndex == "green red blue") {
            colorBytes = QByteArray::fromStdString(QString("%1 %2 %3").arg(color.green()).arg(color.red()).arg(color.blue()).toStdString());
        } else if (colorIndex == "green blue red") {
            colorBytes = QByteArray::fromStdString(QString("%1 %2 %3").arg(color.green()).arg(color.blue()).arg(color.red()).toStdString());
        } else if (colorIndex == "blue red green") {
            colorBytes = QByteArray::fromStdString(QString("%1 %2 %3").arg(color.blue()).arg(color.red()).arg(color.green()).toStdString());
        } else if (colorIndex == "blue green red") {
            colorBytes = QByteArray::fromStdString(QString("%1 %2 %3").arg(color.blue()).arg(color.green()).arg(color.red()).toStdString());
        } else {
            qCWarning(KAMELEON) << "invalid color index" << colorIndex << "for device" << deviceName;
            continue;
        }
        entries.insert(deviceName, colorBytes);
    }

    qCInfo(KAMELEON) << "writing color" << color.name() << "to LED devices";
    KAuth::Action action("org.kde.kameleonhelper.writecolor");
    action.setHelperId("org.kde.kameleonhelper");
    action.addArgument("entries", entries);
    auto *job = action.execute();

    connect(job, &KAuth::ExecuteJob::result, this, [job] {
        if (job->error()) {
            qCWarning(KAMELEON) << "Failed to write color to devices" << job->errorText();
            return;
        }
    });
    job->start();
}

#include "kameleon.moc"
