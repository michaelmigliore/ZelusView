paraview_add_plugin(ZelusPlugin
  VERSION "1.0"
  MODULES ZelusReader
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ZelusReader/vtk.module"
  FORCE_STATIC ON
  XML_DOCUMENTATION OFF)
