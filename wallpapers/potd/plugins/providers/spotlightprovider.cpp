/*
 *   SPDX-FileCopyrightText: 2024 Amedeo Amato <amato.amedeo@gmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "spotlightprovider.h"

#include <QUrlQuery>
#include <QJsonDocument>

#include <KIO/StoredTransferJob>
#include <KPluginFactory>

using namespace Qt::Literals::StringLiterals;

SpotlightProvider::SpotlightProvider(QObject *parent, const KPluginMetaData &data, const QVariantList &args)
    : PotdProvider(parent, data, args)
{
    const QUrl url = buildUrl();
    auto job = KIO::storedGet(url, KIO::NoReload, KIO::HideProgressInfo);
    connect(job, &KIO::StoredTransferJob::finished, this, &SpotlightProvider::pageRequestFinished);
}

void SpotlightProvider::pageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }

    const QJsonValue wallpaperItem = getWallpaperItem(job->data());
    if (!wallpaperItem.isNull()) {
        auto imageUrl = wallpaperItem["image_fullscreen_001_landscape"_L1]["u"_L1].toString();
        m_remoteUrl = QUrl(imageUrl);

        m_title = wallpaperItem["title_text"_L1]["tx"_L1].toString();
        m_author = wallpaperItem["copyright_text"_L1]["tx"_L1].toString();

        auto infoUrl = wallpaperItem["title_destination_url"_L1]["u"_L1].toString();
        if (infoUrl.startsWith("microsoft-edge:"_L1)) {
            infoUrl.remove(0, 15); // Remove "microsoft-edge:" prefix if Microsoft tries to force the browser (depending on country)
        }
        m_infoUrl = QUrl(infoUrl);

        KIO::StoredTransferJob *imageJob = KIO::storedGet(m_remoteUrl, KIO::NoReload, KIO::HideProgressInfo);
        connect(imageJob, &KIO::StoredTransferJob::finished, this, &SpotlightProvider::imageRequestFinished);
        return;
    }
        
    Q_EMIT error(this);
}

void SpotlightProvider::imageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }

    auto image = QImage::fromData(job->data());
    if (image.isNull()) {
         Q_EMIT error(this);
        return;
    }

    Q_EMIT finished(this, image);
}

QUrl SpotlightProvider::buildUrl()
{
    QUrl url("https://arc.msn.com/v3/Delivery/Placement"_L1);
    
    QUrlQuery query;
    // User agent
    query.addQueryItem("ua"_L1, "KDE Plasma"_L1);
    // Unknown but must be those exact values
    query.addQueryItem("pid"_L1, "209567"_L1);
    query.addQueryItem("rafb"_L1, "0"_L1);
    query.addQueryItem("cdm"_L1, "1"_L1);
    query.addQueryItem("lo"_L1, "80217"_L1);
    // Output format
    query.addQueryItem("fmt"_L1, "json"_L1);
    // Screen width in pixels
    query.addQueryItem("disphorzres"_L1, "9999"_L1);
    // Screen height in pixels
    query.addQueryItem("dispvertres"_L1, "9999"_L1);
    // Locale
    QString languageCode = QLocale().name().replace("_", "-");
    query.addQueryItem("pl"_L1, languageCode);
    // Language
    query.addQueryItem("lc"_L1, languageCode);
    // Country
    QString countryCode = languageCode.split('-').at(1).toLower();
    query.addQueryItem("ctry"_L1, countryCode);
    // Time
    QDateTime now = QDateTime::currentDateTime();
    query.addQueryItem("time"_L1, now.toUTC().toString(Qt::ISODate));

    url.setQuery(query);
    return url;
}

QJsonValue SpotlightProvider::getWallpaperItem(QByteArray data)
{
    QJsonDocument json = QJsonDocument::fromJson(data);
    QString itemStr = json["batchrsp"_L1]["items"_L1][0]["item"_L1].toString();
    QJsonValue wallpaperItem = QJsonDocument::fromJson(itemStr.toUtf8())["ad"_L1];
    return wallpaperItem;
}

K_PLUGIN_CLASS_WITH_JSON(SpotlightProvider, "spotlightprovider.json")

#include "spotlightprovider.moc"
