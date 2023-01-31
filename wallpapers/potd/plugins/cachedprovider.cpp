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

#include "debug.h"

LoadImageDataThread::LoadImageDataThread(const QString &filePath)
    : localPath(filePath)
{
}

void LoadImageDataThread::run()
{
    const QString infoPath = localPath + QStringLiteral(".json");
    QFile infoFile(infoPath);

    if (infoFile.exists()) {
        if (infoFile.open(QIODevice::ReadOnly)) {
            QJsonParseError jsonParseError;
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(infoFile.readAll(), &jsonParseError);
            infoFile.close();

            if (jsonParseError.error == QJsonParseError::NoError && jsonDoc.isObject()) {
                const QJsonObject jsonObject = jsonDoc.object();
                infoUrl = QUrl(jsonObject.value(QStringLiteral("InfoUrl")).toString());
                remoteUrl = QUrl(jsonObject.value(QStringLiteral("RemoteUrl")).toString());
                title = jsonObject.value(QStringLiteral("Title")).toString();
                author = jsonObject.value(QStringLiteral("Author")).toString();
            } else {
                qCWarning(WALLPAPERPOTD) << "Failed to read the wallpaper information!";
            }
        } else {
            qCWarning(WALLPAPERPOTD) << "Failed to open the wallpaper information file!";
        }
    }

    Q_EMIT done(this);
}

SaveImageThread::SaveImageThread(const QString &identifier, const QVariantList &args, const QVariantMap &data)
    : m_identifier(identifier)
    , m_args(args)
    , m_remoteUrl(data[QStringLiteral("RemoteUrl")].toUrl())
    , m_infoUrl(data[QStringLiteral("InfoUrl")].toUrl())
    , m_title(data[QStringLiteral("Title")].toString())
    , m_author(data[QStringLiteral("Author")].toString())
    , m_image(data[QStringLiteral("Image")].value<QImage>())
{
}

void SaveImageThread::run()
{
    const QString localPath = CachedProvider::identifierToPath(m_identifier, m_args);
    m_image.save(localPath, "JPEG");

    const QString infoPath = localPath + ".json";
    QFile infoFile(infoPath);
    if (infoFile.open(QIODevice::WriteOnly)) {
        QJsonObject jsonObject;

        jsonObject.insert(QStringLiteral("InfoUrl"), m_infoUrl.url());
        jsonObject.insert(QStringLiteral("RemoteUrl"), m_remoteUrl.url());
        jsonObject.insert(QStringLiteral("Title"), m_title);
        jsonObject.insert(QStringLiteral("Author"), m_author);

        infoFile.write(QJsonDocument(jsonObject).toJson(QJsonDocument::Compact));
        infoFile.close();
    } else {
        qWarning() << "Failed to save the wallpaper information!";
    }

    Q_EMIT done(localPath);
}

QString CachedProvider::identifierToPath(const QString &identifier, const QVariantList &args)
{
    const QString argString = std::accumulate(args.cbegin(), args.cend(), QString(), [](const QString &s, const QVariant &arg) {
        if (arg.canConvert(QMetaType::QString)) {
            return s + QStringLiteral(":%1").arg(arg.toString());
        }

        return s;
    });

    const QString dataDir = QStandardPaths::writableLocation(QStandardPaths::GenericCacheLocation) + QLatin1String("/plasma_engine_potd/");
    QDir d;
    d.mkpath(dataDir);
    return QStringLiteral("%1%2%3").arg(dataDir, identifier, argString);
}

CachedProvider::CachedProvider(const QString &identifier, const QVariantList &args, QObject *parent)
    : PotdProvider(parent, KPluginMetaData(), QVariantList())
    , mIdentifier(identifier)
    , m_args(args)
{
    LoadImageDataThread *thread = new LoadImageDataThread(CachedProvider::identifierToPath(mIdentifier, m_args));
    connect(thread, &LoadImageDataThread::done, this, &CachedProvider::slotFinished);
    QThreadPool::globalInstance()->start(thread);
}

QString CachedProvider::identifier() const
{
    return mIdentifier;
}

QString CachedProvider::localPath() const
{
    return m_localPath;
}

QUrl CachedProvider::remoteUrl() const
{
    return m_remoteUrl;
}

QUrl CachedProvider::infoUrl() const
{
    return m_infoUrl;
}

QString CachedProvider::title() const
{
    return m_title;
}

QString CachedProvider::author() const
{
    return m_author;
}

void CachedProvider::slotFinished(LoadImageDataThread *thread)
{
    m_localPath = thread->localPath;
    m_infoUrl = thread->infoUrl;
    m_remoteUrl = thread->remoteUrl;
    m_title = thread->title;
    m_author = thread->author;

    Q_EMIT finished(this, QImage());
}

bool CachedProvider::isCached(const QString &identifier, const QVariantList &args, bool ignoreAge)
{
    const QString path = CachedProvider::identifierToPath(identifier, args);
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
