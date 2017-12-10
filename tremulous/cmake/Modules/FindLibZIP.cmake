# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindLibZIP
# -------
#
# Locates the LibZIP library.
#
# ::
#
#   LibZIP::zip - library to link with.
#   LibZIP_FOUND - TRUE if LibZIP was found.
#   LibZIP_LIBRARY - full path to LibZIP library.
#   LibZIP_INCLUDE_DIR - where to find LibZIP headers.

find_package( PkgConfig )
pkg_check_modules( PC_LIBZIP QUIET libzip )

find_path( LibZIP_INCLUDE_DIR zip.h
  HINTS ${PC_LIBZIP_INCLUDE_DIRS} )
find_library( LibZIP_LIBRARY
  NAMES zip ${PC_LIBZIP_LIBRARIES}
  HINTS ${PC_LIBZIP_LIBRARY_DIRS} )
mark_as_advanced( LibZIP_LIBRARY LibZIP_INCLUDE_DIR )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( LibZIP
  REQUIRED_ARGS LibZIP_LIBRARY LibZIP_INCLUDE_DIR
  HANDLE_COMPONENTS )

if( LibZIP_FOUND )
  add_library( LibZIP::zip UNKNOWN IMPORTED )
  set_target_properties( LibZIP::zip PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${LibZIP_INCLUDE_DIR}"
    IMPORTED_LOCATION "${LibZIP_LIBRARY}" )
endif( )
