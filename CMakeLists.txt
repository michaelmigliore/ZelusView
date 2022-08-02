cmake_minimum_required(VERSION 3.18)

project(ZelusView)

include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

find_package(ParaView)

add_subdirectory(Zelus)

set(CMAKE_AUTOMOC 1)
set(CMAKE_AUTOUIC 1)

set(sources
  ZelusViewMainWindow.cpp
  ZelusViewMainWindow.h
  ZelusViewMainWindow.ui)

add_subdirectory(ZelusPlugin)

paraview_client_add(
  NAME ZelusView
  TITLE "Zelus View"
  ORGANIZATION "z-emotion"
  VERSION "0.1"
  MAIN_WINDOW_CLASS ZelusViewMainWindow
  SOURCES ${sources}
  APPLICATION_XMLS "${CMAKE_CURRENT_SOURCE_DIR}/Zelus.xml"
  )

target_link_libraries(ZelusView PRIVATE Zelus)
set_target_properties(ZelusView PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
