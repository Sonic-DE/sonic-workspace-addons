/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "backend.h"

#include <QUrl>

#include <KIO/CopyJob>

Backend::Backend(QObject *parent)
    : QObject(parent)
{
}

void Backend::copy(const QUrl &sourceUrl, const QUrl &destUrl)
{
    if (!(sourceUrl.isValid() && destUrl.isValid())) {
        return;
    }

    KIO::CopyJob *copyJob = KIO::copy(sourceUrl, destUrl, KIO::HideProgressInfo);
    copyJob->start();
}
