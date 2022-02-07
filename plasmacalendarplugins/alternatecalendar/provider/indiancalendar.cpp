/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "indiancalendar.h"

namespace AlternateCalendarPlugin
{

class IndianCalendarProviderPrivate : public ICUCalendarPrivate
{
public:
    explicit IndianCalendarProviderPrivate();
    ~IndianCalendarProviderPrivate() override;

    QDate fromGregorian(const QDate &_date);
    CalendarEvents::CalendarEventsPlugin::SubLabel subLabels(const QDate &date);
};

IndianCalendarProviderPrivate::IndianCalendarProviderPrivate()
    : ICUCalendarPrivate()
{
    if (U_FAILURE(m_errorCode)) {
        return; // Failed to create m_GregorianCalendar
    }

    m_calendar.reset(icu::Calendar::createInstance("en_US@calendar=indian", m_errorCode));
}

IndianCalendarProviderPrivate::~IndianCalendarProviderPrivate()
{
}

QDate IndianCalendarProviderPrivate::fromGregorian(const QDate &_date)
{
    if (U_FAILURE(m_errorCode) || !_date.isValid() || !setDate(_date)) {
        return QDate();
    }

    return date();
}

CalendarEvents::CalendarEventsPlugin::SubLabel IndianCalendarProviderPrivate::subLabels(const QDate &date)
{
    if (U_FAILURE(m_errorCode) || !date.isValid() || !setDate(date)) {
        return CalendarEvents::CalendarEventsPlugin::SubLabel{};
    }

    return CalendarEvents::CalendarEventsPlugin::SubLabel{
        QString::number(year()),
        QString::number(month()),
        QString::number(day()),
        CalendarEvents::CalendarEventsPlugin::SubLabelPriority::Low,
    };
}

IndianCalendarProvider::IndianCalendarProvider(CalendarSystem::System calendarSystem)
    : AbstractCalendarProvider(calendarSystem)
    , d(new IndianCalendarProviderPrivate())
{
    Q_ASSERT(m_calendarSystem == CalendarSystem::Indian);
}

IndianCalendarProvider::~IndianCalendarProvider()
{
}

QDate IndianCalendarProvider::fromGregorian(const QDate &date) const
{
    return d->fromGregorian(date);
}

CalendarEvents::CalendarEventsPlugin::SubLabel IndianCalendarProvider::subLabels(const QDate &date) const
{
    return d->subLabels(date);
}

}
