# SPDX-FileCopyrightText: 2025 tusooa <tusooa@kazv.moe>
# SPDX-License-Identifier: BSD-3-Clause

set (ion_open_meteo_SRCS ion_open_meteo.cpp)
ecm_qt_declare_logging_category(ion_open_meteo_SRCS
    HEADER ion_open_meteodebug.h
    IDENTIFIER IONENGINE_OPEN_METEO
    CATEGORY_NAME org.kde.weather.ion.open-meteo
    DEFAULT_SEVERITY Info
    DESCRIPTION "Weather ion open-meteo"
    EXPORT PLASMAWORKSPACE
)

kcoreaddons_add_plugin(openmeteo
    INSTALL_NAMESPACE "plasma/weather_ions"
    SOURCES ${ion_open_meteo_SRCS}
)

target_link_libraries (openmeteo
    weatherion
    KF6::KIOCore
    KF6::UnitConversion
    KF6::I18n
)
