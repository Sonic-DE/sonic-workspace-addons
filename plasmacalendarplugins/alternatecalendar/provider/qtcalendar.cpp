/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "qtcalendar.h"

namespace AlternateCalendarPlugin
{

QtCalendarProvider::QtCalendarProvider(CalendarSystem::System calendarSystem)
    : AbstractCalendarProvider(calendarSystem)
{
    Q_ASSERT(m_calendarSystem <= static_cast<CalendarSystem::System>(QCalendar::System::Last));
    m_calendar = QCalendar(static_cast<QCalendar::System>(m_calendarSystem));
}

QtCalendarProvider::~QtCalendarProvider()
{
}

QDate QtCalendarProvider::fromGregorian(const QDate &date) const
{
    if (!date.isValid()) {
        return QDate();
    }

    const QCalendar::YearMonthDay ymd = m_calendar.partsFromDate(date);
    return QDate(ymd.year, ymd.month, ymd.day);
}

CalendarEvents::CalendarEventsPlugin::SubLabel QtCalendarProvider::subLabels(const QDate &date) const
{
    if (!date.isValid()) {
        return CalendarEvents::CalendarEventsPlugin::SubLabel{};
    }

    const QDate altDate = fromGregorian(date);

    return CalendarEvents::CalendarEventsPlugin::SubLabel{
        QString::number(altDate.year()),
        QString::number(altDate.month()),
        QString::number(altDate.day()),
        CalendarEvents::CalendarEventsPlugin::SubLabelPriority::Low,
    };
}

}