/*
    SPDX-FileCopyrightText: 2024 Natalie Clarius <natalie.clarius@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef _KAMELEONHELPER_H_
#define _KAMELEONHELPER_H_

#include <QObject>

#include <KAuth/ActionReply>

using namespace KAuth;

class KameleonHelper : public QObject
{
    Q_OBJECT
public Q_SLOTS:
    KAuth::ActionReply writecolor(const QVariantMap &args);
};

#endif // _KameleonHELPER_H_
