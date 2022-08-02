paraview_plugin_scan(
    PLUGIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/paraview.plugin"
    PROVIDES_PLUGINS plugins
    ENABLE_BY_DEFAULT ON
    HIDE_PLUGINS_FROM_CACHE ON)

paraview_plugin_build(
    RUNTIME_DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY_SUBDIRECTORY "${PARAVIEW_PLUGIN_SUBDIR}"
    PLUGINS_FILE_NAME "ZelusView.xml"
    INSTALL_EXPORT ZelusViewPlugins
    CMAKE_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ZelusView
    LIBRARY_SUBDIRECTORY "ZelusView"
    PLUGINS ${plugins}
    TARGET zelusview_plugins)

paraview_add_plugin(ZelusReader
    REQUIRED_ON_SERVER
    VERSION "0.1"
    MODULES ZelusReader::vtkZelusReader
    MODULE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/ZelusReader/vtk.module")
