# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: SonicDE

# Find SonicVia for QMK VIA support (version 6.27.0 to match sonic-frameworks)
find_package(SonicVia 6.27.0)
set_package_properties(SonicVia PROPERTIES
    TYPE OPTIONAL
    PURPOSE "QMK VIA keyboard support"
    DESCRIPTION "SonicVia library for VIA protocol"
)
if(SonicVia_FOUND)
    set(HAVE_VIA_SUPPORT ON)
else()
    set(HAVE_VIA_SUPPORT OFF)
endif()

# Find PolkitQt6-1 via pkg-config (CRITICAL: the PolkitQt6-1::Core CMake target does NOT exist).
# REQUIRED: unlike SonicVia (which has a meaningful NotSupported stub), a polkit-less helper cannot
# authenticate ANY call and would return AuthFailed for everything — it is useless. The Rust helper
# likewise had an unconditional zbus_polkit dependency. polkit-qt6 is always present on Plasma systems
# (Plasma itself depends on polkit), and the whole helper is already skipped on FreeBSD.
find_package(PkgConfig REQUIRED)
pkg_check_modules(polkitqt6 REQUIRED IMPORTED_TARGET polkit-qt6-core-1)
set(HAVE_POLKIT_SUPPORT ON)

# Executable MUST be created before ecm_qt_declare_logging_category references it.
add_executable(kameleon-qmk-helper
    qmk-dbus-helper.cpp
    qmk-dbus-helper.h
)

# Logging category (target already exists above)
ecm_qt_declare_logging_category(kameleon-qmk-helper
    HEADER qmk-dbus-helper_debug.h
    IDENTIFIER QMKDBUS
    CATEGORY_NAME org.kde.kameleon.qmk.dbus.helper
    DESCRIPTION "Kameleon QMK D-Bus helper service"
)

target_link_libraries(kameleon-qmk-helper PRIVATE
    Qt6::Core
    Qt6::DBus
    Qt6::Gui
)

if(HAVE_VIA_SUPPORT)
    target_link_libraries(kameleon-qmk-helper PRIVATE SonicVia::SonicVia)
    target_compile_definitions(kameleon-qmk-helper PRIVATE HAVE_VIA_SUPPORT=1)
endif()

# polkit-qt6 is unconditionally required (see REQUIRED pkg_check above) — always link it.
target_link_libraries(kameleon-qmk-helper PRIVATE PkgConfig::polkitqt6)
target_compile_definitions(kameleon-qmk-helper PRIVATE HAVE_POLKIT_SUPPORT=1)

install(TARGETS kameleon-qmk-helper DESTINATION ${KDE_INSTALL_LIBEXECDIR_KF})
install(FILES org.kde.kameleon.qmk.helper.conf
        DESTINATION ${KDE_INSTALL_DBUSDIR}/system.d)
install(FILES org.kde.kameleon.qmk.helper.policy
        DESTINATION ${KDE_INSTALL_DATADIR}/polkit-1/actions)

# Output filename MUST use dots (matches the service Name), not hyphens.
configure_file(org.kde.kameleon.qmk.helper.service.in
               ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kameleon.qmk.helper.service
               @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.kameleon.qmk.helper.service
        DESTINATION ${KDE_INSTALL_DBUSSYSTEMSERVICEDIR})
