#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "TetoEngine::TetoEngine" for configuration "Debug"
set_property(TARGET TetoEngine::TetoEngine APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(TetoEngine::TetoEngine PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/libTetoEngine.a"
  )

list(APPEND _cmake_import_check_targets TetoEngine::TetoEngine )
list(APPEND _cmake_import_check_files_for_TetoEngine::TetoEngine "${_IMPORT_PREFIX}/lib/libTetoEngine.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
