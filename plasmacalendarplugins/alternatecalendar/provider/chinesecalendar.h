/*
    SPDX-FileCopyrightText: 2021 Gary Wang <wzc782970009@gmail.com>
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractcalendarprovider.h"
#include "icucalendar_p.h"

namespace AlternateCalendarPlugin
{

class ChineseCalendarProviderPrivate;

/**
 * @short An alternate calendar provider for Chinese calendar system.
 *
 * This class presents an alternate calendar provider for Chinese calendar system.
 */
class ChineseCalendarProvider : public AbstractCalendarProvider
{
    Q_OBJECT

public:
    explicit ChineseCalendarProvider(CalendarSystem::System calendarSystem);
    ~ChineseCalendarProvider() override;

    CalendarEvents::CalendarEventsPlugin::SubLabel subLabels(const QDate &date) const override;

private:
    std::unique_ptr<ChineseCalendarProviderPrivate> d;
};

}
