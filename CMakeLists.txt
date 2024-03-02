cmake_minimum_required(VERSION 3.6)

project(hosp VERSION 0.2.0
             LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
add_compile_options(-Wall)

include(GNUInstallDirs)


# Dependencies

find_package(PkgConfig REQUIRED)
# Check for impl with libusb backend first
# hidapi docs only specify it for Linux and FreeBSD, but libusb is quite portable
set(HIDAPI_IMPL hidapi-libusb)
pkg_search_module(HIDAPI IMPORTED_TARGET ${HIDAPI_IMPL})
if(NOT HIDAPI_FOUND)
  # Now look for more platform-specific implementations
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux|Android")
    # Check for impl with hidraw backend
    set(HIDAPI_IMPL hidapi-hidraw)
    pkg_search_module(HIDAPI IMPORTED_TARGET ${HIDAPI_IMPL})
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Check for impl with IOHidManager backend
    set(HIDAPI_IMPL hidapi)
    pkg_search_module(HIDAPI IMPORTED_TARGET ${HIDAPI_IMPL})
  elseif(WIN32)
    # Check for impl with dll backend (hidapi.dll)
    set(HIDAPI_IMPL hidapi)
    pkg_search_module(HIDAPI IMPORTED_TARGET ${HIDAPI_IMPL})
  endif()
endif()
if(NOT HIDAPI_FOUND)
  message(FATAL_ERROR "Could not find dependency: hidapi")
endif()


# Libraries

add_library(hosp src/hosp.c)
target_include_directories(hosp PRIVATE ${PROJECT_SOURCE_DIR}/inc
                                PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/inc>
                                       $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/hosp>)
set_target_properties(hosp PROPERTIES PUBLIC_HEADER "${PROJECT_SOURCE_DIR}/inc/hosp.h")
target_link_libraries(hosp PRIVATE PkgConfig::HIDAPI)
if(BUILD_SHARED_LIBS)
  set_target_properties(hosp PROPERTIES VERSION ${PROJECT_VERSION}
                                        SOVERSION ${PROJECT_VERSION_MAJOR})
endif()
install(TARGETS hosp LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                     PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/hosp)


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
set(PKG_CONFIG_REQUIRES "")
set(PKG_CONFIG_REQUIRES_PRIVATE "${HIDAPI_IMPL}")
set(PKG_CONFIG_CFLAGS "-I\${includedir}")
set(PKG_CONFIG_LIBS "-L\${libdir} -lhosp")
set(PKG_CONFIG_LIBS_PRIVATE "")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig.in
  ${CMAKE_CURRENT_BINARY_DIR}/hosp.pc
  @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/hosp.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)


# Uninstall

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  @ONLY
)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
