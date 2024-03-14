cmake_minimum_required(VERSION 3.16)

project(hosp VERSION 0.2.0
             LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
add_compile_options(-Wall)

include(GNUInstallDirs)


# Dependencies

# "hidapi" is used for both macOS (IOHidManager) and Windows (DLL) backends.
# "hidapi-hidraw" is for Linux.
# "hidapi-libusb" is for Linux, BSD, and other UNIX-like systems.
set(HOSP_HIDAPI_PC_MODULES_DEFAULT hidapi hidapi-hidraw hidapi-libusb)
set(HOSP_HIDAPI_PC_MODULES ${HOSP_HIDAPI_PC_MODULES_DEFAULT} CACHE STRING
    ";-list of HIDAPI pkg-config modules to search for, e.g., an ordered subset of: ${HOSP_HIDAPI_PC_MODULES_DEFAULT}")
mark_as_advanced(HOSP_HIDAPI_PC_MODULES)

find_package(PkgConfig REQUIRED)
pkg_search_module(HIDAPI REQUIRED IMPORTED_TARGET ${HOSP_HIDAPI_PC_MODULES})
message(STATUS "Using HIDAPI module: ${HIDAPI_MODULE_NAME}")

# Libraries

add_library(hosp src/hosp.c)
target_include_directories(hosp PRIVATE ${PROJECT_SOURCE_DIR}/inc
                                PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc>
                                       $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/hosp>)
set_target_properties(hosp PROPERTIES PUBLIC_HEADER "${PROJECT_SOURCE_DIR}/inc/hosp.h")
target_link_libraries(hosp PUBLIC PkgConfig::HIDAPI)
if(BUILD_SHARED_LIBS)
  set_target_properties(hosp PROPERTIES VERSION ${PROJECT_VERSION}
                                        SOVERSION ${PROJECT_VERSION_MAJOR})
endif()
install(TARGETS hosp LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                             COMPONENT HOSP_Runtime
                             NAMELINK_COMPONENT HOSP_Development
                     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                             COMPONENT HOSP_Development
                     PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hosp
                                   COMPONENT HOSP_Development)


# Utilities

add_subdirectory(utils)


# pkg-config

set(PKG_CONFIG_PREFIX "${CMAKE_INSTALL_PREFIX}")
set(PKG_CONFIG_EXEC_PREFIX "\${prefix}")
if(IS_ABSOLUTE "${CMAKE_INSTALL_INCLUDEDIR}")
  set(PKG_CONFIG_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/hosp")
else()
  set(PKG_CONFIG_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}/hosp")
endif()
if(IS_ABSOLUTE "${CMAKE_INSTALL_LIBDIR}")
  set(PKG_CONFIG_LIBDIR "${CMAKE_INSTALL_LIBDIR}")
else()
  set(PKG_CONFIG_LIBDIR "\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}")
endif()
set(PKG_CONFIG_NAME "hosp")
set(PKG_CONFIG_DESCRIPTION "Library for managing an ODROID Smart Power")
set(PKG_CONFIG_REQUIRES "${HIDAPI_MODULE_NAME}")
set(PKG_CONFIG_REQUIRES_PRIVATE "")
set(PKG_CONFIG_CFLAGS "-I\${includedir}")
set(PKG_CONFIG_LIBS "-L\${libdir} -lhosp")
set(PKG_CONFIG_LIBS_PRIVATE "")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig.in
  ${CMAKE_CURRENT_BINARY_DIR}/hosp.pc
  @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hosp.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
        COMPONENT HOSP_Development)


# Uninstall

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  @ONLY
)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
