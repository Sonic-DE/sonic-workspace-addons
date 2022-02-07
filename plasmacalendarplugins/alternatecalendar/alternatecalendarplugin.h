/*
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALTERNATECALENDARPLUGIN_H
#define ALTERNATECALENDARPLUGIN_H

#include <memory>

#include <KConfigWatcher>

#include <CalendarEvents/CalendarEventsPlugin>

#include "calendarsystem.h"

class KConfigGroup;

namespace AlternateCalendarPlugin
{

class AlternateCalendarPlugin;
class AbstractCalendarProvider;

class AlternateCalendarPluginPrivate
{
public:
    explicit AlternateCalendarPluginPrivate(AlternateCalendarPlugin *parent);
    ~AlternateCalendarPluginPrivate();

    void init();
    AbstractCalendarProvider *calendarProvider() const;

    CalendarSystem::System m_calendarSystem;
    int m_dateOffset; // For the (tabular) Islamic Civil calendar

private:
    std::unique_ptr<AbstractCalendarProvider> m_calendarProvider;

    // For updating config
    KConfigGroup m_generalConfigGroup;
    KConfigWatcher::Ptr m_configWatcher;

    AlternateCalendarPlugin *p;
};

class AlternateCalendarPlugin : public CalendarEvents::CalendarEventsPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.CalendarEventsPlugin" FILE "alternatecalendarplugin.json")
    Q_INTERFACES(CalendarEvents::CalendarEventsPlugin)

public:
    AlternateCalendarPlugin();
    ~AlternateCalendarPlugin() override;

    void loadEventsForDateRange(const QDate &startDate, const QDate &endDate) override;

public Q_SLOTS:
    void updateSettings();

private:
    std::unique_ptr<AlternateCalendarPluginPrivate> d;
    QDate m_lastStartDate;
    QDate m_lastEndDate;
};

}

#endif
