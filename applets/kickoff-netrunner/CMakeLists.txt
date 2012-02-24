project(kickoff-netrunner)

add_subdirectory(apps)

install(DIRECTORY package/
        DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/kickoff-netrunner)

install(FILES package/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-kickoff-netrunner.desktop)
