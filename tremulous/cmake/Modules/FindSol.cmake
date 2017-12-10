# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindSol
# ----------
#
# Locates the Sol library.
#
# ::
#
#   Sol::sol - library to link with.
#   Sol_FOUND - TRUE if Ogg was found.
#   Sol_LIBRARY - full path to ogg library.
#   Sol_INCLUDE_DIR - where to find Ogg headers.

find_path( Sol_INCLUDE_DIR sol.hpp PATH_SUFFIXES sol )
mark_as_advanced( Sol_INCLUDE_DIR )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Sol REQUIRED_VARS Sol_INCLUDE_DIR )

if( Sol_FOUND )
  add_library( Sol::sol INTERFACE IMPORTED )
  set_target_properties( Sol::sol PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${Sol_INCLUDE_DIR}" )
endif( )
