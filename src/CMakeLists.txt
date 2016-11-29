#MetaSDFX
set(metasdfx_icon_SRC icons/icons.qrc)
set(metasdfx_SRC main_metasdfx.cpp
                metasdfx.cpp
                aboutdialog.cpp
                hmdbreader.cpp
                genericreader.cpp)

set(metasdfx_H metasdfx.h
              aboutdialog.h
              hmdbreader.h
              genericreader.h
              database.h)

set(metasdfx_UIS metasdfx.ui aboutdialog.ui)


#DynMetID
set(dynmetid_SRC dymetdb.cpp
                 dynmetid.cpp
                 aboutdialog.cpp
                 main_dynmetid.cpp)

set(dynmetid_H dymetdb.h
              dynmetid.h
              aboutdialog.h)

set(dynmetid_UIS dynmetid.ui aboutdialog.ui)

set(testdynmetdb_SRC test_dymetdb.cpp dymetdb.cpp)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

#set(icon_RCC icons/icons.qrc)
# start building interface from moc
QT5_WRAP_CPP(metasdfx_MOC_H ${metasdfx_H})
QT5_WRAP_UI(metasdfx_UIS_H ${metasdfx_UIS})

QT5_WRAP_CPP(dynmetid_MOC_H ${dynmetid_H})
QT5_WRAP_UI(dynmetid_UIS_H ${dynmetid_UIS})

# generate rules for building source files from the resources
#QT5_ADD_RESOURCES(icon_RCC_SRCS ${icon_RCC})

source_group("Resources" FILES ${metasdfx_UIS} ${dynmetid_UIS})
source_group("Generated" FILES ${metasdfx_UIS_H} ${metasdfx_MOC_H} ${dynmetid_UIS_H} ${dynmetid_MOC_H})

if(WIN32)
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)
  set(METASDFX_APP_ICON_RC ${CMAKE_CURRENT_SOURCE_DIR}/metasdfx_icon.rc)
  if(METASDFX_APP_ICON_RC)
    set(METASDFX_WIN32_ICON_O ${CMAKE_CURRENT_BINARY_DIR}/_metasdfx_app_icon.o)
    ADD_CUSTOM_COMMAND( OUTPUT ${METASDFX_WIN32_ICON_O} COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -o${METASDFX_WIN32_ICON_O} -i${METASDFX_APP_ICON_RC})
  endif()
  set(DYNMETID_APP_ICON_RC ${CMAKE_CURRENT_SOURCE_DIR}/dynmetid_icon.rc)
  if(DYNMETID_APP_ICON_RC)
    set(DYNMETID_WIN32_ICON_O ${CMAKE_CURRENT_BINARY_DIR}/_dynmetid_app_icon.o)
    ADD_CUSTOM_COMMAND( OUTPUT ${DYNMETID_WIN32_ICON_O} COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -o${DYNMETID_WIN32_ICON_O} -i${DYNMETID_APP_ICON_RC})
  endif()
endif()


if(WIN32)
  add_executable(metasdfx ${metasdfx_SRC} ${metasdfx_MOC_H} ${metasdfx_UIS_H} ${icon_RCC_SRCS} ${METASDFX_WIN32_ICON_O})
  add_executable(dynmetid ${dynmetid_SRC} ${dynmetid_MOC_H} ${dynmetid_UIS_H} ${icon_RCC_SRCS} ${DYNMETID_WIN32_ICON_O})
else(WIN32)
  add_executable(metasdfx ${metasdfx_SRC} ${metasdfx_MOC_H} ${metasdfx_UIS_H} ${icon_RCC_SRCS})
  add_executable(dynmetid ${dynmetid_SRC} ${dynmetid_MOC_H} ${dynmetid_UIS_H} ${icon_RCC_SRCS})
endif(WIN32)

add_executable(testdynmetdb ${testdynmetdb_SRC})
target_link_libraries(testdynmetdb m)

target_link_libraries(metasdfx ${QT5_LIBRARIES})
target_link_libraries(dynmetid ${QT5_LIBRARIES})
install(TARGETS metasdfx DESTINATION bin COMPONENT ApplicationData)
install(TARGETS dynmetid DESTINATION bin COMPONENT ApplicationData)
