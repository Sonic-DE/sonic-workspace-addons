/*
    SPDX-FileCopyrightText: 2021 Gary Wang <wzc782970009@gmail.com>
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "icucalendar_p.h"

#include <QDate>
#include <QString>

namespace AlternateCalendarPlugin
{

void ICUCalendarPrivate::setDate(const QDate &date)
{
    // icu: Month value is 0-based. e.g., 0 for January.
    m_calendar->set(date.year(), date.month() - 1, date.day());
}

int32_t ICUCalendarPrivate::year() const
{
    UErrorCode errorCode = U_ZERO_ERROR;
    int32_t year = m_calendar->get(UCAL_YEAR, errorCode);
    Q_ASSERT(!U_FAILURE(errorCode));

    return year;
}

int32_t ICUCalendarPrivate::month() const
{
    UErrorCode errorCode = U_ZERO_ERROR;
    int32_t month = m_calendar->get(UCAL_MONTH, errorCode);
    Q_ASSERT(!U_FAILURE(errorCode));

    return month + 1;
}

int32_t ICUCalendarPrivate::day() const
{
    UErrorCode errorCode = U_ZERO_ERROR;
    int32_t day = m_calendar->get(UCAL_DATE, errorCode);
    Q_ASSERT(!U_FAILURE(errorCode));

    return day;
}

QDate ICUCalendarPrivate::date() const
{
    return QDate(year(), month(), day());
}

}