# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindOpenAL
# ----------
#
# Locates the OpenAL library.
#
# ::
#
#   OpenAL::AL - library to link with.
#   OpenAL_FOUND - TRUE if OpenAL was found.
#   OpenAL_LIBRARY - full path to the AL library.
#   OpenAL_INCLUDE_DIR - where to find OpenAL headers.

set( _openal_install_dir_key [HKEY_LOCAL_MACHINE\\SOFTWARE\\Creative\ Labs\\OpenAL\ 1.1\ Software\ Development\ Kit\\1.00.0000;InstallDir] )

find_path( OpenAL_INCLUDE_DIR al.h
  HINTS ENV OPENALDIR
  PATH_SUFFIXES include/AL include/OpenAL include
  PATHS ${_openal_install_dir_key} )

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set( _OpenAL_ARCH_DIR libs/Win64 )
else( )
  set( _OpenAL_ARCH_DIR libs/Win32 )
endif( )

find_library( OpenAL_LIBRARY
  NAMES OpenAL al openal OpenAL32
  HINTS ENV OPENALDIR
  PATH_SUFFIXES libx32 lib64 lib libs64 libs ${_OpenAL_ARCH_DIR}
  PATHS ${_OpenAL_InstallDirKey} )

unset( _OpenAL_ARCH_DIR )
mark_as_advanced( OpenAL_LIBRARY OpenAL_INCLUDE_DIR )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( OpenAL
  REQUIRED_ARGS OpenAL_LIBRARY OpenAL_INCLUDE_DIR)

if( OpenAL_FOUND )
  add_library( OpenAL::AL UNKNOWN IMPORTED )
  set_target_properties( OpenAL::AL PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${OpenAL_INCLUDE_DIR}"
    IMPORTED_LOCATION "${OpenAL_LIBRARY}" )
endif( )
