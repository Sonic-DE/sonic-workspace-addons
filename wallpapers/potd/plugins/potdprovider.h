// SPDX-FileCopyrightText: 2007 Tobias Koenig <tokoe@kde.org>
// SPDX-FileCopyrightText: 2021 Guo Yunhe <i@guoyunhe.me>
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QImage>
#include <QUrl>
#include <QVariantList>

#include <KPluginMetaData>

#include "plasma_potd_export.h"

class QDate;

namespace PotdProviderUtils
{
/**
 * Returns a path for the given identifier
 */
QString identifierToPath(const QString &identifier, const QVariantList &args);
}

/**
 * This class is an interface for PoTD providers.
 */
class PLASMA_POTD_EXPORT PotdProvider : public QObject
{
    Q_OBJECT

public:
    /**
     * Creates a new PoTD provider.
     *
     * @param parent The parent object.
     * @param data The metadata of the plugin
     * @param args The arguments.
     * @since 5.25
     */
    explicit PotdProvider(QObject *parent, const KPluginMetaData &data, const QVariantList &args);

    /**
     * Destroys the PoTD provider.
     */
    virtual ~PotdProvider() override;

    /**
     * Returns the local path of the requested image.
     *
     * Note: This method returns only a valid path after the
     *       finished() signal has been emitted.
     */
    virtual QString localPath() const;

    /**
     * Returns the identifier of the PoTD request (name + date).
     */
    virtual QString identifier() const;

    /**
     * Returns the remote URL of the image from the provider
     *
     * @note No @c virtual to keep binary compatibility.
     * @return the remote URL of the image, if any
     * @since 5.25
     */
    virtual QUrl remoteUrl() const;

    /**
     * Returns the information URL of the image from the provider
     *
     * @return the information URL of the image, if any
     * @since 5.25
     */
    virtual QUrl infoUrl() const;

    /**
     * Returns the title of the image from the provider, if any.
     *
     * @return the title of the image, if any
     * @since 5.25
     */
    virtual QString title() const;

    /**
     * Returns the author of the image from the provider
     *
     * @return the title of the image, if any
     * @since 5.25
     */
    virtual QString author() const;

    /**
     * @return the name of this provider (equiv to X-KDE-PlasmaPoTDProvider-Identifier)
     */
    virtual QString name() const;

    /**
     * @return the date to load for this item, if any
     */
    virtual QDate date() const;

Q_SIGNALS:
    /**
     * This signal is emitted whenever a request has been finished
     * successfully.
     *
     * @param provider The provider which emitted the signal.
     */
    void finished(PotdProvider *provider);

    /**
     * This signal is emitted whenever the image and the data are saved to local
     *
     * @param provider The provider which emitted the signal.
     * @since 6.0
     */
    void cached(PotdProvider *provider, const QString &localPath);

    /**
     * This signal is emitted whenever an error has occurred.
     *
     * @param provider The provider which emitted the signal.
     */
    void error(PotdProvider *provider);

protected:
    /**
     * Saves the current image with its information to local
     */
    void save(const QImage &image, const QVariantList &args);

    std::unique_ptr<class PotdProviderPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(PotdProvider)
};
