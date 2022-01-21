/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "backend.h"

#include <QFileDialog> // For Save dialog
#include <QStandardPaths> // For "Pictures" folder

#include <KIO/CopyJob> // For "Save Image"
#include <KLocalizedString>

Backend::Backend(QObject *parent)
    : QObject(parent)
    , m_saveStatus(FileOperationStatus::None)
{
}

QUrl Backend::saveImage(const QUrl &sourceUrl, const QString &defaultName)
{
    const QStringList &locations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    const QString path = locations.isEmpty() ? QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0) : locations.at(0);

    m_savedUrl = QUrl::fromLocalFile(QFileDialog::getSaveFileName(nullptr,
                                                                  i18nc("@title:window", "Save Today's Picture"),
                                                                  path + "/" + sanitizeFileName(defaultName) + ".jpg",
                                                                  i18nc("@label:listbox Template for file dialog", "JPEG image (*.jpeg *.jpg *.jpe)"),
                                                                  nullptr,
                                                                  QFileDialog::DontConfirmOverwrite // KIO::CopyJob will show the confirmation dialog.
                                                                  ));

    if (m_savedUrl.isEmpty() || !m_savedUrl.isValid()) {
        return QUrl();
    }

    m_savedFolder = QUrl::fromLocalFile(m_savedUrl.toLocalFile().section(QDir::separator(), 0, -2));

    KIO::CopyJob *copyJob = KIO::copy(sourceUrl, m_savedUrl, KIO::HideProgressInfo);
    connect(copyJob, &KJob::finished, this, [this](KJob *job) {
        if (job->error()) {
            m_saveStatusMessage = job->errorText();
            if (m_saveStatusMessage.size() == 0) {
                m_saveStatusMessage = i18ndc("plasma_wallpaper_org.kde.potd", "@info:status after a save action", "The image was not saved.");
            }
            setSaveStatus(FileOperationStatus::Failed);
        } else {
            m_saveStatusMessage = i18ndc("plasma_wallpaper_org.kde.potd",
                                         "@info:status after a save action %1 file path %2 basename",
                                         "The image was saved as <a href=\"%1\">%2</a>",
                                         m_savedUrl.toString(),
                                         m_savedUrl.fileName());
            setSaveStatus(FileOperationStatus::Succeeded);
        }
    });
    copyJob->start();

    return m_savedUrl;
}

void Backend::setSaveStatus(FileOperationStatus status)
{
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
