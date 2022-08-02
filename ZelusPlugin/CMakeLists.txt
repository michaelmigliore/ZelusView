paraview_add_plugin(ZelusReader
  REQUIRED_ON_SERVER
  VERSION "0.1"
  MODULES ZelusReader::vtkZelusReader
  MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ZelusReader/vtk.module")
