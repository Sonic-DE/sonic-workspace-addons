/*
 *   SPDX-FileCopyrightText: 2017 Weng Xuetian <wengxt@gmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "bingprovider.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression> // Extract from the copyright text

#include <KIO/Job>
#include <KPluginFactory>

BingProvider::BingProvider(QObject *parent, const QVariantList &args)
    : PotdProvider(parent, args)
{
    const QUrl url(QStringLiteral("https://www.bing.com/HPImageArchive.aspx?format=js&idx=0&n=1"));

    KIO::StoredTransferJob *job = KIO::storedGet(url, KIO::NoReload, KIO::HideProgressInfo);
    connect(job, &KIO::StoredTransferJob::finished, this, &BingProvider::pageRequestFinished);
}

BingProvider::~BingProvider() = default;

void BingProvider::pageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }

    auto json = QJsonDocument::fromJson(job->data());
    do {
        if (json.isNull()) {
            break;
        }
        auto imagesArray = json.object().value(QLatin1String("images"));
        if (!imagesArray.isArray() || imagesArray.toArray().size() <= 0) {
            break;
        }
        auto imageObj = imagesArray.toArray().at(0);
        if (!imageObj.isObject()) {
            break;
        }
        const QJsonObject imageObject = imageObj.toObject();
        auto url = imageObject.value(QLatin1String("url"));
        if (!url.isString() || url.toString().isEmpty()) {
            break;
        }

        // Parse the title and the copyright text from the json data
        // Example copyright text: "草丛中的母狮和它的幼崽，南非 (© Andrew Coleman/Getty Images)"
        const QString copyright = imageObject.value("copyright").toString();
        const QRegularExpression copyrightRegEx("(.+?)[\\(（](.+?)[\\)）]");
        if (const QRegularExpressionMatch match = copyrightRegEx.match(copyright); match.hasMatch()) {
            // In some regions "title" is empty, so extract the title from the copyright text.
            potdProviderData()->wallpaperTitle = match.captured(1).trimmed();
            potdProviderData()->wallpaperAuthor = match.captured(2).remove(QStringLiteral("©")).trimmed();
        }

        const QString title = imageObject.value("title").toString();
        if (!title.isEmpty()) {
            potdProviderData()->wallpaperTitle = title;
        }

        const QString infoUrl = imageObject.value("copyrightlink").toString();
        if (!infoUrl.isEmpty()) {
            potdProviderData()->wallpaperInfoUrl = QUrl(infoUrl);
        }

        potdProviderData()->wallpaperRemoteUrl = QUrl(QStringLiteral("https://www.bing.com/%1").arg(url.toString()));
        KIO::StoredTransferJob *imageJob = KIO::storedGet(potdProviderData()->wallpaperRemoteUrl, KIO::NoReload, KIO::HideProgressInfo);
        connect(imageJob, &KIO::StoredTransferJob::finished, this, &BingProvider::imageRequestFinished);
        return;
    } while (0);

    Q_EMIT error(this);
    return;
}

void BingProvider::imageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }
    QByteArray data = job->data();
    potdProviderData()->wallpaperImage = QImage::fromData(data);
    Q_EMIT finished(this);
}

K_PLUGIN_CLASS_WITH_JSON(BingProvider, "bingprovider.json")

#include "bingprovider.moc"
