#===============================================================================
#
# Defines the Tremulous superbuild environment.
#
# Directories:
#
#   BUILD_PREFIX - prefix for building and installing external projects.
#
# Commands:
#
#   BUILD_MAKE - the 'make' command (with the -jN argument).
#
# Common build system arguments:
#
#   BUILD_ENV - the 'cmake -E env' command to set environment variables.
#
# Build system arguments for external projects:
#
#   EXTERNAL_CONFIGURE_ARGS - GNU Autotools configure arguments.
#   EXTERNAL_CMAKE_CACHE_ARGS - CMake cache arguments.
#   EXTERNAL_CMAKE_ARGS - CMake arguments.
#
# Build system arguments for Tremulous projects:
#
#   TREMULOUS_CMAKE_CACHE_ARGS - CMake cache arguments for Tremulous projects.
#   TREMULOUS_CMAKE_ARGS - CMake arguments for Tremulous projects.
#
#===============================================================================

#-------------------------------------------------------------------------------
# Basics
#-------------------------------------------------------------------------------

# Prefix for building and installing external projects.
set( BUILD_PREFIX "${PROJECT_BINARY_DIR}/external/install" )

# Get the number of CPUs.
include( ProcessorCount )
ProcessorCount( NCPUS )

#-------------------------------------------------------------------------------
# Environment
#-------------------------------------------------------------------------------

set( BUILD_ENV ${CMAKE_COMMAND} -E env
  "CC=${CMAKE_C_COMPILER}"
  "CXX=${CMAKE_CXX_COMPILER}"
  "PKG_CONFIG_PATH=${CMAKE_INSTALL_PREFIX}:${BUILD_PREFIX}:$ENV{PKG_CONFIG_PATH}"
  "LD_LIBRARY_PATH=${BUILD_PREFIX}/lib:$ENV{LD_LIBRARY_PATH}" )

#-------------------------------------------------------------------------------
# Commands
#-------------------------------------------------------------------------------

# Make command.
set( BUILD_MAKE "${CMAKE_MAKE_PROGRAM}" )
if( NCPUS GREATER 0 )
  list( APPEND BUILD_MAKE -j${NCPUS} )
endif( )

#-------------------------------------------------------------------------------
# GNU Autotools
#-------------------------------------------------------------------------------

# ./configure arguments
set( EXTERNAL_CONFIGURE_ARGS
  "--prefix=${BUILD_PREFIX}"
  "--enable-shared="
  "--with-pic" )

#-------------------------------------------------------------------------------
# CMake
#-------------------------------------------------------------------------------

set( COMMON_CMAKE_ARGS
  "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
  "-DCMAKE_CONFIGURATIONS=${CMAKE_CONFIGURATIONS}" )

set( EXTERNAL_CMAKE_ARGS
  ${COMMON_CMAKE_ARGS}
  "-DCMAKE_INSTALL_PREFIX=${BUILD_PREFIX}" )

set( TREMULOUS_CMAKE_ARGS
  ${COMMON_CMAKE_ARGS}
  "-DCMAKE_PREFIX_PATH=${BUILD_PREFIX}"
  "-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}" )

set( EXTERNAL_CMAKE_CACHE_ARGS
  "-DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}"
  "-DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}" )

set( TREMULOUS_CMAKE_CACHE_ARGS ${EXTERNAL_CMAKE_CACHE_ARGS} )

#-------------------------------------------------------------------------------
