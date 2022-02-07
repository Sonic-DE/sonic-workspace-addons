/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractcalendarprovider.h"
#include "icucalendar_p.h"

namespace AlternateCalendarPlugin
{

class IndianCalendarProviderPrivate : public ICUCalendarPrivate
{
public:
    explicit IndianCalendarProviderPrivate();
};

/**
 * @short An alternate calendar provider for Indian calendar system.
 *
 * This class presents an alternate calendar provider for Indian calendar system.
 */
class IndianCalendarProvider : public AbstractCalendarProvider
{
    Q_OBJECT

public:
    explicit IndianCalendarProvider(CalendarSystem::System calendarSystem);

    QDate fromGregorian(const QDate &date) const override;
    CalendarEvents::CalendarEventsPlugin::SubLabel subLabels(const QDate &date) const override;

private:
    std::unique_ptr<IndianCalendarProviderPrivate> d;
};

}
