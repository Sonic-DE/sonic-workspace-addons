/*
    SPDX-FileCopyrightText: 2013 Valentin Rusu <kde@rusu.info>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kameleonhelper.h"
#include "kameleonhelper_debug.h"

#include <KAuth/HelperSupport>

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

ActionReply KameleonHelper::writeColorToDevices(const QVariantMap &args)
{
    Q_UNUSED(args);
    const qint64 uid = QCoreApplication::applicationPid();
    qDebug() << "executing uid=" << uid;

    qCInfo(KAMELEONHELPER) << "writing accent color to LED devices";

    QByteArray accentColor = args.value(QStringLiteral("color")).toByteArray();
    QStringList devices = args.value(QStringLiteral("devices")).toStringList();

    for (const QString &device : devices) {
        QFile file(device);
        if (!QFileInfo(file).exists()) {
            qCWarning(KAMELEONHELPER) << "writing to " << device << "failed:"
                                      << "file does not exist";
            continue;
        }
        if (!QFileInfo(file).isWritable()) {
            qCWarning(KAMELEONHELPER) << "writing to " << device << "failed:"
                                      << "file is not writable";
            continue;
        }
        if (!file.open(QIODevice::WriteOnly)) {
            qCWarning(KAMELEONHELPER) << "writing to " << device << "failed:" << file.error() << file.errorString();
            continue;
        }
        const int bytesWritten = file.write(accentColor);
        if (bytesWritten == -1) {
            qCWarning(KAMELEONHELPER) << "writing to " << device << "failed:" << file.error() << file.errorString();
            continue;
        }
    }

    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.kcontrol.kcmkwallet5", KameleonHelper)

#include "moc_kameleonhelper.cpp"
