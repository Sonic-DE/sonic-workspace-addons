/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alternatecalendarplugin.h"

#include <KConfigGroup>
#include <KSharedConfig>

#include "provider/qtcalendar.h"

namespace AlternateCalendarPlugin
{

AlternateCalendarPluginPrivate::AlternateCalendarPluginPrivate(AlternateCalendarPlugin *parent)
    : p(parent)
{
}

void AlternateCalendarPluginPrivate::init()
{
    // Load/Reload the calendar provider
    switch (m_calendarSystem) {
    default:
        m_calendarProvider.reset(new AbstractCalendarProvider(m_calendarSystem));
    }
}

AbstractCalendarProvider *AlternateCalendarPluginPrivate::calendarProvider() const
{
    return m_calendarProvider.get();
}

CalendarSystem::System AlternateCalendarPluginPrivate::calendarSystem() const
{
    return m_calendarSystem;
}

AlternateCalendarPlugin::AlternateCalendarPlugin()
    : CalendarEvents::CalendarEventsPlugin()
    , d(new AlternateCalendarPluginPrivate(this))
{
}

void AlternateCalendarPlugin::loadEventsForDateRange(const QDate &startDate, const QDate &endDate)
{
    QHash<QDate, QDate> alternateDatesData;
    QHash<QDate, SubLabel> subLabelsData;

    if (!endDate.isValid() || d->calendarSystem() == CalendarSystem::Gregorian) {
        return;
    }

    for (QDate date = startDate; date <= endDate && date.isValid(); date = date.addDays(1)) {
        if (const QDate alt = d->calendarProvider()->fromGregorian(date); alt != date) {
            alternateDatesData.insert(date, alt);
        }
        subLabelsData.insert(date, d->calendarProvider()->subLabels(date));
    }

    if (alternateDatesData.size() > 0) {
        Q_EMIT alternateDateReady(alternateDatesData);
    }
    Q_EMIT subLabelReady(subLabelsData);
}

}
