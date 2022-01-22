// SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2021 Guo Yunhe <i@guoyunhe.me>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef POTDPROVIDER_H
#define POTDPROVIDER_H

#include <optional>

#include <QObject>
#include <QUrl>
#include <QVariantList>

#include <KIO/Job>

#include "plasma_potd_export.h"

class QImage;
class QDate;

/**
 * This class is an interface for PoTD providers.
 */
class PLASMA_POTD_EXPORT PotdProvider : public QObject
{
    Q_OBJECT

public:
    /**
     * @see m_dataKeysMap in potd.h
     * @see PotdEngine::sourceRequestEvent in potd.cpp
     */
    enum RoleType {
        ImageRole,
        ImageLoadingStatusRole, /**< The loading status of the image, true if the provider is fetching the image,
                                otherwise false*/
        UrlRole,
        InfoUrlRole, /**< Information URL of the image */
        RemoteUrlRole, /**< Network URL of the image */
        TitleRole, /**< Title of the image */
        AuthorRole, /**< Author of the image */
    };
    Q_ENUM(RoleType)

    /**
     * Creates a new PoTD provider.
     *
     * @param parent The parent object.
     * @param args The arguments.
     */
    explicit PotdProvider(QObject *parent, const QVariantList &args = QVariantList());

    /**
     * Destroys the PoTD provider.
     */
    ~PotdProvider() override;

    /**
     * Returns the requested image.
     *
     * Note: This method returns only a valid image after the
     *       finished() signal has been emitted.
     */
    virtual QImage image() const = 0;

    /**
     * Returns the identifier of the PoTD request (name + date).
     */
    virtual QString identifier() const;

    /**
     * @return the name of this provider (equiv to X-KDE-PlasmaPoTDProvider-Identifier)
     */
    QString name() const;

    /**
     * @return the date to load for this item, if any
     */
    QDate date() const;

    /**
     * Returns the remote URL of the image from the provider
     *
     * @return the remote URL of the image, if any
     * @since 5.25
     */
    std::optional<QUrl> remoteUrl() const;

    /**
     * Returns the information URL of the image from the provider
     *
     * @return the information URL of the image, if any
     * @since 5.25
     */
    std::optional<QUrl> infoUrl() const;

    /**
     * Returns the title of the image from the provider, if any.
     *
     * @return the title of the image, if any
     * @since 5.25
     */
    std::optional<QString> title() const;

    /**
     * Returns the author of the image from the provider
     *
     * @return the title of the image, if any
     * @since 5.25
     */
    std::optional<QString> author() const;

    /**
     * @return if the date is fixed, or if it should always be "today"
     */
    bool isFixedDate() const;

    void refreshConfig();
    void loadConfig();

Q_SIGNALS:
    /**
     * This signal is emitted whenever a request has been finished
     * successfully.
     *
     * @param provider The provider which emitted the signal.
     */
    void finished(PotdProvider *provider);

    /**
     * This signal is emitted whenever an error has occurred.
     *
     * @param provider The provider which emitted the signal.
     */
    void error(PotdProvider *provider);

    void configLoaded(QString apiKey, QString apiSecret);

protected:
    // Wallpaper information
    std::optional<QUrl> m_wallpaperRemoteUrl;
    std::optional<QUrl> m_wallpaperInfoUrl;
    std::optional<QString> m_wallpaperTitle;
    std::optional<QString> m_wallpaperAuthor;

private:
    void configRequestFinished(KJob *job);
    void configWriteFinished(KJob *job);

    const QScopedPointer<class PotdProviderPrivate> d;

    QUrl configRemoteUrl;
    QUrl configLocalUrl;
    QString configLocalPath;
    bool refreshed = false;
};

#endif
