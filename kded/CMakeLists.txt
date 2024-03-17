# SPDX-FileCopyrightText: 2024 Natalie Clarius <natalie.clarius@kde.org>
# SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

kcoreaddons_add_plugin(kameleon
    SOURCES
        kameleon.cpp
        kameleon.h
    INSTALL_NAMESPACE
        "kf6/kded")

target_link_libraries(kameleon
                      KF6::CoreAddons
                      KF6::I18n
                      KF6::ConfigCore
                      KF6::ConfigGui
                      KF6::DBusAddons
                      KF6::WidgetsAddons)

ecm_qt_declare_logging_category(kameleon
                      HEADER kameleon_debug.h
                      IDENTIFIER KAMELEON
                      CATEGORY_NAME ork.kde.plasma.kameleon
                      DESCRIPTION "Accent color device led kded module"
                      EXPORT KDEPLASMAADDONS
                  )
