/*
 *   SPDX-FileCopyrightText: 2024 Amedeo Amato <amato.amedeo@gmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

#include "potdprovider.h"

#include <QJsonValue>
#include <QByteArray>

class KJob;

/**
 * PoTD provider that uses "Windows Spotlight" to provide different background images.
 * The images are not strictly "daily" as some randomness seems to be involved.
 */
class SpotlightProvider : public PotdProvider
{
    Q_OBJECT

public:
    explicit SpotlightProvider(QObject *parent, const KPluginMetaData &data, const QVariantList &args);

private:
    void pageRequestFinished(KJob *job);
    void imageRequestFinished(KJob *job);

    /**
     * Builds the URL to query the Microsoft/Spotlight API for a new wallpaper.
     */
    QUrl buildUrl();

    /**
     * Gets the first wallpaper result from the JSON data returned by the API.
     */
    QJsonValue getWallpaperItem(QByteArray data);
};
