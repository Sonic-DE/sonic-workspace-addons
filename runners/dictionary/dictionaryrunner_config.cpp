/*
 * SPDX-License-Identifier: GPL-2.0-or-later
 * SPDX-FileCopyrightText: 2010, 2012 Jason A. Donenfeld <Jason@zx2c4.com>
 */

#include "dictionaryrunner_config.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KRunner/AbstractRunner>
#include <KSharedConfig>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>

K_PLUGIN_CLASS(DictionaryRunnerConfig)

DictionaryRunnerConfig::DictionaryRunnerConfig(QObject *parent, const KPluginMetaData &metaData)
    : KCModule(parent, metaData)
{
    QFormLayout *layout = new QFormLayout;
    m_triggerWord = new QLineEdit;
    layout->addRow(i18nc("@label:textbox", "Trigger word:"), m_triggerWord);
    m_operationMode = new QComboBox;
    m_operationMode->addItems({
        i18nc("Use online dictionary", OPMODE_ONLINE),
        i18nc("Use offline dictionary", OPMODE_OFFLINE),
    });
    m_operationMode->setEditable(false);
    widget()->setLayout(layout);
    connect(m_triggerWord, &QLineEdit::textChanged, this, &DictionaryRunnerConfig::markAsChanged);
    connect(m_operationMode, &QComboBox::currentIndexChanged, this, &DictionaryRunnerConfig::markAsChanged);
    load();
}

void DictionaryRunnerConfig::load()
{
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig(QStringLiteral("krunnerrc"));
    KConfigGroup grp = cfg->group(QStringLiteral("Runners"));
    grp = KConfigGroup(&grp, QStringLiteral(KRUNNER_PLUGIN_NAME));
    m_triggerWord->setText(grp.readEntry(CONFIG_TRIGGERWORD, i18nc("Trigger word before word to define", "define")));
    m_operationMode->setCurrentText(grp.readEntry(CONFIG_OPERATIONMODE, OPMODE_OFFLINE));
    KCModule::load();
}

void DictionaryRunnerConfig::save()
{
    KCModule::save();
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig(QLatin1String("krunnerrc"));
    KConfigGroup grp = cfg->group(QStringLiteral("Runners"));
    grp = KConfigGroup(&grp, QStringLiteral(KRUNNER_PLUGIN_NAME));
    grp.writeEntry(CONFIG_TRIGGERWORD, m_triggerWord->text());
    grp.writeEntry(CONFIG_OPERATIONMODE, m_operationMode->currentText());
    grp.sync();
}

void DictionaryRunnerConfig::defaults()
{
    KCModule::defaults();
    m_triggerWord->setText(i18nc("Trigger word before word to define", "define"));
    m_operationMode->setCurrentIndex(1);
    markAsChanged();
}

#include "dictionaryrunner_config.moc"
