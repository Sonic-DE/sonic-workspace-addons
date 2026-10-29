# Sonic Menu, based on kMenu by 51n7 (https://github.com/51n7/kMenu)
plasma_add_applet(org.sonicde.sonicmenu
    QML_SOURCES
        config.qml
        ConfigEditJson.qml
        ConfigGeneral.qml
        ConfigMenuJson.qml
        ItemListDelegate.qml
        ItemListDialog.qml
        ItemListView.qml
        JsonMenuModel.qml
        main.qml
        MenuBuilder.qml
        MenuRepresentation.qml
        SessionActions.qml
        TreeItemDelegate.qml
    RESOURCES
        main.xml
    CPP_SOURCES
        commandrunner.cpp
        commandrunner.h
        logging.cpp
    GENERATE_APPLET_CLASS
)

target_link_libraries(org.sonicde.sonicmenu PRIVATE
    Qt::Core
    Qt::Qml
    Sonic::Logging
)

if(BUILD_TESTING)
    add_subdirectory(autotests)
endif()
