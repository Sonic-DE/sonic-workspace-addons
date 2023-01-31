/*
 *   SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include <QRunnable>

#include "potdprovider.h"

class LoadImageDataThread : public QObject, public QRunnable
{
    Q_OBJECT

public:
    explicit LoadImageDataThread(const QString &filePath);
    void run() override;

Q_SIGNALS:
    void done(const QVariantMap &data);

private:
    QString m_localPath;
};

/**
 * This class provides pictures from the local cache.
 */
class CachedProvider : public PotdProvider
{
    Q_OBJECT

public:
    /**
     * Creates a new cached provider.
     *
     * @param identifier The identifier of the cached picture.
     * @param args The arguments of the identifier.
     * @param parent The parent object.
     */
    CachedProvider(const QString &identifier, const QVariantList &args, QObject *parent);

    /**
     * Returns the identifier of the picture request (name + date).
     */
    QString identifier() const override;

    QString localPath() const override;
    QUrl remoteUrl() const override;
    QUrl infoUrl() const override;
    QString title() const override;
    QString author() const override;

    /**
     * Returns whether a picture with the given @p identifier and @p args is cached.
     */
    static bool isCached(const QString &identifier, const QVariantList &args, bool ignoreAge = false);

    /**
     * Returns a path for the given identifier
     */
    static QString identifierToPath(const QString &identifier, const QVariantList &args);

private Q_SLOTS:
    void slotFinished(const QVariantMap &data);

private:
    QString mIdentifier;
    QVariantList m_args;
    QUrl m_remoteUrl;
    QUrl m_infoUrl;
    QString m_localPath;
    QString m_title;
    QString m_author;
};

class SaveImageThread : public QObject, public QRunnable
{
    Q_OBJECT

public:
    SaveImageThread(const QString &identifier, const QVariantList &args, const QVariantMap &data);
    void run() override;

Q_SIGNALS:
    void done(const QString &localPath);

private:
    QString m_identifier;
    QVariantList m_args;
    QUrl m_remoteUrl;
    QUrl m_infoUrl;
    QString m_title;
    QString m_author;
    QImage m_image;
};
