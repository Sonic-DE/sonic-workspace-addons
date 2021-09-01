/*
 *   SPDX-FileCopyrightText: 2021 Alexey Andreyev <aa13q@ya.ru>
 *
 *   SPDX-License-Identifier: LicenseRef-KDE-Accepted-GPL
 */

#include "simonstalenhagprovider.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QRandomGenerator>

#include <KIO/Job>
#include <KPluginFactory>

SimonStalenhagProvider::SimonStalenhagProvider(QObject *parent, const QVariantList &args)
    : PotdProvider(parent, args)
{
    const QUrl url(QStringLiteral("https://raw.githubusercontent.com/a-andreyev/simonstalenhag-se-metadata/main/entrypoint.json"));

    KIO::StoredTransferJob *job = KIO::storedGet(url, KIO::NoReload, KIO::HideProgressInfo);
    connect(job, &KIO::StoredTransferJob::finished, this, &SimonStalenhagProvider::entrypointRequestFinished);
}

SimonStalenhagProvider::~SimonStalenhagProvider() = default;

QImage SimonStalenhagProvider::image() const
{
    return mImage;
}

void SimonStalenhagProvider::entrypointRequestFinished(KJob *_job)
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
        auto entrypointArray = json.object().value(QLatin1String("simonstalenhag-se-entrypoint"));
        auto entryPointArraySize = entrypointArray.toArray().size();
        if (!entrypointArray.isArray() || entryPointArraySize <= 0) {
            break;
        }
        auto metadataString = entrypointArray.toArray().at(QRandomGenerator::global()->bounded(entryPointArraySize));
        if (!metadataString.isString()) {
            break;
        }
        auto urlStr = metadataString.toString();
        if (urlStr.isEmpty()) {
            break;
        }
        QUrl metaDataUrl(urlStr);
        KIO::StoredTransferJob *metaDataJob = KIO::storedGet(metaDataUrl, KIO::NoReload, KIO::HideProgressInfo);
        connect(metaDataJob, &KIO::StoredTransferJob::finished, this, &SimonStalenhagProvider::metaDataRequestFinished);
        return;
    } while (0);

    Q_EMIT error(this);
    return;
}

void SimonStalenhagProvider::metaDataRequestFinished(KJob *_job)
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
        auto metaDataArray = json.object().value(QLatin1String("simonstalenhag.se"));
        auto metaDataArraySize = metaDataArray.toArray().size();
        if (!metaDataArray.isArray() || metaDataArraySize <= 0) {
            break;
        }
        auto imageObj = metaDataArray.toArray().at(QRandomGenerator::global()->bounded(metaDataArraySize));
        if (!imageObj.isObject()) {
            break;
        }
        auto url = imageObj.toObject().value(QLatin1String("imagebig"));
        if (url.toString().isEmpty()) {
            break;
        }
        QUrl picUrl(url.toString());
        KIO::StoredTransferJob *imageJob = KIO::storedGet(picUrl, KIO::NoReload, KIO::HideProgressInfo);
        connect(imageJob, &KIO::StoredTransferJob::finished, this, &SimonStalenhagProvider::imageRequestFinished);
        return;
    } while (0);

    Q_EMIT error(this);
    return;
}

void SimonStalenhagProvider::imageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }
    QByteArray data = job->data();
    mImage = QImage::fromData(data);
    Q_EMIT finished(this);
}

K_PLUGIN_CLASS_WITH_JSON(SimonStalenhagProvider, "simonstalenhagprovider.json")

#include "simonstalenhagprovider.moc"
