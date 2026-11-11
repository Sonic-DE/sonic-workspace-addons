plasma_add_applet(org.sonicde.terminalapplet
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

target_link_libraries(org.sonicde.terminalapplet PRIVATE
    Qt::Core
    Qt::Qml
    Sonic::Logging
)
