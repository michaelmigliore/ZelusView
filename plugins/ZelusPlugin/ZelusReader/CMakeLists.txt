set(classes
  vtkZelusBinaryReader
  vtkZelusExtractType
  vtkZelusExtractCell)

include(vtkModule)

vtk_module_add_module(ZelusReader
  FORCE_STATIC
  CLASSES ${classes})

target_link_libraries(ZelusReader PRIVATE Zelus)

paraview_add_server_manager_xmls(
  XMLS ZelusReader.xml)
