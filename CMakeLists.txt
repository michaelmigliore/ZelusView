cmake_minimum_required(VERSION 3.18)

project(ZelusView)

find_package(ParaView)

add_subdirectory(Zelus)

set(CMAKE_AUTOMOC 1)
set(CMAKE_AUTOUIC 1)

set(sources
  ZelusViewMainWindow.cpp
  ZelusViewMainWindow.h
  ZelusViewMainWindow.ui)

#add_subdirectory(VTKExtension)

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
