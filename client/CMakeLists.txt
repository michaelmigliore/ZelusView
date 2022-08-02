set(CMAKE_AUTOMOC 1)
set(CMAKE_AUTOUIC 1)

set(sources
  ZelusViewMainWindow.cpp
  ZelusViewMainWindow.h)
set(ui_files
  ZelusViewMainWindow.ui)
set(xml_files
  ZelusFilters.xml)

# The main cmake macro to create a paraview based application
paraview_client_add(
  NAME ZelusView
  VERSION "1.0"
  SOURCES ${sources} ${ui_files}
  PLUGINS_TARGETS zelusview_plugins
  REQUIRED_PLUGINS ZelusPlugin
  APPLICATION_XMLS ${xml_files}
  MAIN_WINDOW_CLASS ZelusViewMainWindow
  ORGANIZATION "z-emotion Inc."
  TITLE "Zelus View")
