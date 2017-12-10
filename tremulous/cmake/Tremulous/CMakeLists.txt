include( CMakePackageConfigHelpers )

configure_package_config_file(
  TremulousConfig.cmake.in
  TremulousConfig.cmake
  INSTALL_DESTINATION "${INSTDIR_CMAKE}"
  PATH_VARS
    INSTDIR_EXE
    INSTDIR_DLL
    INSTDIR_TOOLS
    INSTDIR_INCLUDE
    INSTDIR_DATA
    INSTDIR_DOCS )

configure_file(
  TremulousConfigVersion.cmake.in
  TremulousConfigVersion.cmake
  ESCAPE_QUOTES @ONLY )

install( FILES
  "${CMAKE_CURRENT_BINARY_DIR}/TremulousConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/TremulousConfigVersion.cmake"
  AddTremulousPk3.cmake
  AddTremulousGame.cmake
  DESTINATION "${INSTDIR_CMAKE}" )
