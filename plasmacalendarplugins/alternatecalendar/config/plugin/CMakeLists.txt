add_library(plasmacalendaralternatecalendarconfig SHARED configplugin.cpp configstorage.cpp ../../calendarsystem.h)

target_link_libraries(plasmacalendaralternatecalendarconfig
    Qt::Qml
    Qt::Core
    KF5::ConfigCore
    KF5::I18n
)

install(TARGETS plasmacalendaralternatecalendarconfig DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasmacalendar/alternatecalendarconfig)
install(FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/plasmacalendar/alternatecalendarconfig)
