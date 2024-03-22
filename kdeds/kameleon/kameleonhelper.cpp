/*
    SPDX-FileCopyrightText: 2024 Natalie Clarius <natalie.clarius@kde.org>
    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kameleonhelper.h"
#include "kameleonhelper_debug.h"

#include <KAuth/HelperSupport>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

KAuth::ActionReply KameleonHelper::writecolor(const QVariantMap &args)
{
    QMap<QString, QVariant> devices = args.value(QStringLiteral("entries")).toMap();
    for (auto i = devices.cbegin(), end = devices.cend(); i != end; ++i) {
        QByteArray color = i.value().toByteArray();
        QRegularExpression re("\\d{1,3}\\s\\d{1,3}\\s\\d{1,3}");
        QRegularExpressionValidator validator(re, 0);
        QString s = QString(color);
        int pos = 0;
        if (validator.validate(s, pos) != QValidator::Acceptable) {
            qCWarning(KAMELEONHELPER) << "writing color" << color << "failed:"
                                      << "color is not a valid RGB value";
            return KAuth::ActionReply::HelperErrorReply();
        }

        QString device = i.key();
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
        } else {
            qCInfo(KAMELEONHELPER) << "wrote color to" << file.fileName();
        }
    }

    return KAuth::ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("org.kde.kameleonhelper", KameleonHelper)

#include "moc_kameleonhelper.cpp"
