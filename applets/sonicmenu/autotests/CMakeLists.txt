add_executable(sonicmenutest
    sonicmenutest.cpp
    ../commandrunner.cpp
    ../logging.cpp
)
target_compile_definitions(sonicmenutest PRIVATE SONICMENU_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/..")
target_include_directories(sonicmenutest PRIVATE ..)
target_link_libraries(sonicmenutest Qt::Qml Qt::Test Sonic::Logging)
add_test(NAME sonicmenutest COMMAND sonicmenutest)
