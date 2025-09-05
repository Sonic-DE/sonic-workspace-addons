/*
    SPDX-FileCopyrightText: 2022 Vlad Zahorodnii <vlad.zahorodnii@kde.org>
    SPDX-FileCopyrightText: 2025 Hocine Hachemi <salahhachmi06@gmail.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "cubeeffectkcm.h"

#include <KActionCollection>
#include <KConfigGroup>
#include <KConfigLoader>
#include <KGlobalAccel>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>

#include <QAction>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QFileDialog>

K_PLUGIN_CLASS(CubeEffectConfig)

CubeEffectConfig::CubeEffectConfig(QObject *parent, const KPluginMetaData &data)
    : KCModule(parent, data)
{
    ui.setupUi(widget());

    QFile xmlFile(QStringLiteral(":/main.xml"));
    KConfigGroup cg = KSharedConfig::openConfig(QStringLiteral("kwinrc"))->group("Effect-cube");
    m_configLoader = new KConfigLoader(cg, &xmlFile, this);
    addConfig(m_configLoader, widget());

    auto actionCollection = new KActionCollection(this, QStringLiteral("kwin"));
    actionCollection->setComponentDisplayName(i18n("KWin"));
    actionCollection->setConfigGroup(QStringLiteral("Cube"));
    actionCollection->setConfigGlobal(true);

    const QKeySequence defaultToggleShortcut = Qt::META | Qt::Key_C;
    QAction *toggleAction = actionCollection->addAction(QStringLiteral("Cube"));
    toggleAction->setText(i18n("Toggle Cube"));
    toggleAction->setProperty("isConfigurationAction", true);
    KGlobalAccel::self()->setDefaultShortcut(toggleAction, {defaultToggleShortcut});
    KGlobalAccel::self()->setShortcut(toggleAction, {defaultToggleShortcut});

    ui.shortcutsEditor->addCollection(actionCollection);
    connect(ui.shortcutsEditor, &KShortcutsEditor::keyChange, this, &CubeEffectConfig::markAsChanged);

    connect(ui.slider_DistanceFactor, &QSlider::sliderMoved, this, &CubeEffectConfig::updateDistanceFactorFromUi);
    connect(ui.slider_DistanceFactor, &QSlider::valueChanged, this, &CubeEffectConfig::updateDistanceFactorTooltip);
    connect(ui.kcfg_ElevationAngle, &QSlider::valueChanged, this, &CubeEffectConfig::updateElevationAngleTooltip);

    // Initialize tooltips with current values
    updateDistanceFactorTooltip();
    updateElevationAngleTooltip();
}

void CubeEffectConfig::load()
{
    KCModule::load();
    updateUiFromConfig();
    updateUnmanagedState();
}

void CubeEffectConfig::save()
{
    ui.shortcutsEditor->save();
    m_configLoader->save();

    KCModule::save();
    updateUnmanagedState();

    QDBusMessage reconfigureMessage = QDBusMessage::createMethodCall(QStringLiteral("org.kde.KWin"),
                                                                     QStringLiteral("/Effects"),
                                                                     QStringLiteral("org.kde.kwin.Effects"),
                                                                     QStringLiteral("reconfigureEffect"));
    reconfigureMessage.setArguments({QStringLiteral("cube")});
    QDBusConnection::sessionBus().call(reconfigureMessage);
}

void CubeEffectConfig::defaults()
{
    KCModule::defaults();
    updateUiFromDefaultConfig();
    updateUnmanagedState();
}

void CubeEffectConfig::updateUiFromConfig()
{
    setDistanceFactor(m_configLoader->findItemByName(QStringLiteral("DistanceFactor"))->property().toDouble());
}

void CubeEffectConfig::updateUiFromDefaultConfig()
{
    setDistanceFactor(m_configLoader->findItemByName(QStringLiteral("DistanceFactor"))->property().toDouble());
    ui.shortcutsEditor->allDefault();
}

qreal CubeEffectConfig::distanceFactor() const
{
    return ui.slider_DistanceFactor->value() / 100.0;
}

void CubeEffectConfig::setDistanceFactor(qreal factor)
{
    ui.slider_DistanceFactor->setValue(std::round(factor * 100));
}

void CubeEffectConfig::updateDistanceFactorFromUi()
{
    m_configLoader->findItemByName(QStringLiteral("DistanceFactor"))->setProperty(distanceFactor());
    updateUnmanagedState();
}

void CubeEffectConfig::updateDistanceFactorTooltip()
{
    qreal factor = ui.slider_DistanceFactor->value() / 100.0;
    ui.slider_DistanceFactor->setToolTip(i18n("Distance factor: %1x", factor));
}

void CubeEffectConfig::updateElevationAngleTooltip()
{
    int angle = ui.kcfg_ElevationAngle->value();
    ui.kcfg_ElevationAngle->setToolTip(i18n("Elevation angle: %1°", angle));
}

void CubeEffectConfig::updateUnmanagedState()
{
    const auto distanceFactorItem = m_configLoader->findItemByName(QStringLiteral("DistanceFactor"));
    const auto elevationAngleItem = m_configLoader->findItemByName(QStringLiteral("ElevationAngle"));

    unmanagedWidgetChangeState(distanceFactorItem->isSaveNeeded() || elevationAngleItem->isSaveNeeded());
    unmanagedWidgetDefaultState(distanceFactorItem->isDefault() || elevationAngleItem->isDefault());
}

#include "cubeeffectkcm.moc"
