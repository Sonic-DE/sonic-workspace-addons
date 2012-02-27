set(krunner_books_SRCS
    imageiconengine.cpp
    booksjob.cpp
    books.cpp
)

kde4_add_plugin(krunner_books ${krunner_books_SRCS})
target_link_libraries(krunner_books ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS krunner_books DESTINATION ${PLUGIN_INSTALL_DIR} )

install(FILES books.desktop DESTINATION ${SERVICES_INSTALL_DIR})

