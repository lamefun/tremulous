# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindJPEG
# -------
#
# Locates the JPEG library.
#
# ::
#
#   JPEG::jpeg - library to link with.
#   JPEG_FOUND - TRUE if JPEG was found.
#   JPEG_LIBRARY - full path to JPEG library.
#   JPEG_INCLUDE_DIR - where to find JPEG headers.

find_package( PkgConfig )
pkg_check_modules( PC_JPEG QUIET libjpeg )

find_path( JPEG_INCLUDE_DIR jpeglib.h
  HINTS ${PC_JPEG_INCLUDE_DIRS} )
find_library( JPEG_LIBRARY
  NAMES jpeg libjpeg ${PC_JPEG_LIBRARIES}
  HINTS ${PC_JPEG_LIBRARY_DIRS} )
mark_as_advanced( JPEG_LIBRARY JPEG_INCLUDE_DIR )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( JPEG
  REQUIRED_ARGS JPEG_LIBRARY JPEG_INCLUDE_DIR )

if( JPEG_FOUND )
  add_library( JPEG::jpeg UNKNOWN IMPORTED )
  set_target_properties( JPEG::jpeg PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${JPEG_INCLUDE_DIR}"
    IMPORTED_LOCATION "${JPEG_LIBRARY}" )
endif( )
