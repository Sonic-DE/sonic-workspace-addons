/*
    SPDX-FileCopyrightText: 2023 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "hebrewcalendar.h"

#include <array>

#include "icucalendar_p.h"

class HebrewCalendarProviderPrivate : public ICUCalendarPrivate
{
public:
    explicit HebrewCalendarProviderPrivate();

    QCalendar::YearMonthDay fromGregorian(const QDate &_date);
    CalendarEvents::CalendarEventsPlugin::SubLabel subLabels(const QDate &date);
};

HebrewCalendarProviderPrivate::HebrewCalendarProviderPrivate()
    : ICUCalendarPrivate()
{
    if (U_FAILURE(m_errorCode)) {
        return; // Failed to create m_GregorianCalendar
    }

    m_calendar.reset(icu::Calendar::createInstance(icu::Locale("he_IL@calendar=hebrew"), m_errorCode));
}

QCalendar::YearMonthDay HebrewCalendarProviderPrivate::fromGregorian(const QDate &_date)
{
    if (U_FAILURE(m_errorCode) || !_date.isValid() || !setDate(_date)) {
        return {};
    }

    return date();
}

CalendarEvents::CalendarEventsPlugin::SubLabel HebrewCalendarProviderPrivate::subLabels(const QDate &date)
{
    auto sublabel = CalendarEvents::CalendarEventsPlugin::SubLabel{};

    if (U_FAILURE(m_errorCode) || !date.isValid() || !setDate(date)) {
        return sublabel;
    }

    static const std::array<QString, 13> monthNames{
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Nisan"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Iyyar"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Sivan"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Tammuz"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Ab"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Elul"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Tishri"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Heshvan"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Kislev"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Tebeth"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Shebat"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Adar I"),
        i18ndc("plasma_calendar_alternatecalendar", "Month name in Hebrew/Jewish Calendar", "Adar II"),
    };

    sublabel.dayLabel = QString::number(day());
    sublabel.label = i18ndc("plasma_calendar_alternatecalendar",
                            "@label %1 day %2 month name in Hebrew/Jewish Calendar %3 year",
                            "%1 %2, %3",
                            sublabel.dayLabel,
                            monthNames[month() - 1],
                            QString::number(year()));
    sublabel.priority = CalendarEvents::CalendarEventsPlugin::SubLabelPriority::Low;

    return sublabel;
}

HebrewCalendarProvider::HebrewCalendarProvider(QObject *parent, CalendarSystem::System calendarSystem)
    : AbstractCalendarProvider(parent, calendarSystem)
    , d(std::make_unique<HebrewCalendarProviderPrivate>())
{
    Q_ASSERT(m_calendarSystem == CalendarSystem::Hebrew);
}

HebrewCalendarProvider::~HebrewCalendarProvider()
{
}

QCalendar::YearMonthDay HebrewCalendarProvider::fromGregorian(const QDate &date) const
{
    return d->fromGregorian(date);
}

CalendarEvents::CalendarEventsPlugin::SubLabel HebrewCalendarProvider::subLabels(const QDate &date) const
{
    return d->subLabels(date);
}
