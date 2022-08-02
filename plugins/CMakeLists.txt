option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Classic plugin scan/build
# Each plugin should be listed in the PLUGIN_FILES arg of paraview_plugin_scan

paraview_plugin_scan(
  PLUGIN_FILES
    ZelusPlugin/paraview.plugin
  PROVIDES_PLUGINS plugins
  ENABLE_BY_DEFAULT ON
  HIDE_PLUGINS_FROM_CACHE ON)

paraview_plugin_build(
  RUNTIME_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY_SUBDIRECTORY "${PARAVIEW_PLUGIN_SUBDIR}"
  PLUGINS_FILE_NAME "ZelusView.xml"
  CMAKE_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ZelusView
  LIBRARY_SUBDIRECTORY "ZelusView"
  PLUGINS ${plugins}
  TARGET zelusview_plugins)
