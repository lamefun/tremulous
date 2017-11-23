#===============================================================================
# Project information
#===============================================================================

cmake_minimum_required(VERSION 3.6)
project(tremulous)

# Tremulous version.
set( PRODUCT_VERSION 1.3.0 )

# Change these when working on a mod.
set( GAME_MOD_NAME gpp )
set( GAME_PK3_NAME tremulous )


#===============================================================================
# Options
#===============================================================================

option( BUILD_CLIENT "Build Tremulous client" ON )
option( BUILD_SERVER "Build Tremulous server" ON )
option( BUILD_GAME "Build Tremulous game logic" ON )
option( INTERNAL_DEPS "Use internal dependencies where possible" OFF )
set( INSTALL_LAYOUT "Regular" CACHE STRING
     "Installation layout (Regular|Portable|Windows)" )


#===============================================================================
# Basics
#===============================================================================

# Use C++14 and C99 standards.
set( CMAKE_C_STANDARD 99 )
set( CMAKE_CXX_STANDARD 14 )

# Enable position-independent code.
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

# Use our custom CMake module directory.
list( APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )

# Include our utility module.
include( TremulousUtils )

# Toplevel "src" directory.
set( TOP_SRCDIR "${CMAKE_CURRENT_SOURCE_DIR}/src" )

# Tremulous include directories.
set( TREMULOUS_INCLUDE_DIRECTORIES "${TOP_SRCDIR}" )

# Tremulous compiler definitions.
set( TREMULOUS_COMPILE_DEFINITIONS
  USE_RESTCLIENT
  USE_RENDERER_DLOPEN
  USE_OPENAL
  PRODUCT_VERSION="${PRODUCT_VERSION}"
  GAMEMOD="${GAME_MOD_NAME}" )

# Add WIN32 flag if building on Windows.
if( WIN32 )
  list( APPEND TREMULOUS_COMPILE_DEFINITIONS WIN32 )
endif( )


#===============================================================================
# Installation directories
#===============================================================================

# Check if the installation layout is supported.
if( NOT INSTALL_LAYOUT MATCHES Regular|Portable|Windows )
  message( FATAL_ERROR
    "The \"${INSTALL_LAYOUT}\" installation layout is not supported."
    "Supported layouts are: Regular, Portable and Windows." )
endif( )

# Check whether to install the Freedesktop application metadata used by Linux
# and most other UNIX-like systems (for example, the icons and the application
# launcher).
set( INSTALL_FREEDESKTOP_METADATA FALSE )
if( INSTALL_LAYOUT STREQUAL Regular AND UNIX AND NOT APPLE )
  set( INSTALL_FREEDESKTOP_METADATA TRUE )
endif( )

# Use a flat installation layout for Windows and portable builds. Use the
# standard GNU installation directories in all other cases.
if( INSTALL_LAYOUT MATCHES Portable|Windows )
  set( GAME_EXE_INSTALL_DIR "." )
  set( TOOL_EXE_INSTALL_DIR "tools" )
  set( SHARED_MODULE_INSTALL_DIR "libs" )
  set( BASE_DATA_INSTALL_DIR "base" )
  set( GAME_DATA_INSTALL_DIR "${GAME_MOD_NAME}" )
  set( DOC_INSTALL_DIR "docs" )
else( )
  include( GNUInstallDirs )
  set( TREMULOUS_DATA_DIR "${CMAKE_INSTALL_FULL_DATADIR}/tremulous" )
  set( GAME_EXE_INSTALL_DIR "${CMAKE_INSTALL_FULL_BINDIR}" )
  set( TOOL_EXE_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBEXECDIR}/tremulous" )
  set( SHARED_MODULE_INSTALL_DIR "${CMAKE_INSTALL_FULL_LIBDIR}/tremoulous" )
  set( BASE_DATA_INSTALL_DIR "${TREMULOUS_DATA_DIR}/base" )
  set( GAME_DATA_INSTALL_DIR "${TREMULOUS_DATA_DIR}/${GAME_MOD_NAME}" )
  set( DOC_INSTALL_DIR "${CMAKE_INSTALL_FULL_DATADIR}/doc/tremulous" )
  list( APPEND TREMULOUS_COMPILE_DEFINITIONS
    FS_DEFAULT_BASEPATH="${TREMULOUS_DATA_DIR}"
    SYS_SHARED_MODULE_DIR="${SHARED_MODULE_INSTALL_DIR}" )
endif( )


#===============================================================================
# Dependencies
#===============================================================================

find_program( PYTHON3_COMMAND
  NAMES python3 python
  DOC "Python 3 command" )

if( PYTHON3_COMMAND STREQUAL "PYTHON3_COMMAND-NOTFOUND" )
  message( FATAL_ERROR "Python 3 not found" )
endif( )

find_dependency( ZLIB REQUIRED
  PKG_CONFIG zlib
  INTERNAL_SOURCE_DIRS external/zlib
  INTERNAL_LIB_TARGETS zlib )

find_dependency( LUA REQUIRED
  PKG_CONFIG lua
  INTERNAL_SOURCE_DIRS external/lua-5.3.3
  INTERNAL_LIB_TARGETS lua )

find_dependency( SOL REQUIRED
  INTERNAL_SOURCE_DIRS external/sol
  INTERNAL_LIB_TARGETS sol )

find_dependency( CURL REQUIRED
  PKG_CONFIG libcurl )

find_dependency( RAPIDJSON REQUIRED
  INTERNAL_SOURCE_DIRS external/rapidjson
  INTERNAL_LIB_TARGETS rapidjson )

find_dependency( RESTCLIENT REQUIRED
  INTERNAL_SOURCE_DIRS external/restclient
  INTERNAL_LIB_TARGETS restclient )

find_dependency( NETTLE REQUIRED
  INTERNAL_SOURCE_DIRS external/nettle-3.3
  INTERNAL_LIB_TARGETS nettle )

if( BUILD_CLIENT )
  find_dependency( SEMVER REQUIRED
    INTERNAL_SOURCE_DIRS external/semver
    INTERNAL_LIB_TARGETS semver )

  find_dependency( SDL2 REQUIRED
    PKG_CONFIG sdl2 )

  find_package( OpenGL REQUIRED )

  find_dependency( OPENAL REQUIRED
    PKG_CONFIG openal )

  find_dependency( LIBJPEG REQUIRED
    PKG_CONFIG libjpeg
    INTERNAL_SOURCE_DIRS external/jpeg-8c
    INTERNAL_LIB_TARGETS jpeg-8c )

  find_dependency( VORBISFILE REQUIRED
    PKG_CONFIG vorbisfile
    INTERNAL_SOURCE_DIRS external/libogg-1.3.2 external/libvorbis-1.3.5
    INTERNAL_LIB_TARGETS vorbis )
endif( )


#===============================================================================
# Collect the source lists
#===============================================================================

include( src/asm/Files.cmake )
include( src/qcommon/Files.cmake )
include( src/sys/Files.cmake )
include( src/sdl/Files.cmake )
include( src/null/Files.cmake )
include( src/server/Files.cmake )
include( src/game/Files.cmake )
include( src/ui/Files.cmake )


#===============================================================================
# Build Tremulous
#===============================================================================

if( BUILD_CLIENT OR BUILD_SERVER )
  add_subdirectory( src/script )
  set( COMMON_LINK_LIBRARIES
    ${CMAKE_DL_LIBS}
    ${LUA_LIB_TARGETS}
    ${SOL_LIB_TARGETS}
    ${RESTCLIENT_LIB_TARGETS}
    ${NETTLE_LIB_TARGETS}
    ${ZLIB_LIB_TARGETS}
    script_api )
endif( )

if( BUILD_CLIENT )
  add_subdirectory( src/client )
  add_subdirectory( src/renderercommon )
  add_subdirectory( src/renderergl1 )
  add_subdirectory( src/renderergl2 )
endif( )

if( BUILD_SERVER )
  add_subdirectory( src/server )
endif( )

if( BUILD_GAME )
  add_subdirectory( src/tools )
  add_subdirectory( src/game )
  add_subdirectory( src/cgame )
  add_subdirectory( src/ui )
endif( )

add_subdirectory( data )
add_subdirectory( docs )

install( FILES COPYING DESTINATION "${DOC_INSTALL_DIR}" )
