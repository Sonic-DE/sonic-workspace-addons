find_package(KWin REQUIRED)
find_package(KF6 REQUIRED COMPONENTS Config)

kcoreaddons_add_plugin(kwin_reflectcursor STATIC INSTALL_NAMESPACE "kwin/effects/plugins")

target_compile_definitions(kwin_reflectcursor PRIVATE -DTRANSLATION_DOMAIN=\"kwin_effect_reflectcusor\")
install(FILES metadata.json DESTINATION ${KDE_INSTALL_DATADIR}/kwin/builtin-effects/ RENAME kwin_reflectcursor.json)

target_sources(kwin_reflectcursor PRIVATE
    main.cpp
    reflectcursor.cpp
)

kconfig_add_kcfg_files(kwin_reflectcursor
    reflectcursorconfig.kcfgc
)

target_link_libraries(kwin_reflectcursor PRIVATE
    KF6::ConfigGui

    KWin::kwin
)
