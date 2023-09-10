/*
 *   SPDX-FileCopyrightText: 2008 Montel Laurent <montel@kde.org>
 *   SPDX-FileCopyrightText: 2020 Alexander Lohnau <alexander.lohnau@gmx.de>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KONSOLEPROFILES_H
#define KONSOLEPROFILES_H

#include "profilesmodel.h"
#include <KRunner/AbstractRunner>

using namespace KRunner;

class KonsoleProfiles : public AbstractRunner
{
    Q_OBJECT

public:
    explicit KonsoleProfiles(QObject *parent, const KPluginMetaData &metaData);

    void match(RunnerContext &context) override;
    void run(const RunnerContext &context, const QueryMatch &match) override;
    void init() override
    {
        m_model.setAppName(m_triggerWord);
    }

private:
    ProfilesModel m_model{this};
    const QLatin1String m_triggerWord = QLatin1String("konsole");
};

#endif
