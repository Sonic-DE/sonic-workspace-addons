/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "potdplugin.h"
#include "backend.h"

void PotdPlugin::registerTypes(const char *uri)
{
    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.kde.potd.private"));
    qmlRegisterType<Backend>(uri, 0, 1, "Backend");
    qmlRegisterUncreatableType<Backend>(uri, 0, 1, "Global", "Error: only enums");
}
