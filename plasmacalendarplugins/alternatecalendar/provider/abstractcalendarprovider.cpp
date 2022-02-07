/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "abstractcalendarprovider.h"

namespace AlternateCalendarPlugin
{

AbstractCalendarProvider::AbstractCalendarProvider(CalendarSystem::System calendarSystem)
    : m_calendarSystem(calendarSystem)
{
}

QDate AbstractCalendarProvider::fromGregorian(const QDate &date) const
{
    if (!date.isValid()) {
        return QDate();
    }

    return date;
}

CalendarEvents::CalendarEventsPlugin::SubLabel AbstractCalendarProvider::subLabels([[maybe_unused]] const QDate &date) const
{
    return CalendarEvents::CalendarEventsPlugin::SubLabel{QString(), QString(), QString(), CalendarEvents::CalendarEventsPlugin::SubLabelPriority::Low};
}

}
