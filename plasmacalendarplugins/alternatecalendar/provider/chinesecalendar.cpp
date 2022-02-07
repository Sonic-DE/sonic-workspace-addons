/*
    SPDX-FileCopyrightText: 2021 Gary Wang <wzc782970009@gmail.com>
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "chinesecalendar.h"

namespace AlternateCalendarPlugin
{

ChineseCalendarProviderPrivate::ChineseCalendarProviderPrivate()
    : ICUCalendarPrivate()
{
    UErrorCode errorCode = U_ZERO_ERROR;
    m_calendar.reset(icu::Calendar::createInstance("en_US@calendar=chinese", errorCode));
    Q_ASSERT(!U_FAILURE(errorCode));
}

ChineseCalendarProviderPrivate::~ChineseCalendarProviderPrivate()
{
}

QString ChineseCalendarProviderPrivate::formattedDateString(const icu::UnicodeString &str, bool hanidays) const
{
    icu::Locale locale("zh", 0, 0, hanidays ? "calendar=chinese;numbers=hanidays" : "calendar=chinese");
    UErrorCode errorCode = U_ZERO_ERROR;
    icu::UnicodeString dateString;
    icu::SimpleDateFormat formatter(str, locale, errorCode);
    formatter.setCalendar(*m_calendar);
    formatter.format(m_calendar->getTime(errorCode), dateString);

    std::string utf8Str;
    dateString.toUTF8String<std::string>(utf8Str);

    return QString::fromUtf8(utf8Str.c_str());
}

QString ChineseCalendarProviderPrivate::yearDisplayName() const
{
    return formattedDateString("U");
}

QString ChineseCalendarProviderPrivate::monthDisplayName() const
{
    return formattedDateString("MMM");
}

QString ChineseCalendarProviderPrivate::dayDisplayName() const
{
    return formattedDateString("d", true);
}

ChineseCalendarProvider::ChineseCalendarProvider(CalendarSystem::System calendarSystem)
    : AbstractCalendarProvider(calendarSystem)
    , d(new ChineseCalendarProviderPrivate())
{
    Q_ASSERT(m_calendarSystem == CalendarSystem::Chinese);
}

ChineseCalendarProvider::~ChineseCalendarProvider()
{
}

CalendarEvents::CalendarEventsPlugin::SubLabel ChineseCalendarProvider::subLabels(const QDate &date) const
{
    if (!date.isValid() || !d->setDate(date)) {
        return CalendarEvents::CalendarEventsPlugin::SubLabel{};
    }

    return CalendarEvents::CalendarEventsPlugin::SubLabel{
        d->yearDisplayName(),
        d->monthDisplayName(),
        d->day() == 1 ? d->monthDisplayName() : d->dayDisplayName(),
        CalendarEvents::CalendarEventsPlugin::SubLabelPriority::Low,
    };
}

}
