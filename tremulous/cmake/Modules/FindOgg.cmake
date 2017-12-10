# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindOgg
# -------
#
# Locates the Ogg library.
#
# ::
#
#   Ogg::ogg - library to link with.
#   Ogg_FOUND - TRUE if Ogg was found.
#   Ogg_LIBRARY - full path to the ogg library.
#   Ogg_INCLUDE_DIR - where to find Ogg headers.

find_package( PkgConfig )
pkg_check_modules( PC_OGG QUIET ogg )

find_path( Ogg_INCLUDE_DIR ogg/ogg.h
  HINTS ${PC_OGG_INCLUDE_DIRS} )
find_library( Ogg_LIBRARY
  NAMES ogg ${PC_OGG_LIBRARIES}
  HINTS ${PC_OGG_LIBRARY_DIRS} )
mark_as_advanced( Ogg_LIBRARY Ogg_INCLUDE_DIR )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Ogg
  REQUIRED_ARGS Ogg_LIBRARY Ogg_INCLUDE_DIR )

if( Ogg_FOUND )
  add_library( Ogg::ogg UNKNOWN IMPORTED )
  set_target_properties( Ogg::ogg PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${Ogg_INCLUDE_DIR}"
    IMPORTED_LOCATION "${Ogg_LIBRARY}" )
endif( )
