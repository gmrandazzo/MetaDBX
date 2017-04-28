PROJECT(MetaDBX)
cmake_minimum_required(VERSION 2.8)
set(AUTHOR "Giuseppe Marco Randazzo")
set(VERSION "1.0.0")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_CXX_STANDARD 11)

#set(_components
#    Core
#    Gui
#    DBus
#    Designer
#    Declarative
#    Script
#    ScriptTools
#    Network
#    Test
#    Xml
#    Svg
#    Sql
#    Widgets
#    PrintSupport
#    Concurrent
#    UiTools
#    WebKit
#    WebKitWidgets
#    OpenGL)

set(_components Core Gui Widgets PrintSupport Concurrent Xml)
foreach(_component ${_components})
  find_package(Qt5${_component})
  list(APPEND QT5_LIBRARIES ${Qt5${_component}_LIBRARIES})
endforeach()


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

string(TOLOWER "${CMAKE_BUILD_TYPE}" cmake_build_type_tolower)

if(cmake_build_type_tolower STREQUAL "debug")
  set(CMAKE_BUILD_TYPE "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wall -pedantic -g")
else()
  set(CMAKE_BUILD_TYPE "Release")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")
endif()

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

add_definitions(${QT5_DEFINITIONS})

include_directories(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory (src)
