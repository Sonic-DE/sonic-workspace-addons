/*
    SPDX-FileCopyrightText: 2023 Bharadwaj Raju <bharadwaj.raju777@gmail.com>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#pragma once

#include <KConfigWatcher>
#include <kdedmodule.h>

#include <QColor>

using namespace Qt::Literals::StringLiterals;

// DEFAULT_SINK_NAME in module-always-sink.c
constexpr QLatin1String DUMMY_OUTPUT_NAME = "auto_null"_L1;

class Kameleon : public KDEDModule
{
    Q_OBJECT
public:
    Kameleon(QObject *parent, const QList<QVariant> &);

private:
    bool m_enabled;
    QColor m_accentColor;

    KConfigWatcher::Ptr m_configWatcher;
    QStringList m_ledDevices;

    void loadConfig();
    void findRgbLedDevices();
    void writeColorToDevices();
};
