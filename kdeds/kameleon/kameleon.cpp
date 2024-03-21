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

    updateLedColor();
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

void Kameleon::updateLedColor()
{
    qCDebug(KAMELEON) << "load led color";

    bool followAccent = m_config->group("General").readEntry<bool>("AccentColoredDeviceLeds", true);
    if (followAccent != m_accent) {
        qCInfo(KAMELEON) << "accent syncing configured" << followAccent;
        m_accent = followAccent;
    }
    if (m_accent) {
        updateAccentColor();
        return;
    }

    QColor color = m_config->group("General").readEntry<QColor>("ColorDeviceLeds", QColor(QColorConstants::White));
    if (color.isValid() && color != m_color) {
        qCInfo(KAMELEON) << "color configured" << color.name();
        applyColor(color);
    }
}

void Kameleon::updateAccentColor()
{
    if (!m_accent) {
        return;
    }
    qCDebug(KAMELEON) << "load accent color";
    QColor customAccentColor = m_config->group("General").readEntry<QColor>("AccentColor", QColor::Invalid);
    QColor schemeAccentColor = m_config->group("Colors::View").readEntry<QColor>("ForegroundActive", QColor::Invalid);
    QColor activeAccentColor = customAccentColor.isValid() ? customAccentColor
        : schemeAccentColor.isValid()                      ? schemeAccentColor
                                                           : QColor(QColorConstants::White);

    if (activeAccentColor != m_color) {
        qCInfo(KAMELEON) << "accent color changed";
        m_color = activeAccentColor;
        applyColor(activeAccentColor);
    }
}

QString Kameleon::colorName()
{
    return m_color.name();
}

bool Kameleon::isAccent()
{
    return m_accent;
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
        m_config->group("General").writeEntry<bool>("AccentColoredDeviceLeds", false);
        m_config->sync();
    }

    if (color != m_color) {
        qCInfo(KAMELEON) << "setting color" << colorName;
        m_config->group("General").writeEntry<QColor>("ColorDeviceLeds", colorName);
        m_config->sync();
        applyColor(m_color);
    }
}

void Kameleon::setAccent()
{
    if (!m_accent) {
        qCInfo(KAMELEON) << "setting accent syncing enabled";
        m_config->group("General").writeEntry<bool>("AccentColoredDeviceLeds", true);
        m_config->group("General").deleteEntry("ColorDeviceLeds");
        m_config->sync();
        m_accent = true;
        updateAccentColor();
    }
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
        m_color = color;
    });
    job->start();
}

#include "kameleon.moc"
