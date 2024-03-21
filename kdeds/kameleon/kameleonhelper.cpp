/*
    SPDX-FileCopyrightText: 2024 Natalie Clarius <natalie.clarius@kde.org>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kameleonhelper.h"
#include "kameleonhelper_debug.h"

#include <KAuth/HelperSupport>

#include <QCollator>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

KAuth::ActionReply KameleonHelper::writecolor(const QVariantMap &args)
{
    QStringList devices = args.value(QStringLiteral("devices")).toStringList();
    QStringList entries = args.value(QStringLiteral("entries")).toStringList();
    if (devices.length() != entries.length()) {
        qCWarning(KAMELEONHELPER) << "lists of devices and entires do not match in length";
        return KAuth::ActionReply::HelperErrorReply();
    }

    for (int i = 0; i < devices.size(); ++i) {
        QString device = devices[i];
        QByteArray color = entries[i].toUtf8();
        if (!(color.length() >= QByteArray("0 0 0").length() && color.length() <= QByteArray("255 255 255").length())) {
            qCWarning(KAMELEONHELPER) << "invalid RGB color" << color << "for device" << device;
            return KAuth::ActionReply::HelperErrorReply();
        }

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
        const int bytesWritten = file.write(color);
        if (bytesWritten == -1) {
            qCWarning(KAMELEONHELPER) << "writing to" << file.fileName() << "failed:" << file.error() << file.errorString();
            return KAuth::ActionReply::HelperErrorReply();
        }
        qCInfo(KAMELEONHELPER) << "wrote color to" << device;
    }

    return KAuth::ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.kameleonhelper", KameleonHelper)

#include "moc_kameleonhelper.cpp"
