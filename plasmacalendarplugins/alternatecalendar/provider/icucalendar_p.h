/*
    SPDX-FileCopyrightText: 2021 Gary Wang <wzc782970009@gmail.com>
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <unicode/calendar.h>
#include <unicode/smpdtfmt.h>

class QDate;
class QString;

namespace AlternateCalendarPlugin
{

class ICUCalendarPrivate
{
public:
    void setDate(const QDate &date);

    int32_t year() const;
    int32_t month() const;
    int32_t day() const;
    QDate date() const;

protected:
    std::unique_ptr<icu::Calendar> m_calendar;
};

}