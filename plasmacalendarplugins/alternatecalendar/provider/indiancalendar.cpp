/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "indiancalendar.h"

namespace AlternateCalendarPlugin
{

IndianCalendarProviderPrivate::IndianCalendarProviderPrivate()
{
    UErrorCode errorCode = U_ZERO_ERROR;
    m_calendar.reset(icu::Calendar::createInstance("en_US@calendar=indian", errorCode));
    Q_ASSERT(!U_FAILURE(errorCode));
}

IndianCalendarProvider::IndianCalendarProvider(CalendarSystem::System calendarSystem)
    : AbstractCalendarProvider(calendarSystem)
    , d(new IndianCalendarProviderPrivate())
{
    Q_ASSERT(m_calendarSystem == CalendarSystem::Indian);
}

QDate IndianCalendarProvider::fromGregorian(const QDate &date) const
{
    if (!date.isValid()) {
        return QDate();
    }

    d->setDate(date);

    return d->date();
}

CalendarEvents::CalendarEventsPlugin::SubLabel IndianCalendarProvider::subLabels(const QDate &date) const
{
    if (!date.isValid()) {
        return CalendarEvents::CalendarEventsPlugin::SubLabel{};
    }

    d->setDate(date);

    return CalendarEvents::CalendarEventsPlugin::SubLabel{
        QString::number(d->year()),
        QString::number(d->month()),
        QString::number(d->day()),
        CalendarEvents::CalendarEventsPlugin::SubLabelPriority::Low,
    };
}

}
