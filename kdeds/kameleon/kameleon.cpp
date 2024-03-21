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
        if (QFileInfo(QFile(LED_SYSFS_PATH + ledDevice + LED_RGB_FILE)).exists()) {
            qCDebug(KAMELEON) << "found RGB LED device" << ledDevice;
            m_rgbLedDevices.append(ledDevice);
        }
    }
}

bool Kameleon::isSupported()
{
    return !m_rgbLedDevices.isEmpty();
}

void Kameleon::loadConfig()
{
    m_followAccent = m_config->group("General").readEntry<bool>("AccentColoredDeviceLeds", true);

    color = m_config->group("General").readEntry<QColor>("ColorDeviceLeds", QColor(QColorConstants::White));

    QColor customAccentColor = m_config->group("General").readEntry<QColor>("AccentColor", QColor::Invalid);
    QColor schemeAccentColor = m_config->group("Colors::View").readEntry<QColor>("ForegroundActive", QColor::Invalid);
    QColor activeAccentColor = customAccentColor.isValid() ? customAccentColor
        : schemeAccentColor.isValid()                      ? schemeAccentColor
                                                           : QColor(QColorConstants::White);

    if (m_followAccent) {
        color = activeAccentColor;
    }

    if (color != m_color) {
        qCInfo(KAMELEON) << "color changed";
        m_color = color;
        applyColor(color);
    }
}

QString Kameleon::color()
{
    return m_color.name();
}

bool Kameleon::isAccent()
{
    return m_isAccent;
}

void Kameleon::setColor(QString colorName)
{
    QColor color = QColor(colorName);
    if (!color.isValid()) {
        qCWarning(KAMELEON) << "invalid color" << colorName;
    }

    if (m_followAccent) {
        qCInfo(KAMELEON) << "accent syncing disabled";
        m_config->group("General").writeEntry<bool>("AccentColoredDeviceLeds", false);
    }

    if (color != m_color) {
        qCInfo(KAMELEON) << "setting color" << colorName;
        m_config->group("General").writeEntry<QColor>("ColorDeviceLeds", colorName);
    }
}

void Kameleon::setAccent()
{
    if (!m_followAccent) {
        qCInfo(KAMELEON) << "accent syncing enabled";
        m_config->group("General").writeEntry<bool>("AccentColoredDeviceLeds", true);
    }
}

void Kameleon::applyColor(QColor color)
{
    KAuth::Action action("org.kde.kameleonhelper.writecolor");
    action.setHelperId("org.kde.kameleonhelper");
    action.addArgument("color", color.name());
    action.addArgument("devices", m_rgbLedDevices);
    auto *job = action.execute();

    connect(job, &KAuth::ExecuteJob::result, this, [job] {
        if (job->error()) {
            qCWarning(KAMELEON) << "Failed to write color to devices" << job->errorText();
            return;
        } else {
            m_color = color;
        }
    });
    job->start();
}

#include "kameleon.moc"
