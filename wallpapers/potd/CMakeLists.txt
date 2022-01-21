plasma_install_package(package org.kde.potd wallpapers wallpaper)

set(potdplugin_SRCS
    plugin/backend.cpp
    plugin/potdplugin.cpp
)

install(FILES plugin/qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/potd/private)

add_library(potdplugin SHARED ${potdplugin_SRCS})

target_link_libraries(
    potdplugin
    Qt::Core
    Qt::Qml # potdplugin.h
    Qt::Widgets # QFileDialog in backend.cpp
    KF5::I18n
    KF5::KIOCore # CopyJob in backend.cpp
)

install(TARGETS potdplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/potd/private)
