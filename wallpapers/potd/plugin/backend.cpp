/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "backend.h"

#include <QFileDialog> // For Save dialog
#include <QStandardPaths> // For "Pictures" folder
#include <QTimer> // For resetting the status
#include <QUrl>

#include <KIO/CopyJob> // For "Save Image"
#include <KLocalizedString>

Backend::Backend(QObject *parent)
    : QObject(parent)
    , m_saveStatus(FileOperationStatus::None)
{
}

QString Backend::saveImage(const QUrl &sourceUrl, const QString &defaultName)
{
    const QStringList &locations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    const QString path = locations.isEmpty() ? QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0) : locations.at(0);

    const QString dest = QFileDialog::getSaveFileName(nullptr,
                                                      i18nc("@title:window", "Save Today's Picture"),
                                                      path + "/" + sanitizeFileName(defaultName) + ".jpg",
                                                      i18nc("@label:listbox Template for file dialog", "JPEG image (*.jpeg *.jpg *.jpe)"),
                                                      nullptr,
                                                      QFileDialog::DontConfirmOverwrite // KIO::CopyJob will show the confirmation dialog.
    );

    if (dest.isEmpty()) {
        return dest;
    }

    KIO::CopyJob *copyJob = KIO::copy(sourceUrl, QUrl::fromLocalFile(dest), KIO::HideProgressInfo);
    connect(copyJob, &KJob::finished, this, [this](KJob *job) {
        setSaveStatus(job->error() ? FileOperationStatus::Failed : FileOperationStatus::Succeeded);
        QTimer::singleShot(5000, this, std::bind(&Backend::setSaveStatus, this, FileOperationStatus::None));
    });
    copyJob->start();

    return dest;
}

Backend::FileOperationStatus Backend::saveStatus() const
{
    return m_saveStatus;
}

void Backend::setSaveStatus(FileOperationStatus status)
{
    if (status == m_saveStatus) {
        return;
    }

    m_saveStatus = status;
    Q_EMIT saveStatusChanged();
}

QString Backend::sanitizeFileName(const QString &name) const
{
    if (name.isEmpty()) {
        return name;
    }

    const char notAllowedChars[] = ",^@={}[]~!?:&*\"|#%<>$\"'();`'/\\";
    QString sanitizedName(name);

    for (const char *c = notAllowedChars; *c; c++) {
        sanitizedName.replace(QLatin1Char(*c), QLatin1Char('-'));
    }

    return sanitizedName;
}
