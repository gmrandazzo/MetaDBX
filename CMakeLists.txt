PROJECT(MetaDBX)
cmake_minimum_required(VERSION 2.8)
set(AUTHOR "Giuseppe Marco Randazzo")
set(VERSION "1.0.0")


find_package(Qt4 REQUIRED COMPONENTS QtCore QtGui QtXml)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})


if(WIN32)
  message(STATUS "OS: Windows")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pipe -mwindows -pedantic -Wall -static -DWIN32")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -s")
  set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
else(WIN32)
  message(STATUS "OS: Unix")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pedantic -Wall -Werror -DLINUX -g")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -g3")
endif(WIN32)

set(CMAKE_INSTALL_PREFIX "/usr/local/")


add_subdirectory(src)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")
