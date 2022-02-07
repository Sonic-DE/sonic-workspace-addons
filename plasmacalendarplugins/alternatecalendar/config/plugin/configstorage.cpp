/*
    SPDX-FileCopyrightText: 2018 Friedrich W. H. Kossebau <kossebau@kde.org>
    SPDX-FileCopyrightText: 2022 Fushan Wen <qydwhotmail@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configstorage.h"

#include <QMetaEnum>

#include <KLocalizedString>
#include <KSharedConfig>

namespace AlternateCalendarPlugin
{

CalendarSystemModel::CalendarSystemModel(QObject *parent)
    : QAbstractListModel(parent)
{
    const std::map<CalendarSystem::System, QString> textMap{
        {CalendarSystem::Julian, i18ndc("plasma_calendar_alternatecalendar", "@item:inlist", "Julian")},
        {CalendarSystem::Milankovic, i18ndc("plasma_calendar_alternatecalendar", "@item:inlist", "Milankovic")},
        {CalendarSystem::Jalali, i18ndc("plasma_calendar_alternatecalendar", "@item:inlist", "The Solar Hijri calendar (Persian)")},
        {CalendarSystem::IslamicCivil, i18ndc("plasma_calendar_alternatecalendar", "@item:inlist", "The Islamic Civil calendar")},
        {CalendarSystem::Chinese, i18ndc("plasma_calendar_alternatecalendar", "@item:inlist", "Chinese Lunar Calendar")},
        {CalendarSystem::Indian, i18ndc("plasma_calendar_alternatecalendar", "@item:inlist", "Indian National Calendar")},
    };
    const QMetaEnum e = QMetaEnum::fromType<CalendarSystem::System>();

    beginInsertRows(QModelIndex(), 0, e.keyCount() - 2 /* Gregorian */);
    m_items.reserve(e.keyCount() - 2);
    for (int k = 0; k < e.keyCount(); k++) {
        CalendarSystemItem item;
        item.value = static_cast<CalendarSystem::System>(e.value(k));
        if (textMap.count(item.value) > 0) {
            item.text = textMap.at(item.value);
            m_items.emplace_back(item);
        }
    }
    endInsertRows();
}

CalendarSystemModel::~CalendarSystemModel() = default;

QVariant CalendarSystemModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();
    if (row < rowCount()) {
        switch (role) {
        case Qt::DisplayRole:
            return m_items.at(row).text;
        case ValueRole:
            return m_items.at(row).value;
        default:
            break;
        }
    }

    return QVariant();
}

int CalendarSystemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_items.size();
}

QHash<int, QByteArray> CalendarSystemModel::roleNames() const
{
    return {
        {Qt::DisplayRole, QByteArrayLiteral("display")},
        {ValueRole, QByteArrayLiteral("value")},
    };
}

int CalendarSystemModel::indexOfCalendarSystem(CalendarSystem::System value) const
{
    const auto it = std::find_if(m_items.cbegin(), m_items.cend(), [value](const CalendarSystemItem &item) {
        return item.value == value;
    });

    if (it != m_items.cend()) {
        return it - m_items.cbegin();
    }

    return 0;
}

ConfigStorage::ConfigStorage(QObject *parent)
    : QObject(parent)
    , m_calendarSystemModel(new CalendarSystemModel())
{
    auto config = KSharedConfig::openConfig(QStringLiteral("plasma_calendar_alternatecalendar"));
    m_generalConfigGroup = config->group("General");

    m_calendarSystem = static_cast<CalendarSystem::System>(m_generalConfigGroup.readEntry("calendarSystem", static_cast<int>(CalendarSystem::Julian)));
    m_dateOffset = m_generalConfigGroup.readEntry("dateOffset", 0);
}

QAbstractListModel *ConfigStorage::calendarSystemModel() const
{
    return m_calendarSystemModel.get();
}

int ConfigStorage::currentIndex() const
{
    return m_calendarSystemModel->indexOfCalendarSystem(m_calendarSystem);
}

void ConfigStorage::save()
{
    m_generalConfigGroup.writeEntry("calendarSystem", static_cast<int>(m_calendarSystem), KConfigBase::Notify);
    m_generalConfigGroup.writeEntry("dateOffset", m_dateOffset, KConfigBase::Notify);

    m_generalConfigGroup.sync();
}

}
