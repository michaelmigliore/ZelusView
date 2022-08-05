set(CMAKE_AUTOMOC 1)
set(CMAKE_AUTOUIC 1)

include(ParaViewClient)

set(sources_files
  ZelusViewMainWindow.cpp
  ZelusViewMainWindow.h)
set(ui_files
  ZelusViewMainWindow.ui)
set(xml_files
  ZelusFilters.xml
  ZelusSources.xml)
set(resource_files
  ZelusView.qrc)

# The main cmake macro to create a paraview based application
paraview_client_add(
  NAME ZelusView
  VERSION "1.0"
  SOURCES ${sources_files} ${ui_files} ${resource_files}
  PLUGINS_TARGETS zelusview_plugins
  REQUIRED_PLUGINS ZelusPlugin
  APPLICATION_XMLS ${xml_files}
  MAIN_WINDOW_CLASS ZelusViewMainWindow
  APPLICATION_ICON "${CMAKE_CURRENT_SOURCE_DIR}/z-emotion.ico"
  ORGANIZATION "z-emotion"
  TITLE "Zelus View"
)

# In order to debug, we need to set ParaView and Qt path to the debugger PATH
set_target_properties(ZelusView PROPERTIES
	VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${QT5_ROOT}/bin;${ParaView_BINARY_DIR}/bin/$<CONFIG>"
)
