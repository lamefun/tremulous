# Utility function for acquiring dependencies.
#
# Usage: acquire_dependency( NAME [REQUIRED] ... )
#
# Defines variables:
#
#   DEPS_${UPPERCASE_NAME}_FOUND
#     Whether to dependency was found.
#
# Arguments:
#
#   PKG_CONFIG
#     pkg-config package to search (eg. "lua >= 5.2")
#
#   FIND_PACKAGE
#   FIND_PACKAGE_IMPORTED_TARGETS
#   FIND_PACKAGE_LIBRARY_VARS
#   FIND_PACKAGE_INCLUDE_DIR_VARS
#   FIND_PACKAGE_LIBRARIES_VARS
#   FIND_PACKAGE_INCLUDE_DIRS_VARS
#     find_package package to search for and the variables into which the find
#     module puts the results into. Make sure to use the arguments whose names end
#     with an 'S' (for example, FIND_PACKAGE_LIBRARIES_VARS) for find output
#     variables whose names end with an 'S' as well (for example, SDL2_LIBRARIES).
#
#   INTERNAL_SOURCE_DIRS
#     Internal source directories, included using add_subdirectory( ).
#
#   INTERNAL_SOURCE_LIBRARIES
#     Library targets built from internal sources.
#
#   INTERNAL_WIN32_INCLUDE_DIRS
#   INTERNAL_WIN32_LIBRARIES
#   INTERNAL_WIN64_INCLUDE_DIRS
#   INTERNAL_WIN64_LIBRARIES
#     Pre-built libraries and include directories for them.

include( CMakeParseArguments )
find_package( PkgConfig )

function( acquire_dependency NAME )
  set( FLAGS REQUIRED )
  set( SINGLE_PARAM_ARGS FIND_PACKAGE )
  set( MULTI_PARAM_ARGS
    PKG_CONFIG
    FIND_PACKAGE_IMPORTED_TARGETS
    FIND_PACKAGE_LIBRARY_VARS
    FIND_PACKAGE_INCLUDE_DIR_VARS
    FIND_PACKAGE_LIBRARIES_VARS
    FIND_PACKAGE_INCLUDE_DIRS_VARS
    INTERNAL_SOURCE_DIRS
    INTERNAL_SOURCE_LIBRARIES
    INTERNAL_WIN32_INCLUDE_DIRS
    INTERNAL_WIN32_LIBRARIES
    INTERNAL_WIN64_INCLUDE_DIRS
    INTERNAL_WIN64_LIBRARIES )

  # Parse the arguments.
  cmake_parse_arguments( ARG "${FLAGS}" "${SINGLE_PARAM_ARGS}"
                         "${MULTI_PARAM_ARGS}" ${ARGN} )

  set( FOUND FALSE )  # Whether the dependency was found.
  set( INCLUDE_DIRS ) # List of include directories.
  set( LIBRARIES )    # List of libraries to link with.

  # Check what type of a bundled dependency to use.
  set( INTERNAL_TYPE None )
  if( WIN32 AND "${CMAKE_SIZEOF_VOID_P}" EQUAL 4 AND
      NOT "${ARG_INTERNAL_WIN32_LIBRARIES}" STREQUAL "" )
    set( INTERNAL_TYPE Win32 )
  elseif( WIN32 AND "${CMAKE_SIZEOF_VOID_P}" EQUAL 8 AND
          NOT "${ARG_INTERNAL_WIN64_LIBRARIES}" STREQUAL "" )
    set( INTERNAL_TYPE Win64 )
  elseif( NOT "${ARG_INTERNAL_SOURCE_DIRS}" STREQUAL "" )
    set( INTERNAL_TYPE Source )
  endif( )

  # Check if we have information to find the dependency.
  set( HAVE_EXTERNAL FALSE )
  if( NOT "${ARG_PKG_CONFIG}" STREQUAL "" OR
      NOT "${ARG_FIND_PACKAGE}" STREQUAL "" )
    set( HAVE_EXTERNAL TRUE )
  endif( )

  # Check if we want to ignore internal dependencies.
  if( "${INTERNAL_DEPS}" STREQUAL Never AND HAVE_EXTERNAL )
    set( INTERNAL_TYPE None )
  endif( )

  # Check if we want to ignore external dependencies.
  set( IGNORE_EXTERNAL FALSE )
  if( "${INTERNAL_DEPS}" STREQUAL Always AND HAVE_INTERNAL )
    set( IGNORE_EXTERNAL TRUE )
  endif( )

  # Try using pkg-config.
  if( NOT FOUND AND NOT "${ARG_PKG_CONFIG}" STREQUAL "" AND
      PKG_CONFIG_FOUND AND NOT IGNORE_EXTERNAL )
    pkg_check_modules( DEPS_${NAME} IMPORTED_TARGET ${ARG_PKG_CONFIG} )
    if( DEPS_${NAME}_FOUND )
      set( LIBRARIES "PkgConfig::DEPS_${NAME}" )
      set( FOUND TRUE )
    endif( )
  endif( )

  # Try using find_package.
  if( NOT FOUND AND NOT "${ARG_FIND_PACKAGE}" STREQUAL "" AND
      NOT IGNORE_EXTERNAL )
    find_package( ${ARG_FIND_PACKAGE} )
    if( ${ARG_FIND_PACKAGE}_FOUND )
      # Add imported targets.
      list( APPEND LIBRARIES ${ARG_FIND_PACKAGE_IMPORTED_TARGETS} )
      # Add non-list variables.
      foreach( LIB IN LISTS ${ARG_FIND_PACKAGE_LIBRARY_VARS} )
        list( APPEND LIBRARIES "${${LIB}}" )
      endforeach( )
      foreach( INC IN LISTS ${ARG_FIND_PACKAGE_INCLUDE_DIR_VARS} )
        list( APPEND INCLUDE_DIRS "${${INC}}" )
      endforeach( )
      # Add list variables.
      foreach( LIB IN LISTS ${ARG_FIND_PACKAGE_LIBRARIES_VARS} )
        list( APPEND LIBRARIES ${${LIB}} )
      endforeach( )
      foreach( INC IN LISTS ${ARG_FIND_PACKAGE_INCLUDE_DIRS_VARS} )
        list( APPEND INCLUDE_DIRS ${${INC}} )
      endforeach( )
      set( FOUND TRUE )
    endif( )
  endif( )

  # Try using pre-built Win32 libraries.
  if( NOT FOUND AND "${INTERNAL_TYPE}" STREQUAL Win32 )
    message( "Using pre-built Win32 libraries for ${NAME}: \
${ARG_INTERNAL_WIN32_LIBRARIES}" )
    set( INCLUDE_DIRS ${ARG_INTERNAL_WIN32_INCLUDE_DIRS} )
    set( LIBRARIES ${ARG_INTERNAL_WIN32_LIBRARIES} )
    set( FOUND TRUE )
  endif( )

  # Try using pre-built Win64 libraries.
  if( NOT FOUND AND "${INTERNAL_TYPE}" STREQUAL Win64 )
    message( "Using pre-built Win64 libraries for ${NAME}: \
${ARG_INTERNAL_WIN64_LIBRARIES}" )
    set( INCLUDE_DIRS ${ARG_INTERNAL_WIN64_INCLUDE_DIRS} )
    set( LIBRARIES ${ARG_INTERNAL_WIN64_LIBRARIES} )
    set( FOUND TRUE )
  endif( )

  # Try using internal sources.
  if( NOT FOUND AND "${INTERNAL_TYPE}" STREQUAL Source )
    message( "Using internal sources for ${NAME}: ${ARG_INTERNAL_SOURCE_DIRS}" )
    foreach( DIR IN LISTS ARG_INTERNAL_SOURCE_DIRS )
      add_subdirectory( "${DIR}" )
    endforeach( )
    set( LIBRARIES ${ARG_INTERNAL_SOURCE_LIBRARIES} )
    set( FOUND TRUE )
  endif( )

  # Check if a required dependency is missing.
  if( ARG_REQUIRED AND NOT FOUND )
    message( FATAL_ERROR "Required dependency ${NAME} was not found" )
  endif( )

  # Export the dependency.
  string( TOUPPER "${NAME}" UPPERCASE_NAME )
  set( DEPS_${UPPERCASE_NAME}_FOUND ${FOUND} PARENT_SCOPE )
  add_library( DEPS::${NAME} INTERFACE IMPORTED )
  if( NOT "${INCLUDE_DIRS}" STREQUAL "" )
    set_target_properties( DEPS::${NAME} PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES ${INCLUDE_DIRS} )
  endif( )
  if( NOT "${LIBRARIES}" STREQUAL "" )
    set_target_properties( DEPS::${NAME} PROPERTIES
      INTERFACE_LINK_LIBRARIES ${LIBRARIES} )
  endif( )
endfunction( )
