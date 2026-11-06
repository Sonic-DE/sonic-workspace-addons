plasma_add_applet(org.sonicde.terminalpanel
    QML_SOURCES
        config.qml
        ConfigGeneral.qml
        main.qml
    RESOURCES
        main.xml
    CPP_SOURCES
        logging.cpp
    GENERATE_APPLET_CLASS
)

target_link_libraries(org.sonicde.terminalpanel PRIVATE
    Qt::Core
    Qt::Qml
    Sonic::Logging
)

plasma_install_package(layout-template org.sonicde.plasma.desktop.terminalPanel layout-templates layout-template)
