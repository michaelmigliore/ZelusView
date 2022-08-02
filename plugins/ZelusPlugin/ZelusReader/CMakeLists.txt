set(classes
  vtkZelusBinaryReader)

vtk_module_add_module(ZelusReader
  FORCE_STATIC # Using FORCE_STATIC build the vtk module statically into the plugin library, to avoid confusion when loading
  CLASSES ${classes})

target_link_libraries(ZelusReader PRIVATE Zelus)

paraview_add_server_manager_xmls(
  XMLS ZelusReader.xml)
