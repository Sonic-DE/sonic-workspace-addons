/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "backend.h"

#include <QTimer>
#include <QUrl>

#include <KIO/CopyJob>

Backend::Backend(QObject *parent)
    : QObject(parent)
    , m_saveStatus(FileOperationStatus::None)
{
}

void Backend::copy(const QUrl &sourceUrl, const QUrl &destUrl)
{
    if (!(sourceUrl.isValid() && destUrl.isValid())) {
        return;
    }

    KIO::CopyJob *copyJob = KIO::copy(sourceUrl, destUrl, KIO::HideProgressInfo);
    connect(copyJob, &KJob::finished, this, [this](KJob *job) {
        m_saveStatus = job->error() ? FileOperationStatus::Failed : FileOperationStatus::Succeeded;
        Q_EMIT saveStatusChanged();
        QTimer::singleShot(5000, this, [this] {
            m_saveStatus = FileOperationStatus::None; // Reset the status
            Q_EMIT saveStatusChanged();
        });
    });
    copyJob->start();
}

Backend::FileOperationStatus Backend::saveStatus() const
{
    return m_saveStatus;
}