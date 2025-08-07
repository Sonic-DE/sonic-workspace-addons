/*
    SPDX-FileCopyrightText: 2025 tusooa <tusooa@kazv.moe>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ion_open_meteo.h"

#include "ion_open_meteodebug.h"

#include <KIO/StoredTransferJob>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KUnitConversion/Value>

#include <QDate>
#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QTimeZone>
#include <QUrl>
#include <QUrlQuery>

#include <optional>

using namespace Qt::StringLiterals;

static const QString SEARCH_ENDPOINT = u"https://geocoding-api.open-meteo.com/v1/search"_s;
static const QString WEATHER_ENDPOINT = u"https://api.open-meteo.com/v1/forecast"_s;
static const QString CREDIT_URL = u"https://open-meteo.com"_s;
static const QUrlQuery WEATHER_QUERY{
    {u"daily"_s, u"weather_code,temperature_2m_max,temperature_2m_min,wind_direction_10m_dominant,precipitation_probability_max"_s},
    {u"current"_s,
     u"temperature_2m,weather_code,wind_speed_10m,wind_direction_10m,relative_humidity_2m,apparent_temperature,is_day,wind_speed_10m,wind_direction_10m,wind_gusts_10m,surface_pressure,relative_humidity_2m"_s},
    {u"timezone"_s, u"auto"_s}};
static const QStringList ADMIN_AREA_KEYS{
    u"admin1"_s,
    u"admin2"_s,
    u"admin3"_s,
    u"admin4"_s,
};
// taken from ion_noaa
static const QStringList WIND_DIRECTION_STRS{
    u"N"_s,
    u"NNE"_s,
    u"NE"_s,
    u"ENE"_s,
    u"E"_s,
    u"SSE"_s,
    u"SE"_s,
    u"S"_s,
    u"SSW"_s,
    u"SW"_s,
    u"WSW"_s,
    u"W"_s,
    u"WNW"_s,
    u"NW"_s,
    u"NNW"_s,
};

// https://open-meteo.com/en/docs/geocoding-api
static QUrl searchPlacesUrl(const QString &placeQuery, const QString &language)
{
    QUrl url(SEARCH_ENDPOINT);
    QUrlQuery urlQuery{
        {u"name"_s, placeQuery},
        {u"language"_s, language},
    };
    url.setQuery(urlQuery);
    return url;
}

// https://open-meteo.com/en/docs
static QUrl getWeatherUrl(const QString &latitude, const QString &longitude)
{
    QUrl url(WEATHER_ENDPOINT);
    QUrlQuery urlQuery = WEATHER_QUERY;
    urlQuery.addQueryItem(u"latitude"_s, latitude);
    urlQuery.addQueryItem(u"longitude"_s, longitude);
    url.setQuery(urlQuery);
    return url;
}

static std::optional<QJsonObject> validateJob(KJob *job)
{
    auto jobCast = qobject_cast<KIO::StoredTransferJob *>(job);
    if (!jobCast) {
        return std::nullopt;
    }
    if (jobCast->error()) {
        return std::nullopt;
    }
    auto jd = QJsonDocument::fromJson(jobCast->data());
    if (!jd.isObject()) {
        return std::nullopt;
    }
    return jd.object();
}

static QString formatPlaceName(const QJsonObject &place)
{
    QStringList names;
    auto country = place[u"country"_s].toString();
    if (!country.isEmpty()) {
        names.append(country);
    }

    for (const auto &k : ADMIN_AREA_KEYS) {
        auto adminAreaName = place[k].toString();
        if (!adminAreaName.isEmpty()) {
            names.append(adminAreaName);
        }
    }
    names.append(place[u"name"_s].toString());

    QString translatedTemplate;
    if (names.size() == 6) {
        translatedTemplate = i18nc("place name, %6 is the smallest area and %1 the largest", "%6, %5, %4, %3, %2, %1");
    } else if (names.size() == 5) {
        translatedTemplate = i18nc("place name, %5 is the smallest area and %1 the largest", "%5, %4, %3, %2, %1");
    } else if (names.size() == 4) {
        translatedTemplate = i18nc("place name, %4 is the smallest area and %1 the largest", "%4, %3, %2, %1");
    } else if (names.size() == 3) {
        translatedTemplate = i18nc("place name, %3 is the smallest area and %1 the largest", "%3, %2, %1");
    } else if (names.size() == 2) {
        translatedTemplate = i18nc("place name, %2 is the smallest area and %1 the largest", "%2, %1");
    } else if (names.size() == 1) {
        translatedTemplate = i18nc("place name", "%1");
    } else {
        translatedTemplate = u""_s;
    }

    for (const auto &name : names) {
        translatedTemplate = translatedTemplate.arg(name);
    }
    return translatedTemplate;
}

// https://en.wikipedia.org/wiki/Wind_direction
// open-meteo returns the direction in degrees, but
// weather applet expects a string direction
static QString windDirectionToStr(double dir)
{
    auto index = qRound(dir / 22.5) % 16;
    return WIND_DIRECTION_STRS[index];
}

template<class T>
static void resolvePromise(QPromise<std::decay_t<T>> &promise, T &&result)
{
    promise.addResult(std::forward<T>(result));
    promise.finish();
}

template<class T>
static void cancelPromise(QPromise<std::decay_t<T>> &promise)
{
    promise.cancel();
}

struct OpenMeteoIon::Private {
    QHash<int, QString> weatherCodeToStrMapCommon;
    QHash<int, QString> weatherCodeToStrMapDay;
    QHash<int, QString> weatherCodeToStrMapNight;
    QHash<int, ConditionIcons> weatherCodeToIconMapCommon{
        {3, Overcast},
        {30, Overcast},
        {4, Haze},
        {40, Haze},
        {45, Haze},
        {48, Haze},
        {49, Haze},
        {5, Mist},
        {50, Mist},
        {51, Mist},
        {53, Mist},
        {55, Mist},
        {56, FreezingDrizzle},
        {57, FreezingDrizzle},
        {6, Rain},
        {60, LightRain},
        {61, LightRain},
        {63, Rain},
        {65, Rain},
        {66, FreezingRain},
        {67, FreezingRain},
        {68, RainSnow},
        {69, RainSnow},
        {7, Snow},
        {70, LightSnow},
        {71, LightSnow},
        {73, Snow},
        {75, Flurries},
        {8, Showers},
        {81, Showers},
        {82, Showers},
        {83, RainSnow},
        {84, RainSnow},
        {85, Snow},
        {86, Snow},
        {9, Thunderstorm},
        {90, Thunderstorm},
        {96, Thunderstorm},
        {999, NotAvailable},
    };
    QHash<int, ConditionIcons> weatherCodeToIconMapDay{
        {0, ClearDay},
        {1, FewCloudsDay},
        {10, FewCloudsDay},
        {2, PartlyCloudyDay},
        {20, PartlyCloudyDay},
        {80, ChanceShowersDay},
        {95, ChanceThunderstormDay},
    };
    QHash<int, ConditionIcons> weatherCodeToIconMapNight{
        {0, ClearNight},
        {1, FewCloudsNight},
        {10, FewCloudsNight},
        {2, PartlyCloudyNight},
        {20, PartlyCloudyNight},
        {80, ChanceShowersNight},
        {95, ChanceThunderstormNight},
    };

    Private()
        // XXX: copied from wettercom
        : weatherCodeToStrMapCommon{
              {1, i18nc("weather condition", "few clouds")},
              {10, i18nc("weather condition", "few clouds")},
              {2, i18nc("weather condition", "cloudy")},
              {20, i18nc("weather condition", "cloudy")},
              {3, i18nc("weather condition", "overcast")},
              {30, i18nc("weather condition", "overcast")},
              {4, i18nc("weather condition", "haze")},
              {40, i18nc("weather condition", "haze")},
              {45, i18nc("weather condition", "haze")},
              {48, i18nc("weather condition", "fog with icing")},
              {49, i18nc("weather condition", "fog with icing")},
              {5, i18nc("weather condition", "drizzle")},
              {50, i18nc("weather condition", "drizzle")},
              {51, i18nc("weather condition", "light drizzle")},
              {53, i18nc("weather condition", "drizzle")},
              {55, i18nc("weather condition", "heavy drizzle")},
              {56, i18nc("weather condition", "freezing drizzle")},
              {57, i18nc("weather condition", "heavy freezing drizzle")},
              {6, i18nc("weather condition", "rain")},
              {60, i18nc("weather condition", "light rain")},
              {61, i18nc("weather condition", "light rain")},
              {63, i18nc("weather condition", "moderate rain")},
              {65, i18nc("weather condition", "heavy rain")},
              {66, i18nc("weather condition", "light freezing rain")},
              {67, i18nc("weather condition", "freezing rain")},
              {68, i18nc("weather condition", "light rain snow")},
              {69, i18nc("weather condition", "heavy rain snow")},
              {7, i18nc("weather condition", "snow")},
              {70, i18nc("weather condition", "light snow")},
              {71, i18nc("weather condition", "light snow")},
              {73, i18nc("weather condition", "moderate snow")},
              {75, i18nc("weather condition", "heavy snow")},
              {8, i18nc("weather condition", "showers")},
              {80, i18nc("weather condition", "light showers")},
              {81, i18nc("weather condition", "showers")},
              {82, i18nc("weather condition", "heavy showers")},
              {83, i18nc("weather condition", "light snow rain showers")},
              {84, i18nc("weather condition", "heavy snow rain showers")},
              {85, i18nc("weather condition", "light snow showers")},
              {86, i18nc("weather condition", "snow showers")},
              {9, i18nc("weather condition", "thunderstorm")},
              {90, i18nc("weather condition", "thunderstorm")},
              {95, i18nc("weather condition", "light thunderstorm")},
              {96, i18nc("weather condition", "heavy thunderstorm")},
              {999, i18nc("weather condition", "n/a")},
          }
          , weatherCodeToStrMapDay{
        {0, i18nc("weather condition", "sunny")},
    }
          , weatherCodeToStrMapNight{
        {0, i18nc("weather condition", "clear sky")},
    }
    {
        weatherCodeToStrMapDay.insert(weatherCodeToStrMapCommon);
        weatherCodeToStrMapNight.insert(weatherCodeToStrMapCommon);
        weatherCodeToIconMapDay.insert(weatherCodeToIconMapCommon);
        weatherCodeToIconMapNight.insert(weatherCodeToIconMapCommon);
    }

    QString weatherCodeToStr(int code, bool isDay) const
    {
        if (isDay) {
            return weatherCodeToStrMapDay.contains(code) ? weatherCodeToStrMapDay[code] : i18nc("weather condition", "n/a");
        } else {
            return weatherCodeToStrMapNight.contains(code) ? weatherCodeToStrMapNight[code] : i18nc("weather condition", "n/a");
        }
    }

    ConditionIcons weatherCodeToIconCode(int code, bool isDay) const
    {
        if (isDay) {
            return weatherCodeToIconMapDay.contains(code) ? weatherCodeToIconMapDay[code] : NotAvailable;
        } else {
            return weatherCodeToIconMapNight.contains(code) ? weatherCodeToIconMapNight[code] : NotAvailable;
        }
    }
};

OpenMeteoIon::OpenMeteoIon(QObject *parent)
    : Ion(parent)
    , m_d(std::make_unique<Private>())
{
    qCDebug(IONENGINE_OPEN_METEO) << "loaded OpenMeteo";
}

OpenMeteoIon::~OpenMeteoIon() = default;

void OpenMeteoIon::findPlaces(std::shared_ptr<QPromise<std::shared_ptr<Locations>>> promise, const QString &query)
{
    auto systemLocale = QLocale::system();
    auto languages = systemLocale.uiLanguages();
    // open-meteo does not support fallback to other languages,
    // and it does not return in the response whether names
    // in the requested language are available.
    // So our only choice is to use one language in the query.
    auto fullLanguageCode = languages.isEmpty() ? u""_s : languages[0];
    // Also, it does not work well with full language codes,
    // so we truncate it so that it only contains the language part.
    // If the situation changes in the future, this part of the
    // code should be changed accordingly.
    auto language = fullLanguageCode.split(u'-')[0];

    promise->start();
    auto job = KIO::storedGet(searchPlacesUrl(query, language), KIO::Reload, KIO::HideProgressInfo);
    connect(
        job,
        &KJob::result,
        this,
        [job, promise, query]() {
            auto res = validateJob(job);
            auto locations = std::make_shared<Locations>();
            if (!res.has_value()) {
                resolvePromise(*promise, locations);
                return;
            }
            qCDebug(IONENGINE_OPEN_METEO) << "validated" << res.value();
            auto places = res.value()[u"results"_s].toArray();

            for (const auto &place : places) {
                auto o = place.toObject();
                auto latitude = o[u"latitude"_s].toDouble();
                auto longitude = o[u"longitude"_s].toDouble();
                auto name = formatPlaceName(o);
                Location location;
                location.setDisplayName(name);
                location.setStation(name);
                location.setCoordinates(QPointF(latitude, longitude));
                location.setPlaceInfo(u"%1|%2|%3"_s.arg(name).arg(latitude).arg(longitude));
                locations->addLocation(location);
            }
            resolvePromise(*promise, locations);
        },
        Qt::SingleShotConnection);
}

void OpenMeteoIon::fetchForecast(std::shared_ptr<QPromise<std::shared_ptr<Forecast>>> promise, const QString &placeInfo)
{
    promise->start();
    const auto coords = placeInfo.split(QChar(u'|'));
    if (coords.size() != 3) {
        promise->finish();
        return;
    }
    auto url = getWeatherUrl(coords[1], coords[2]);
    if (url.isEmpty()) {
        promise->finish();
        return;
    }
    auto job = KIO::storedGet(url, KIO::Reload, KIO::HideProgressInfo);
    qCDebug(IONENGINE_OPEN_METEO) << "get weather for" << url;
    connect(
        job,
        &KJob::result,
        this,
        [job, promise, coords, this]() {
            auto locale = QLocale::system();
            auto res = validateJob(job);
            if (!res.has_value()) {
                promise->finish();
                return;
            }

            auto data = std::make_shared<Forecast>();
            qCDebug(IONENGINE_OPEN_METEO) << "validated" << res.value();
            auto j = std::move(res).value();
            auto current = j[u"current"_s].toObject();
            auto units = j[u"current_units"_s].toObject();
            auto time = QDateTime::fromString(current[u"time"_s].toString(), Qt::ISODate);
            auto timezone = QTimeZone(j[u"timezone"_s].toString().toLocal8Bit());
            time.setTimeZone(timezone);
            auto curWeatherCode = current[u"weather_code"_s].toInt();
            auto currentIsDay = !!current[u"is_day"_s].toInt();
            Station station;
            station.setPlace(coords[0]);
            station.setCoordinates(coords[1].toDouble(), coords[2].toDouble());
            data->setStation(station);
            MetaData metaData;
            metaData.setTemperatureUnit(KUnitConversion::Celsius);
            metaData.setWindSpeedUnit(KUnitConversion::KilometerPerHour);
            metaData.setPressureUnit(KUnitConversion::Hectopascal);
            metaData.setHumidityUnit(KUnitConversion::Percent);
            metaData.setCredit(i18nc("weather credit", "Weather data by open-meteo"));
            metaData.setCreditURL(CREDIT_URL);
            data->setMetadata(metaData);
            LastObservation lastObservation;
            lastObservation.setCurrentConditions(m_d->weatherCodeToStr(curWeatherCode, currentIsDay));
            lastObservation.setConditionIcon(getWeatherIcon(m_d->weatherCodeToIconCode(curWeatherCode, currentIsDay)));
            lastObservation.setTemperature(current[u"temperature_2m"_s].toDouble());
            lastObservation.setObservationTimestamp(time);
            lastObservation.setWindchill(current[u"apparent_temperature"_s].toDouble());
            lastObservation.setWindSpeed(current[u"wind_speed_10m"_s].toDouble());
            lastObservation.setWindGust(current[u"wind_gusts_10m"_s].toDouble());
            lastObservation.setWindDirection(windDirectionToStr(current[u"wind_direction"_s].toDouble()));
            lastObservation.setPressure(current[u"surface_pressure"_s].toDouble());
            lastObservation.setHumidity(current[u"relative_humidity_2m"_s].toDouble());
            data->setLastObservation(lastObservation);

            auto futureDays = std::make_shared<FutureDays>();

            auto forecast = j[u"daily"_s].toObject();
            auto numDays = forecast[u"time"_s].toArray().size();
            for (int d = 0; d < numDays; ++d) {
                FutureDayForecast futureDayForecast;
                FutureForecast futureForecast;
                auto forecastDate = QDate::fromString(forecast[u"time"_s].toArray()[d].toString(), Qt::ISODate);
                futureDayForecast.setMonthDay(forecastDate.day());
                futureDayForecast.setWeekDay(locale.toString(forecastDate, u"ddd"_s));
                const auto isDay = true;
                auto weatherCode = forecast[u"weather_code"_s].toArray()[d].toInt();
                futureForecast.setConditionIcon(getWeatherIcon(m_d->weatherCodeToIconCode(weatherCode, isDay)));
                futureForecast.setCondition(m_d->weatherCodeToStr(weatherCode, isDay));
                futureForecast.setHighTemp(forecast[u"temperature_2m_max"_s].toArray()[d].toDouble());
                futureForecast.setLowTemp(forecast[u"temperature_2m_min"_s].toArray()[d].toDouble());
                futureForecast.setConditionProbability(forecast[u"precipitation_probability_max"_s].toArray()[d].toDouble());
                futureDayForecast.setDaytime(futureForecast);
                futureDays->addDay(futureDayForecast);
            }
            data->setFutureDays(futureDays);

            resolvePromise(*promise, data);
        },
        Qt::SingleShotConnection);
}

K_PLUGIN_CLASS_WITH_JSON(OpenMeteoIon, "metadata.json")

#include "ion_open_meteo.moc"
