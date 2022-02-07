/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alternatecalendarplugin.h"

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
    }

    if (alternateDatesData.size() > 0) {
        Q_EMIT alternateDateReady(alternateDatesData);
    }
    Q_EMIT subLabelReady(subLabelsData);
}

}
