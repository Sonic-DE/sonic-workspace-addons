/*
    SPDX-FileCopyrightText: 2024 Natalie Clarius <natalie.clarius@kde.org>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kameleonhelper.h"
#include "kameleonhelper_debug.h"

#include <KAuth/HelperSupport>

#include <QColor>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

KAuth::ActionReply KameleonHelper::writecolor(const QVariantMap &args)
{
    QColor color(args.value(QStringLiteral("color")).toString());
    if (!color.isValid()) {
        qCWarning(KAMELEONHELPER) << "invalid color" << args.value(QStringLiteral("color")).toString();
        return KAuth::ActionReply::HelperErrorReply();
    }
    const QByteArray colorStr = QByteArray::number(color.red()) + " " + QByteArray::number(color.green()) + " " + QByteArray::number(color.blue());

    QStringList devices = args.value(QStringLiteral("devices")).toStringList();

    for (const QString &device : devices) {
        QFile file(LED_SYSFS_PATH + device + LED_RGB_FILE);
        if (!QFileInfo(file).exists()) {
            qCWarning(KAMELEONHELPER) << "writing to" << file.fileName() << "failed:"
                                      << "file does not exist";
            return KAuth::ActionReply::HelperErrorReply();
        }
        if (!QFileInfo(file).isWritable()) {
            qCWarning(KAMELEONHELPER) << "writing to" << file.fileName() << "failed:"
                                      << "file is not writable";
            return KAuth::ActionReply::HelperErrorReply();
        }
        if (!file.open(QIODevice::WriteOnly)) {
            qCWarning(KAMELEONHELPER) << "writing to" << file.fileName() << "failed:" << file.error() << file.errorString();
            return KAuth::ActionReply::HelperErrorReply();
        }
        const int bytesWritten = file.write(colorStr);
        if (bytesWritten == -1) {
            qCWarning(KAMELEONHELPER) << "writing to" << file.fileName() << "failed:" << file.error() << file.errorString();
            return KAuth::ActionReply::HelperErrorReply();
        } else {
            qCDebug(KAMELEONHELPER) << "wrote color to" << file.fileName();
        }
    }

    qCInfo(KAMELEONHELPER) << "wrote color" << colorStr << "to LED devices";
    return KAuth::ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.kameleonhelper", KameleonHelper)

#include "moc_kameleonhelper.cpp"
