// SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2008 Anne-Marie Mahfouf <annma@kde.org>
// SPDX-FileCopyrightText: 2008 Georges Toth <gtoth@trypill.org>
// SPDX-FileCopyrightText: 2021 Guo Yunhe <i@guoyunhe.me>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "flickrprovider.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QUrlQuery>

#include <KIO/Job>
#include <KPluginFactory>

static QUrl buildUrl(const QDate &date, const QString apiKey)
{
    QUrl url(QLatin1String("https://api.flickr.com/services/rest/"));
    QUrlQuery urlQuery(url);
    urlQuery.addQueryItem(QStringLiteral("api_key"), apiKey);
    urlQuery.addQueryItem(QStringLiteral("method"), QStringLiteral("flickr.interestingness.getList"));
    urlQuery.addQueryItem(QStringLiteral("date"), date.toString(Qt::ISODate));
    // url_o might be either too small or too large.
    urlQuery.addQueryItem(QStringLiteral("extras"), QStringLiteral("url_k,url_h,url_o"));
    url.setQuery(urlQuery);

    return url;
}

FlickrProvider::FlickrProvider(QObject *parent, const QVariantList &args)
    : PotdProvider(parent, args)
{
    connect(this, &PotdProvider::configLoaded, this, &FlickrProvider::sendXmlRequest);

    loadConfig();
}

FlickrProvider::~FlickrProvider() = default;

QImage FlickrProvider::image() const
{
    return mImage;
}

void FlickrProvider::sendXmlRequest(QString apiKey, QString apiSecret)
{
    Q_UNUSED(apiSecret);
    if (apiKey.isNull()) {
        refreshConfig();
        return;
    }

    mApiKey = apiKey;
    mActualDate = date().addDays(-2);

    const QUrl xmlUrl = buildUrl(mActualDate, apiKey);

    KIO::StoredTransferJob *xmlJob = KIO::storedGet(xmlUrl, KIO::NoReload, KIO::HideProgressInfo);
    connect(xmlJob, &KIO::StoredTransferJob::finished, this, &FlickrProvider::xmlRequestFinished);
}

void FlickrProvider::xmlRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        qDebug() << "xmlRequestFinished error";
        refreshConfig();
        return;
    }

    const QString data = QString::fromUtf8(job->data());

    // Clear the list
    m_photoList.clear();

    xml.clear();
    xml.addData(data);

    while (!xml.atEnd()) {
        xml.readNext();

        if (xml.isStartElement()) {
            auto attributes = xml.attributes();
            if (xml.name() == QLatin1String("rsp")) {
                /* no pictures available for the specified parameters */
                if (attributes.value(QLatin1String("stat")).toString() != QLatin1String("ok")) {
                    Q_EMIT error(this);
                    qDebug() << "xmlRequestFinished error: no photos for the query";
                    return;
                }
            } else if (xml.name() == QLatin1String("photo")) {
                if (attributes.value(QLatin1String("ispublic")).toString() != QLatin1String("1")) {
                    continue;
                }

                const char *fallbackList[] = {"url_k", "url_h"};

                bool found = false;
                for (auto urlAttr : fallbackList) {
                    // Get the best url.
                    QLatin1String urlAttrString(urlAttr);
                    if (attributes.hasAttribute(urlAttrString)) {
                        m_photoList.append(attributes.value(urlAttrString).toString());
                        found = true;
                        break;
                    }
                }

                // The logic here is, if url_h or url_k are present, url_o must
                // has higher quality, otherwise, url_o is worse than k/h size.
                // If url_o is better, prefer url_o.
                if (found) {
                    QLatin1String originAttr("url_o");
                    if (attributes.hasAttribute(originAttr)) {
                        m_photoList.back() = attributes.value(QLatin1String(originAttr)).toString();
                    }
                }

                // Parse title from XML tag
                if (attributes.hasAttribute("title")) {
                    m_wallpaperTitle = attributes.value("title").toString();
                }

                /**
                 * Visit the photo page to get the author
                 * API document: https://www.flickr.com/services/api/misc.urls.html
                 * https://www.flickr.com/photos/{user-id}/{photo-id}
                 */
                if (attributes.hasAttribute("id") && attributes.hasAttribute("owner")) {
                    const QString userId = attributes.value("owner").toString();
                    const QString photoId = attributes.value("id").toString();
                    m_infoPageUrl = QUrl(QStringLiteral("https://www.flickr.com/photos/%1/%2").arg(userId).arg(photoId));
                }
            }
        }
    }

    if (xml.error() && xml.error() != QXmlStreamReader::PrematureEndOfDocumentError) {
        qWarning() << "XML ERROR:" << xml.lineNumber() << ": " << xml.errorString();
    }

    if (m_photoList.begin() != m_photoList.end()) {
        m_wallpaperRemoteUrl = QUrl(m_photoList.at(QRandomGenerator::global()->bounded(m_photoList.size())));
        KIO::StoredTransferJob *imageJob = KIO::storedGet(m_wallpaperRemoteUrl.value(), KIO::NoReload, KIO::HideProgressInfo);
        connect(imageJob, &KIO::StoredTransferJob::finished, this, &FlickrProvider::imageRequestFinished);
    } else {
        qDebug() << "empty list";
    }
}

void FlickrProvider::imageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT error(this);
        return;
    }

    // Visit the photo page to get the author
    if (m_infoPageUrl.has_value()) {
        KIO::StoredTransferJob *pageJob = KIO::storedGet(m_infoPageUrl.value(), KIO::NoReload, KIO::HideProgressInfo);
        connect(pageJob, &KIO::StoredTransferJob::finished, this, &FlickrProvider::pageRequestFinished);
    }

    mImage = QImage::fromData(job->data());
}

void FlickrProvider::pageRequestFinished(KJob *_job)
{
    KIO::StoredTransferJob *job = static_cast<KIO::StoredTransferJob *>(_job);
    if (job->error()) {
        Q_EMIT finished(this); // No author is fine
        return;
    }

    const QString data = QString::fromUtf8(job->data()).simplified();

    // Example: <a href="/photos/jellybeanzgallery/" class="owner-name truncate" title="Go to Hammerchewer&#x27;s photostream"
    // data-track="attributionNameClick">Hammerchewer</a>
    QRegularExpression authorRegEx("<a.*?class=\"owner-name truncate\".*?>(.+?)</a>");
    QRegularExpressionMatch match = authorRegEx.match(data);

    if (match.hasMatch()) {
        m_wallpaperAuthor = match.captured(1);
    }

    Q_EMIT finished(this);
}

K_PLUGIN_CLASS_WITH_JSON(FlickrProvider, "flickrprovider.json")

#include "flickrprovider.moc"
