cmake_minimum_required(VERSION 2.8.5)

project(hosp)
set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)
set(PROJECT_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99")

include_directories(${PROJECT_SOURCE_DIR}/inc)

if(CMAKE_HOST_UNIX)
  include(GNUInstallDirs)
else()
  set(CMAKE_INSTALL_LIBDIR lib)
  set(CMAKE_INSTALL_INCLUDEDIR include)
  set(CMAKE_INSTALL_BINDIR bin)
  set(CMAKE_INSTALL_MANDIR share/man)
endif()


# Dependencies

find_package(PkgConfig REQUIRED)
# Check for impl with libusb backend first
# hidapi docs only specify it for Linux and FreeBSD, but libusb is quite portable
set(HIDAPI_IMPL hidapi-libusb)
pkg_search_module(HIDAPI ${HIDAPI_IMPL})
if(NOT HIDAPI_FOUND)
  # Now look for more platform-specific implementations
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux|Android")
    # Check for impl with hidraw backend
    set(HIDAPI_IMPL hidapi-hidraw)
    pkg_search_module(HIDAPI ${HIDAPI_IMPL})
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # Check for impl with IOHidManager backend
    set(HIDAPI_IMPL hidapi)
    pkg_search_module(HIDAPI ${HIDAPI_IMPL})
  elseif(WIN32)
    # Check for impl with dll backend (hidapi.dll)
    set(HIDAPI_IMPL hidapi)
    pkg_search_module(HIDAPI ${HIDAPI_IMPL})
  endif()
endif()
if(NOT HIDAPI_FOUND)
  message(FATAL_ERROR "Could not find dependency: hidapi")
endif()

include_directories(${HIDAPI_INCLUDE_DIRS})


# Libraries

add_library(${PROJECT_NAME} src/hosp.c)
target_link_libraries(${PROJECT_NAME} -L${HIDAPI_LIBDIR} ${HIDAPI_LIBRARIES})
if(BUILD_SHARED_LIBS)
  set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION}
                                                   SOVERSION ${VERSION_MAJOR})
endif()


# Utilities

add_subdirectory(utils)


# pkg-config

set(PKG_CONFIG_EXEC_PREFIX "\${prefix}")
set(PKG_CONFIG_LIBDIR "\${prefix}/${CMAKE_INSTALL_LIBDIR}")
set(PKG_CONFIG_INCLUDEDIR "\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}")
set(PKG_CONFIG_CFLAGS "-I\${includedir}")

set(PKG_CONFIG_NAME "${PROJECT_NAME}")
set(PKG_CONFIG_DESCRIPTION "Library for managing an ODROID Smart Power")
set(PKG_CONFIG_LIBS "-L\${libdir} -l${PROJECT_NAME}")
set(PKG_CONFIG_REQUIRES_PRIVATE "${HIDAPI_IMPL}")
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/pkgconfig.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc
)


# Install

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(FILES inc/hosp.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)


# Uninstall

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  @ONLY
)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
