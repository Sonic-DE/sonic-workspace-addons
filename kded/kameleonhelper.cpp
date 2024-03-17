/*
    SPDX-FileCopyrightText: 2013 Valentin Rusu <kde@rusu.info>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kameleonhelper.h"

#include <KAuth/HelperSupport>

#include <QCoreApplication>
#include <QDebug>

ActionReply KameleonHelper::Kameleon(const QVariantMap &args)
{
    Q_UNUSED(args);
    const qint64 uid = QCoreApplication::applicationPid();
    qDebug() << "executing uid=" << uid;

    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.kcontrol.kcmkwallet5", KameleonHelper)

#include "moc_kameleonhelper.cpp"
