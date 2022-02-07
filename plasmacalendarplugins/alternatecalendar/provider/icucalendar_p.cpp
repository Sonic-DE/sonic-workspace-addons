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

ICUCalendarPrivate::ICUCalendarPrivate()
{
    UErrorCode errorCode = U_ZERO_ERROR;
    m_GregorianCalendar.reset(icu::Calendar::createInstance("en_US@calendar=gregorian", errorCode));
    Q_ASSERT(!U_FAILURE(errorCode));
}

ICUCalendarPrivate::~ICUCalendarPrivate()
{
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

bool ICUCalendarPrivate::setDate(const QDate &date)
{
    // icu: Month value is 0-based. e.g., 0 for January.
    m_GregorianCalendar->set(date.year(), date.month() - 1, date.day());

    UErrorCode errorCode = U_ZERO_ERROR;
    UDate time = m_GregorianCalendar->getTime(errorCode);

    if (U_FAILURE(errorCode)) {
        return false;
    }

    m_calendar->setTime(time, errorCode);

    return !U_FAILURE(errorCode);
}

bool ICUCalendarPrivate::setTime(double time)
{
    UErrorCode errorCode = U_ZERO_ERROR;
    m_calendar->setTime(time, errorCode);

    return !U_FAILURE(errorCode);
}
}