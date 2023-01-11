/*
    SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "islamiccalendar.h"

#include "icucalendar_p.h"

class IslamicCalendarProviderPrivate : public ICUCalendarPrivate
{
public:
    explicit IslamicCalendarProviderPrivate();

    /**
     * For formatting, see the documentation of SimpleDateFormat:
     * https://unicode-org.github.io/icu-docs/apidoc/released/icu4c/classicu_1_1SimpleDateFormat.html#details
     */
    QString formattedDateString(const icu::UnicodeString &str) const;
    QString formattedDateStringInNativeLanguage(const icu::UnicodeString &str) const;

    QCalendar::YearMonthDay fromGregorian(const QDate &_date);
    CalendarEvents::CalendarEventsPlugin::SubLabel subLabels(const QDate &date);

private:
    // See https://unicode-org.github.io/icu/userguide/locale/#keywords for available keywords
    icu::Locale m_arabicLocale;
    icu::Locale m_nativeLocale;
};

IslamicCalendarProviderPrivate::IslamicCalendarProviderPrivate()
    : ICUCalendarPrivate()
    // See https://github.com/unicode-org/cldr/blob/main/common/bcp47/number.xml for available number systems
    , m_arabicLocale(icu::Locale("ar_SA", 0, 0, "calendar=islamic;numbers=arab"))
    , m_nativeLocale(icu::Locale(QLocale::system().name().toLatin1(), 0, 0, "calendar=islamic;"))
{
    if (U_FAILURE(m_errorCode)) {
        return; // Failed to create m_GregorianCalendar
    }

    m_calendar.reset(icu::Calendar::createInstance(m_arabicLocale, m_errorCode));
}

QString IslamicCalendarProviderPrivate::formattedDateString(const icu::UnicodeString &str) const
{
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::UnicodeString dateString;
    icu::SimpleDateFormat formatter(str, m_arabicLocale, errorCode);
    formatter.setCalendar(*m_calendar);
    formatter.format(m_calendar->getTime(errorCode), dateString);

    std::string utf8Str;
    dateString.toUTF8String<std::string>(utf8Str);

    return QString::fromStdString(utf8Str);
}

QString IslamicCalendarProviderPrivate::formattedDateStringInNativeLanguage(const icu::UnicodeString &str) const
{
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::UnicodeString dateString;
    icu::SimpleDateFormat formatter(str, m_nativeLocale, errorCode);
    formatter.setCalendar(*m_calendar);
    formatter.format(m_calendar->getTime(errorCode), dateString);

    std::string utf8Str;
    dateString.toUTF8String<std::string>(utf8Str);

    return QString::fromStdString(utf8Str);
}

QCalendar::YearMonthDay IslamicCalendarProviderPrivate::fromGregorian(const QDate &_date)
{
    if (U_FAILURE(m_errorCode) || !_date.isValid() || !setDate(_date)) {
        return {};
    }

    return date();
}

CalendarEvents::CalendarEventsPlugin::SubLabel IslamicCalendarProviderPrivate::subLabels(const QDate &date)
{
    auto sublabel = CalendarEvents::CalendarEventsPlugin::SubLabel{};

    if (U_FAILURE(m_errorCode) || !date.isValid() || !setDate(date)) {
        return sublabel;
    }

    const bool isLocaleArabic = QLocale::system().language() == QLocale::Arabic;

    sublabel.dayLabel = isLocaleArabic ? formattedDateString("d") : QString::number(day());
    const QString arabicDateString = formattedDateString(QLocale(QLocale::Arabic).dateFormat().toUtf8().constData());
    // Translated month names are available in https://github.com/unicode-org/icu/tree/main/icu4c/source/data/locales
    sublabel.label = isLocaleArabic ? arabicDateString
                                    : i18ndc("plasma_calendar_alternatecalendar",
                                             "@label %1 Islamic calendar date in system language %2 Islamic calendar date in Arabic",
                                             "%1 (%2)",
                                             formattedDateStringInNativeLanguage(QLocale::system().dateFormat().toUtf8().constData()),
                                             arabicDateString);
    sublabel.priority = CalendarEvents::CalendarEventsPlugin::SubLabelPriority::Low;

    return sublabel;
}

IslamicCalendarProvider::IslamicCalendarProvider(QObject *parent, CalendarSystem::System calendarSystem)
    : AbstractCalendarProvider(parent, calendarSystem)
    , d(std::make_unique<IslamicCalendarProviderPrivate>())
{
    Q_ASSERT(m_calendarSystem == CalendarSystem::Islamic);
}

IslamicCalendarProvider::~IslamicCalendarProvider()
{
}

QCalendar::YearMonthDay IslamicCalendarProvider::fromGregorian(const QDate &date) const
{
    return d->fromGregorian(date);
}

CalendarEvents::CalendarEventsPlugin::SubLabel IslamicCalendarProvider::subLabels(const QDate &date) const
{
    return d->subLabels(date);
}
