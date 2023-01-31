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

    QUrl remoteUrl;
    QUrl infoUrl;
    QString localPath;
    QString title;
    QString author;

Q_SIGNALS:
    void done(LoadImageDataThread *thread);
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

private Q_SLOTS:
    void slotFinished(LoadImageDataThread *thread);

private:
    QString mIdentifier;
    QVariantList m_args;
    QUrl m_remoteUrl;
    QUrl m_infoUrl;
    QString m_localPath;
    QString m_title;
    QString m_author;
};
