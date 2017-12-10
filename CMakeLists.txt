#===============================================================================
#
# This is the Tremulous superbuild, which builds, installs and packages the
# Tremulous engine, the basics assets, the standard game modifications, and all
# the dependencies necessary for Tremulous to function.
#
# Note that the Tremulous components are by themselves complete CMake projects
# and can be built independently, if provided with the necessary dependencies.
# The superbuild only exists for development and packaging convenience.
#
#===============================================================================

# Define the project.
cmake_minimum_required( VERSION 3.6 )
project( tremulous-superbuild C CXX )

# Include the ExternalProject module.
include( ExternalProject )

# Define the build environment. See the include itself for the information about
# the build environment variables.
include( Environment.cmake )

#-------------------------------------------------------------------------------
# Superbuild Options
#-------------------------------------------------------------------------------

# Include the engine options.
include( tremulous/cmake/Options.cmake )

# Superbuild options.
option( INSTALL_BASE_DATA "Whether to install the base assets" ON )
option( BUILD_GAME_GPP "Build the GPP game modification" ON )
option( BUILD_GAME_SWIRL "Build the Swirl game modification" ON )

#-------------------------------------------------------------------------------
# Third-Party Dependencies
#-------------------------------------------------------------------------------

add_subdirectory( external/zlib )
add_subdirectory( external/libzip )
add_subdirectory( external/lua )
add_subdirectory( external/sol )
add_subdirectory( external/rapidjson )

if( BUILD_CLIENT )
  add_subdirectory( external/libogg )
  add_subdirectory( external/libvorbis )
  add_subdirectory( external/libjpeg-turbo )
endif( )

#-------------------------------------------------------------------------------
# Tremulous Components
#-------------------------------------------------------------------------------

# Pass the engine options to the engine build system.
set( ENGINE_CMAKE_ARGS ${TREMULOUS_CMAKE_ARGS} )
foreach( option IN LISTS ENGINE_OPTIONS )
  list( APPEND ENGINE_CMAKE_ARGS "-D${option}=${${option}}" )
endforeach( )

# Engine dependencies.
set( ENGINE_DEPENDS zlib lua sol rapidjson )
if( BUILD_CLIENT )
  list( APPEND ENGINE_DEPENDS libogg libvorbis libjpeg-turbo )
endif( )

# Tremulous engine.
ExternalProject_Add( tremulous
  DEPENDS ${ENGINE_DEPENDS}
  BUILD_ALWAYS TRUE
  PREFIX "${BUILD_PREFIX}"
  SOURCE_DIR "${PROJECT_SOURCE_DIR}/tremulous"
  BINARY_DIR "${PROJECT_BINARY_DIR}/tremulous"
  STAMP_DIR "${PROJECT_BINARY_DIR}/tremulous/stamp"
  INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
  CMAKE_ARGS ${ENGINE_CMAKE_ARGS}
  CMAKE_CACHE_ARGS ${TREMULOUS_CMAKE_CACHE_ARGS} )

# Essential assets.
ExternalProject_Add( tremulous-game-base
  DEPENDS tremulous
  BUILD_ALWAYS TRUE
  PREFIX "${BUILD_PREFIX}"
  SOURCE_DIR "${PROJECT_SOURCE_DIR}/tremulous-game-base"
  BINARY_DIR "${PROJECT_BINARY_DIR}/tremulous-game-base"
  STAMP_DIR "${PROJECT_BINARY_DIR}/tremulous-game-base/stamp"
  INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
  CMAKE_ARGS ${TREMULOUS_CMAKE_ARGS}
  CMAKE_CACHE_ARGS ${TREMULOUS_CMAKE_CACHE_ARGS} )

# GPP game mod.
ExternalProject_Add( tremulous-game-gpp
  DEPENDS tremulous-game-base
  BUILD_ALWAYS TRUE
  PREFIX "${BUILD_PREFIX}"
  SOURCE_DIR "${PROJECT_SOURCE_DIR}/tremulous-game-gpp"
  BINARY_DIR "${PROJECT_BINARY_DIR}/tremulous-game-gpp"
  STAMP_DIR "${PROJECT_BINARY_DIR}/tremulous-game-gpp/stamp"
  INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
  CMAKE_ARGS ${TREMULOUS_CMAKE_ARGS}
  CMAKE_CACHE_ARGS ${TREMULOUS_CMAKE_CACHE_ARGS} )

#-------------------------------------------------------------------------------
