#===============================================================================
#
# This is the main file of the Tremulous engine build system.
#
# See cmake/Options.cmake for the full list of options.
#
#===============================================================================

#-------------------------------------------------------------------------------
# Basics
#-------------------------------------------------------------------------------

# Project information.
cmake_minimum_required( VERSION 3.6 )
project( tremulous-engine )

# Add our custom CMake module path.
list( INSERT CMAKE_MODULE_PATH 0 "${PROJECT_SOURCE_DIR}/cmake/Modules" )

# Include utilities.
include( cmake/Utilities.cmake )

# Include engine options.
include( cmake/Options.cmake )

# Version of the engine.
set( Tremulous_VERSION_MAJOR 1 )
set( Tremulous_VERSION_MINOR 3 )
set( Tremulous_VERSION_PATCH 0 )
set( Tremulous_VERSION_TWEAK 0 )
set( Tremulous_VERSION_COUNT 3 )
set( Tremulous_VERSION_EXTRA "-beta.1" )

# Create Tremulous_VERSION_STRING from the individual components.
make_version_string( Tremulous )

# Use C++14 and C99 standards.
set( CMAKE_C_STANDARD 99 )
set( CMAKE_CXX_STANDARD 14 )

# Enable position-independent code.
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

# Use our custom CMake module directory.
list( APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )

# Toplevel "src" directory.
set( TOP_SRCDIR "${PROJECT_SOURCE_DIR}/src" )

#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------

find_package( ZLIB REQUIRED )
find_package( Lua REQUIRED )
find_package( Sol REQUIRED )
find_package( RapidJSON REQUIRED )
find_package( CURL REQUIRED )

add_library( EXT::RapidJSON INTERFACE IMPORTED )
set_target_properties( EXT::RapidJSON PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${RAPIDJSON_INCLUDE_DIR}" )

if( BUILD_CLIENT )
  find_package( OpenGL REQUIRED )
  find_package( OpenAL REQUIRED )
  find_package( SDL2 REQUIRED )
  find_package( JPEG REQUIRED )
  find_package( Ogg REQUIRED )
  find_package( Vorbis REQUIRED )
endif( )

#-------------------------------------------------------------------------------
# Installation Layout
#-------------------------------------------------------------------------------

set( PATH_COMPILE_DEFINITIONS )

if( NOT INSTALL_TYPE MATCHES ${INSTALL_TYPES} )
  message( FATAL_ERROR "Unknown install type (${INSTALL_TYPES})" )
endif( )

if( WIN32 OR INSTALL_TYPE STREQUAL Debug )
  set( INSTDIR_EXE "." )
  set( INSTDIR_DLL "libs" )
  set( INSTDIR_TOOLS "tools" )
  set( INSTDIR_CMAKE "cmake" )
  set( INSTDIR_INCLUDE "include" )
  set( INSTDIR_DATA "data" )
  set( INSTDIR_DOCS "docs" )
else( )
  include( GNUInstallDirs )
  set( INSTDIR_EXE "${CMAKE_INSTALL_BINDIR}" )
  set( INSTDIR_DLL "${CMAKE_INSTALL_LIBDIR}/tremulous" )
  set( INSTDIR_TOOLS "${CMAKE_INSTALL_LIBEXECDIR}/tremulous" )
  set( INSTDIR_CMAKE "${CMAKE_INSTALL_DATADIR}/cmake/Tremulous" )
  set( INSTDIR_INCLUDE "${CMAKE_INSTALL_INCLUDEDIR}/tremulous" )
  set( INSTDIR_DATA "${CMAKE_INSTALL_DATADIR}/tremulous" )
  set( INSTDIR_DOCS "${CMAKE_INSTALL_DATADIR}/doc/tremulous" )
  set( PATH_COMPILE_DEFINITIONS
    INSTDIR_DATA="${INSTDIR_DATA}"
    INSTDIR_DLL="${INSTDIR_DLL}" )
endif( )

#-------------------------------------------------------------------------------
# Build Engine Components
#-------------------------------------------------------------------------------

# Built-in libraries.
add_subdirectory( external/nettle-3.3 )
add_subdirectory( external/restclient )
add_subdirectory( external/semver )

# Collect source files.
include( src/asm/Sources.cmake )
include( src/qcommon/Sources.cmake )
include( src/sys/Sources.cmake )
include( src/server/Sources.cmake )
include( src/null/Sources.cmake )
include( src/sdl/Sources.cmake )

# Tremulous server and the common stuff.
if( BUILD_CLIENT OR BUILD_SERVER )
  add_subdirectory( src/script )

  set( COMMON_INCLUDE_DIRECTORIES "${TOP_SRCDIR}" )

  set( COMMON_COMPILE_DEFINITIONS
    USE_RESTCLIENT
    USE_RENDERER_DLOPEN
    USE_OPENAL
    PRODUCT_VERSION="${Tremulous_VERSION_STRING}"
    PRIMARY_MOD="${PRIMARY_MOD}"
    ${PATH_COMPILE_DEFINITIONS} )

  set( COMMON_LINK_LIBRARIES
    ${CMAKE_DL_LIBS}
    ZLIB::ZLIB
    Lua::lua
    Sol::sol
    EXT::RapidJSON
    nettle
    restclient
    semver
    script_api )
endif( )

# Tremulous client.
if( BUILD_CLIENT )
  add_subdirectory( src/client )
  add_subdirectory( src/renderercommon )
  add_subdirectory( src/renderergl1 )
  add_subdirectory( src/renderergl2 )
endif( )

# Tremulous server.
if( BUILD_SERVER )
  add_subdirectory( src/server )
endif( )

# QVM toolchain.
if( BUILD_SDK )
  add_subdirectory( src/qvmtools )
  add_subdirectory( src/qcommon )
  add_subdirectory( src/ui )
  add_subdirectory( src/game )
  add_subdirectory( src/cgame )
endif( )

# CMake package.
if( INSTALL_CMAKE_PACKAGE )
  add_subdirectory( cmake/Tremulous )
endif( )

# Documentation.
if( INSTALL_DOCS )
  add_subdirectory( docs )
endif( )
