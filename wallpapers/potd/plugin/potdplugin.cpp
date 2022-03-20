/*
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "potdplugin.h"
#include "potd.h"
#include <QQmlContext>

void PotdPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(uri == QLatin1String("org.kde.plasma.wallpapers.potd"));

    qmlRegisterType<PotdEngine>(uri, 1, 0, "PotdEngine");
    qmlRegisterType<PotdProvidersModel>(uri, 1, 0, "PotdProvidersModel");
}
