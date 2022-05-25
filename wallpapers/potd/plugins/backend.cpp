/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "backend.h"

#include <QDBusConnection>
#include <QFileDialog>
#include <QStandardPaths> // For "Pictures" folder

#include <KIO/CopyJob> // For "Save Image"
#include <KLocalizedString>

#include "potdengine.h"

namespace
{
static PotdEngine *s_engine = nullptr;
static int s_instanceCount = 0;
}

Backend::Backend(QObject *parent)
    : QObject(parent)
{
    if (!s_engine) {
        Q_ASSERT(s_instanceCount == 0);
        s_engine = new PotdEngine();
    }
    s_instanceCount++;
}

Backend::~Backend()
{
    s_engine->unregisterClient(m_identifier, m_args);
    s_instanceCount--;

    if (!s_instanceCount) {
        delete s_engine;
        s_engine = nullptr;
    }
}

void Backend::classBegin()
{
}

void Backend::componentComplete()
{
    // don't bother loading single image until all properties have settled
    m_ready = true;

    // Register the identifier in the data engine
    registerClient();
}

QString Backend::identifier() const
{
    return m_identifier;
}

void Backend::setIdentifier(const QString &identifier)
{
    if (m_identifier == identifier) {
        return;
    }

    if (m_ready) {
        s_engine->unregisterClient(m_identifier, m_args);
    }
    m_identifier = identifier;
    registerClient();

    Q_EMIT identifierChanged();
}

QVariantList Backend::arguments() const
{
    return m_args;
}

void Backend::setArguments(const QVariantList &args)
{
    if (m_args == args) {
        return;
    }

    if (m_ready) {
        s_engine->unregisterClient(m_identifier, m_args);
    }
    m_args = args;
    registerClient();

    Q_EMIT argumentsChanged();
}

QImage Backend::image() const
{
    if (!m_client) {
        return {};
    }

    return m_client->m_data.wallpaperImage;
}

bool Backend::loading() const
{
    if (!m_client) {
        return false;
    }

    return m_client->m_loading;
}

QString Backend::localUrl() const
{
    if (!m_client) {
        return {};
    }

    return m_client->m_data.wallpaperLocalUrl;
}

QUrl Backend::infoUrl() const
{
    if (!m_client) {
        return {};
    }

    return m_client->m_data.wallpaperInfoUrl;
}

QUrl Backend::remoteUrl() const
{
    if (!m_client) {
        return {};
    }

    return m_client->m_data.wallpaperRemoteUrl;
}

QString Backend::title() const
{
    if (!m_client) {
        return {};
    }

    return m_client->m_data.wallpaperTitle;
}

QString Backend::author() const
{
    if (!m_client) {
        return {};
    }

    return m_client->m_data.wallpaperAuthor;
}

void Backend::saveImage()
{
    if (m_client->m_data.wallpaperLocalUrl.isEmpty()) {
        return;
    }

    auto sanitizeFileName = [](const QString &name) {
        if (name.isEmpty()) {
            return name;
        }

        const char notAllowedChars[] = ",^@={}[]~!?:&*\"|#%<>$\"'();`'/\\";
        QString sanitizedName(name);

        for (const char *c = notAllowedChars; *c; c++) {
            sanitizedName.replace(QLatin1Char(*c), QLatin1Char('-'));
        }

        return sanitizedName;
    };

    const QStringList &locations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    const QString path = locations.isEmpty() ? QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0) : locations.at(0);

    // clang-format off
    QString defaultFileName = m_client->m_metadata.name().trimmed();

    if (!m_client->m_data.wallpaperTitle.isEmpty()) {
        defaultFileName += QLatin1Char('-') + m_client->m_data.wallpaperTitle.trimmed();
        if (!m_client->m_data.wallpaperAuthor.isEmpty()) {
            defaultFileName += QLatin1Char('-') + m_client->m_data.wallpaperAuthor.trimmed();
        }
    } else {
        // Use current date
        if (!defaultFileName.isEmpty()) {
            defaultFileName += QLatin1Char('-');
        }
        defaultFileName += QDate::currentDate().toString();
    }

    m_savedUrl = QUrl::fromLocalFile(
        QFileDialog::getSaveFileName(
            nullptr,
            i18nc("@title:window", "Save Today's Picture"),
            path + "/" + sanitizeFileName(defaultFileName) + ".jpg",
            i18nc("@label:listbox Template for file dialog", "JPEG image (*.jpeg *.jpg *.jpe)"),
            nullptr,
            QFileDialog::DontConfirmOverwrite // KIO::CopyJob will show the confirmation dialog.
        )
    );
    // clang-format on

    if (m_savedUrl.isEmpty() || !m_savedUrl.isValid()) {
        return;
    }

    m_savedFolder = QUrl::fromLocalFile(m_savedUrl.toLocalFile().section(QDir::separator(), 0, -2));

    KIO::CopyJob *copyJob = KIO::copy(QUrl::fromLocalFile(m_client->m_data.wallpaperLocalUrl), m_savedUrl, KIO::HideProgressInfo);
    connect(copyJob, &KJob::finished, this, [this](KJob *job) {
        if (job->error()) {
            m_saveStatusMessage = job->errorText();
            if (m_saveStatusMessage.isEmpty()) {
                m_saveStatusMessage = i18nc("@info:status after a save action", "The image was not saved.");
            }
            m_saveStatus = FileOperationStatus::Failed;
            Q_EMIT saveStatusChanged();
        } else {
            m_saveStatusMessage = i18nc("@info:status after a save action %1 file path %2 basename",
                                        "The image was saved as <a href=\"%1\">%2</a>",
                                        m_savedUrl.toString(),
                                        m_savedUrl.fileName());
            m_saveStatus = FileOperationStatus::Successful;
            Q_EMIT saveStatusChanged();
        }
    });
    copyJob->start();
}

void Backend::registerClient()
{
    if (!m_ready) {
        return;
    }

    m_client = s_engine->registerClient(m_identifier, m_args);

    if (!m_client) {
        // Invalid identifier
        return;
    }

    connect(m_client, &Client::imageChanged, this, &Backend::imageChanged);
    connect(m_client, &Client::loadingChanged, this, &Backend::loadingChanged);
    connect(m_client, &Client::localUrlChanged, this, &Backend::localUrlChanged);
    connect(m_client, &Client::infoUrlChanged, this, &Backend::infoUrlChanged);
    connect(m_client, &Client::remoteUrlChanged, this, &Backend::remoteUrlChanged);
    connect(m_client, &Client::titleChanged, this, &Backend::titleChanged);
    connect(m_client, &Client::authorChanged, this, &Backend::authorChanged);

    // Refresh the desktop wallpaper and the information in config dialog
    Q_EMIT imageChanged();
    Q_EMIT loadingChanged();
    Q_EMIT localUrlChanged();
    Q_EMIT infoUrlChanged();
    Q_EMIT remoteUrlChanged();
    Q_EMIT titleChanged();
    Q_EMIT authorChanged();
}
