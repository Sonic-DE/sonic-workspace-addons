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
    QMap<QString, QVariant> entries = args.value(QStringLiteral("entries")).toMap();
    // Sort to ensure keys light up nicely one next to the other
    QStringList devices = entries.keys();
    QCollator coll;
    coll.setNumericMode(true);
    std::sort(devices.begin(), devices.end(), [&](const QString &s1, const QString &s2) {
        return coll.compare(s1, s2) < 0;
    });
    qCInfo(KAMELEONHELPER) << "sorted devices" << devices;

    for (int i = 0; i < devices.size(); ++i) {
        QString device = devices[i];
        QByteArray color = entries[device].toByteArray();
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
