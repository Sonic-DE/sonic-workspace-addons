/*
 *   SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "cachedprovider.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QThreadPool>
#include <QTimer>

#include <QDebug>

LoadImageThread::LoadImageThread(const QString &filePath)
    : m_filePath(filePath)
{
}

void LoadImageThread::run()
{
    QImage image;
    image.load(m_filePath);

    QUrl wallpaperInfoUrl;
    QUrl wallpaperRemoteUrl;
    QString wallpaperTitle;
    QString wallpaperAuthor;

    const QString infoPath = m_filePath + ".json";
    QFile infoFile(infoPath);

    if (infoFile.exists()) {
        if (infoFile.open(QIODevice::ReadOnly)) {
            QJsonParseError jsonParseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(infoFile.readAll(), &jsonParseError);
            infoFile.close();

            if (jsonParseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
                const QJsonObject jsonObject = jsonDoc.object();
                wallpaperInfoUrl = QUrl(jsonObject.value(QString(PotdProvider::InfoUrlRole)).toString());
                wallpaperRemoteUrl = QUrl(jsonObject.value(QString(PotdProvider::RemoteUrlRole)).toString());
                wallpaperTitle = jsonObject.value(QString(PotdProvider::TitleRole)).toString();
                wallpaperAuthor = jsonObject.value(QString(PotdProvider::AuthorRole)).toString();
            } else {
                qWarning() << "Failed to read the wallpaper information!";
            }
        } else {
            qWarning() << "Failed to open the wallpaper information file!";
        }
    }

    const std::map<PotdProvider::RoleType, QVariant> dataMap{
        {PotdProvider::ImageRole, image},
        {PotdProvider::InfoUrlRole, wallpaperInfoUrl},
        {PotdProvider::RemoteUrlRole, wallpaperRemoteUrl},
        {PotdProvider::TitleRole, wallpaperTitle},
        {PotdProvider::AuthorRole, wallpaperAuthor},
    };
    Q_EMIT done(dataMap);
}

SaveImageThread::SaveImageThread(const QString &identifier, const std::map<PotdProvider::RoleType, QVariant> &dataMap)
    : m_image(dataMap.at(PotdProvider::ImageRole).value<QImage>())
    , m_identifier(identifier)
    , m_wallpaperInfoUrl(dataMap.at(PotdProvider::InfoUrlRole).toUrl())
    , m_wallpaperRemoteUrl(dataMap.at(PotdProvider::RemoteUrlRole).toUrl())
    , m_wallpaperTitle(dataMap.at(PotdProvider::TitleRole).toString())
    , m_wallpaperAuthor(dataMap.at(PotdProvider::AuthorRole).toString())
{
}

void SaveImageThread::run()
{
    const QString path = CachedProvider::identifierToPath(m_identifier);
    m_image.save(path, "JPEG");

    const QString infoPath = path + ".json";
    QFile infoFile(infoPath);
    if (infoFile.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObject;

        jsonObject.insert(QString(PotdProvider::InfoUrlRole), m_wallpaperInfoUrl.url());
        jsonObject.insert(QString(PotdProvider::RemoteUrlRole), m_wallpaperRemoteUrl.url());
        jsonObject.insert(QString(PotdProvider::TitleRole), m_wallpaperTitle);
        jsonObject.insert(QString(PotdProvider::AuthorRole), m_wallpaperAuthor);

        infoFile.write(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));
        infoFile.close();
    } else {
        qWarning() << "Failed to save the wallpaper information!";
    }

    const std::vector<std::pair<PotdProvider::RoleType, QVariant>> data{
        {PotdProvider::ImageRole, m_image},
        {PotdProvider::UrlRole, path},
        {PotdProvider::InfoUrlRole, m_wallpaperInfoUrl},
        {PotdProvider::RemoteUrlRole, m_wallpaperRemoteUrl},
        {PotdProvider::TitleRole, m_wallpaperTitle},
        {PotdProvider::AuthorRole, m_wallpaperAuthor},
    };
    Q_EMIT done(m_identifier, data);
}

QString CachedProvider::identifierToPath(const QString &identifier)
{
    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QLatin1String("/plasma_engine_potd/");
    QDir d;
    d.mkpath(dataDir);
    return dataDir + identifier;
}

CachedProvider::CachedProvider(const QString &identifier, QObject *parent)
    : PotdProvider(parent)
    , mIdentifier(identifier)
{
    LoadImageThread *thread = new LoadImageThread(identifierToPath(mIdentifier));
    connect(thread, &LoadImageThread::done, this, &CachedProvider::triggerFinished);
    QThreadPool::globalInstance()->start(thread);
}

CachedProvider::~CachedProvider()
{
}

QImage CachedProvider::image() const
{
    return mImage;
}

QString CachedProvider::identifier() const
{
    return mIdentifier;
}

void CachedProvider::triggerFinished(const std::map<PotdProvider::RoleType, QVariant> &dataMap)
{
    mImage = dataMap.at(PotdProvider::ImageRole).value<QImage>();
    if (const QUrl url = dataMap.at(PotdProvider::InfoUrlRole).toUrl(); !url.isEmpty()) {
        m_wallpaperInfoUrl = url;
    }
    if (const QUrl url = dataMap.at(PotdProvider::RemoteUrlRole).toUrl(); !url.isEmpty()) {
        m_wallpaperRemoteUrl = url;
    }
    if (const QString str = dataMap.at(PotdProvider::TitleRole).toString(); !str.isEmpty()) {
        m_wallpaperTitle = str;
    }
    if (const QString str = dataMap.at(PotdProvider::AuthorRole).toString(); !str.isEmpty()) {
        m_wallpaperAuthor = str;
    }
    Q_EMIT finished(this);
}

bool CachedProvider::isCached(const QString &identifier, bool ignoreAge)
{
    const QString path = identifierToPath(identifier);
    if (!QFile::exists(path)) {
        return false;
    }

    QRegularExpression re(QLatin1String(":\\d{4}-\\d{2}-\\d{2}"));

    if (!ignoreAge && !re.match(identifier).hasMatch()) {
        // no date in the identifier, so it's a daily; check to see ifthe modification time is today
        QFileInfo info(path);
        if (info.lastModified().daysTo(QDateTime::currentDateTime()) >= 1) {
            return false;
        }
    }

    return true;
}
