/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alternatecalendarplugin.h"

#include "provider/qtcalendar.h"

namespace AlternateCalendarPlugin
{

AlternateCalendarPluginPrivate::AlternateCalendarPluginPrivate(AlternateCalendarPlugin *parent)
    : p(parent)
{
}

AlternateCalendarPluginPrivate::~AlternateCalendarPluginPrivate()
{
}

void AlternateCalendarPluginPrivate::init()
{
    // Load/Reload the calendar provider
    switch (m_calendarSystem) {
#ifndef QT_BOOTSTRAPPED
    case CalendarSystem::Julian:
    case CalendarSystem::Milankovic:
#endif
#if QT_CONFIG(jalalicalendar)
    case CalendarSystem::Jalali:
#endif
#if QT_CONFIG(islamiccivilcalendar)
    case CalendarSystem::IslamicCivil:
#endif
        m_calendarProvider.reset(new QtCalendarProvider(m_calendarSystem));
        break;
    default:
        m_calendarProvider.reset(new AbstractCalendarProvider(m_calendarSystem));
    }
}

AbstractCalendarProvider *AlternateCalendarPluginPrivate::calendarProvider() const
{
    return m_calendarProvider.get();
}

AlternateCalendarPlugin::AlternateCalendarPlugin()
    : CalendarEvents::CalendarEventsPlugin()
    , d(new AlternateCalendarPluginPrivate(this))
{
}

AlternateCalendarPlugin::~AlternateCalendarPlugin()
{
}

void AlternateCalendarPlugin::loadEventsForDateRange(const QDate &startDate, const QDate &endDate)
{
    QHash<QDate, QDate> alternateDatesData;
    QHash<QDate, SubLabel> subLabelsData;

    if (!endDate.isValid() || d->m_calendarSystem == CalendarSystem::Gregorian) {
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
