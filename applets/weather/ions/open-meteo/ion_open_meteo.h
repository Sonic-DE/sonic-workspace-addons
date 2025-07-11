/*
    SPDX-FileCopyrightText: 2025 tusooa <tusooa@kazv.moe>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "../ion.h"
#include <memory>
Q_MOC_INCLUDE("forecast.h")
Q_MOC_INCLUDE("locations.h")

class KJob;
class Forecast;
class Locations;

/**
 * open-meteo.com
 */
class Q_DECL_EXPORT OpenMeteoIon : public Ion
{
    Q_OBJECT

public:
    explicit OpenMeteoIon(QObject *parent = nullptr);
    ~OpenMeteoIon() override;

    void findPlaces(std::shared_ptr<QPromise<std::shared_ptr<Locations>>> promise, const QString &searchString) override;

    void fetchForecast(std::shared_ptr<QPromise<std::shared_ptr<Forecast>>> promise, const QString &placeInfo) override;

private:
    struct Private;
    std::unique_ptr<Private> m_d;
};
