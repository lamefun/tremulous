# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindVorbis
# -------
#
# Locates the Vorbis libraries.
#
# ::
#
#   Vorbis_FOUND - TRUE if vorbisfile was found.
#   Vorbis_INCLUDE_DIR - Where to find Vorbis headers.
#
# Libraries:
#
# ::
#
#   Vorbis::vorbis - vorbis library.
#   Vorbis::vorbisenc - vorbisenc library.
#   Vorbis::vorbisfile - vorbisfile library.
#
# Library locations:
#
# ::
#
#   Vorbis_LIBRARY - full path to vorbis.
#   VorbisEnc_LIBRARY - full path to vorbisenc.
#   VorbisFile_LIBRARY - full path to vorbisfile.

find_package( PkgConfig )
pkg_check_modules( PC_VORBIS QUIET vorbis vorbisenc vorbisfile )

find_path( Vorbis_INCLUDE_DIR vorbis/codec.h )
find_library( Vorbis_LIBRARY NAMES vorbis HINTS ${PC_VORBIS_LIBRARY_DIRS} )
find_library( VorbisEnc_LIBRARY NAMES vorbisenc HINTS ${PC_VORBIS_LIBRARY_DIRS} )
find_library( VorbisFile_LIBRARY NAMES vorbisfile HINTS ${PC_VORBIS_LIBRARY_DIRS} )

set( _vorbis_vars Vorbis_LIBRARY VorbisEnc_LIBRARY VorbisFile_LIBRARY Vorbis_INCLUDE_DIR )

mark_as_advanced( ${_vorbis_vars} )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Vorbis REQUIRED_ARGS ${_vorbis_vars} )

if( Vorbis_FOUND )
  add_library( Vorbis::vorbis UNKNOWN IMPORTED )
  set_target_properties( Vorbis::vorbis PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIR}"
    IMPORTED_LOCATION "${Vorbis_LIBRARY}" )

  add_library( Vorbis::vorbisenc UNKNOWN IMPORTED )
  set_target_properties( Vorbis::vorbisenc PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIR}"
    IMPORTED_LOCATION "${VorbisEnc_LIBRARY}"
    IMPORTED_INTERFACE_LINK_LIBRARIES "${Vorbis_LIBRARY}" )

  add_library( Vorbis::vorbisfile UNKNOWN IMPORTED )
  set_target_properties( Vorbis::vorbisfile PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIR}"
    IMPORTED_LOCATION "${VorbisFile_LIBRARY}"
    IMPORTED_INTERFACE_LINK_LIBRARIES "${VorbisEnc_LIBRARY}" )
endif( )
