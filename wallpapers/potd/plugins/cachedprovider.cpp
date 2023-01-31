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
#include <QThreadPool>
#include <QTimer>

#include <QDebug>
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

CachedProvider::CachedProvider(const QString &identifier, const QVariantList &args, QObject *parent)
    : PotdProvider(parent, KPluginMetaData(), QVariantList())
    , mIdentifier(identifier)
    , m_args(args)
{
    LoadImageDataThread *thread = new LoadImageDataThread(PotdProviderUtils::identifierToPath(mIdentifier, m_args));
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

    Q_EMIT finished(this);
}

bool CachedProvider::isCached(const QString &identifier, const QVariantList &args, bool ignoreAge)
{
    const QString path = PotdProviderUtils::identifierToPath(identifier, args);
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
