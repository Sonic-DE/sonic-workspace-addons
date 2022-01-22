/*
 *   SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
 *   SPDX-FileCopyrightText: 2008 Anne-Marie Mahfouf <annma@kde.org>
 *   SPDX-FileCopyrightText: 2013 Aaron Seigo <aseigo@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "natgeoprovider.h"

#include <QDebug>

#include <KIO/Job>
#include <KPluginFactory>

NatGeoProvider::NatGeoProvider(QObject *parent, const QVariantList &args)
    : PotdProvider(parent, args)
{
    const QUrl url(QStringLiteral("https://www.nationalgeographic.com/photography/photo-of-the-day/"));

    KIO::StoredTransferJob *job = KIO::storedGet(url, KIO::NoReload, KIO::HideProgressInfo);
    connect(job, &KIO::StoredTransferJob::finished, this, &NatGeoProvider::pageRequestFinished);
}

NatGeoProvider::~NatGeoProvider() = default;

QImage NatGeoProvider::image() const
{
    return mImage;
}

void NatGeoProvider::pageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }

    const QString data = QString::fromUtf8(job->data());
    const QStringList lines = data.split(QLatin1Char('\n'));

    re.setPattern(QStringLiteral("<meta\\s+(?:\\S+=[\"']?(?:.(?![\"']?\\s+(?:\\S+)=|\\s*/?[>\"']))+.[\"']?\\s*)*property=\"og:image\"\\s*(?:\\S+=[\"']?(?:.(?![\"']?\\s+(?:\\S+)=|\\s*/?[>\"']))+.[\"']?\\s*)*content=[\"']?((?:.(?![\"']?\\s+(?:\\S+)=|\\s*/?[>\"']))+.)[\"']?\\s*(?:\\S+=[\"']?(?:.(?![\"']?\\s+(?:\\S+)=|\\s*/?[>\"']))+.[\"']?\\s*)*/>"));

    for (int i = 0; i < lines.size(); i++) {
        QRegularExpressionMatch match = re.match(lines.at(i));
        if (match.hasMatch()) {
            m_wallpaperRemoteUrl = QUrl(match.captured(1));
        }
    }

    if (!m_wallpaperRemoteUrl.has_value() || m_wallpaperRemoteUrl->isEmpty()) {
        Q_EMIT error(this);
        return;
    }

    const QString simplifiedData(data.simplified());

    /**
     * Match title
     * Example:
     * <p class="Caption__Title" aria-hidden="false">Destiny and Daisy</p>
     */
    const QRegularExpression titleRegEx("<p class=\"Caption__Title\".*?>(.+?)</p>");
    const QRegularExpressionMatch titleMatch = titleRegEx.match(simplifiedData);
    if (titleMatch.hasMatch()) {
        m_wallpaperTitle = titleMatch.captured(1).trimmed();
    }

    /**
     * Match author
     * Example:
     * <span aria-label="Photograph by Erika Larsen, Nat Geo Image Collection" class="RichText Caption__Credit">Photograph by Erika Larsen, Nat Geo Image
     * Collection</span>
     */
    const QRegularExpression authorRegEx("<span.*?class=\".*?Caption__Credit.*?\".*?>(.+?)</span>");
    const QRegularExpressionMatch authorMatch = authorRegEx.match(simplifiedData);
    if (authorMatch.hasMatch()) {
        m_wallpaperAuthor = authorMatch.captured(1).remove("Photograph by").trimmed();
    }

    KIO::StoredTransferJob *imageJob = KIO::storedGet(m_wallpaperRemoteUrl.value(), KIO::NoReload, KIO::HideProgressInfo);
    connect(imageJob, &KIO::StoredTransferJob::finished, this, &NatGeoProvider::imageRequestFinished);
}

void NatGeoProvider::imageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }

    mImage = QImage::fromData(job->data());
    Q_EMIT finished(this);
}

K_PLUGIN_CLASS_WITH_JSON(NatGeoProvider, "natgeoprovider.json")

#include "natgeoprovider.moc"
