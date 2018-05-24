# Utilities

add_executable(hosp-get hosp-get.c util.c)
target_link_libraries(hosp-get ${PROJECT_NAME})

add_executable(hosp-set hosp-set.c util.c)
target_link_libraries(hosp-set ${PROJECT_NAME})

add_executable(hosp-poll hosp-poll.c util.c)
target_link_libraries(hosp-poll ${PROJECT_NAME})

install(TARGETS hosp-get
                hosp-set
                hosp-poll
        DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY man/ DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
