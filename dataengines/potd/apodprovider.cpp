/*
 *   SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
 *   SPDX-FileCopyrightText: 2008 Anne-Marie Mahfouf <annma@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "apodprovider.h"

#include <QDebug>
#include <QRegularExpression>

#include <KIO/Job>
#include <KPluginFactory>

ApodProvider::ApodProvider(QObject *parent, const QVariantList &args)
    : PotdProvider(parent, args)
{
    const QUrl url(QStringLiteral("http://antwrp.gsfc.nasa.gov/apod/"));

    KIO::StoredTransferJob *job = KIO::storedGet(url, KIO::NoReload, KIO::HideProgressInfo);
    connect(job, &KIO::StoredTransferJob::finished, this, &ApodProvider::pageRequestFinished);
}

ApodProvider::~ApodProvider() = default;

QImage ApodProvider::image() const
{
    return mImage;
}

void ApodProvider::pageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }

    const QString data = QString::fromUtf8(job->data()).simplified(); // Join lines so title match can work

    const QString pattern = QStringLiteral("<a href=\"(image/.*)\"");
    QRegExp exp(pattern);
    exp.setMinimal(true);
    if (exp.indexIn(data) != -1) {
        const QString sub = exp.cap(1);
        m_wallpaperRemoteUrl = QUrl(QLatin1String("http://antwrp.gsfc.nasa.gov/apod/") + sub);

        /**
         * Match title and author
         * Example:
         * <b> The Full Moon and the Dancer </b> <br>
         *
         * <b>Image Credit &
         * <a href="lib/about_apod.html#srapply">Copyright</a>:</b>
         *
         * <a href="https://www.instagram.com/through_my_lens_84/">Elena Pinna</a>
         */
        const QRegularExpression infoRegEx("<b>(.+?)</b>.*?<br>.*?Copyright.*?<a.+?>(.+?)</a>");
        const QRegularExpressionMatch match = infoRegEx.match(data);

        if (match.hasMatch()) {
            m_wallpaperTitle = match.captured(1).trimmed();
            m_wallpaperAuthor = match.captured(2).trimmed();
        }

        KIO::StoredTransferJob *imageJob = KIO::storedGet(m_wallpaperRemoteUrl.value(), KIO::NoReload, KIO::HideProgressInfo);
        connect(imageJob, &KIO::StoredTransferJob::finished, this, &ApodProvider::imageRequestFinished);
    } else {
        Q_EMIT error(this);
    }
}

void ApodProvider::imageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }

    mImage = QImage::fromData(job->data());
    Q_EMIT finished(this);
}

K_PLUGIN_CLASS_WITH_JSON(ApodProvider, "apodprovider.json")

#include "apodprovider.moc"
