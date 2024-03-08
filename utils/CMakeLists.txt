# Utilities

add_executable(hosp-get hosp-get.c util.c)
target_link_libraries(hosp-get PRIVATE hosp)

add_executable(hosp-set hosp-set.c util.c)
target_link_libraries(hosp-set PRIVATE hosp)

add_executable(hosp-poll hosp-poll.c util.c)
target_link_libraries(hosp-poll PRIVATE hosp)

add_executable(hosp-enumerate hosp-enumerate.c)
target_link_libraries(hosp-enumerate PRIVATE hosp)

install(TARGETS hosp-get
                hosp-set
                hosp-poll
                hosp-enumerate
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY man/ DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
