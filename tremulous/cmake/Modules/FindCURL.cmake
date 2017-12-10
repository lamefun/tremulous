# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindCURL
# --------
#
# Find the native CURL headers and libraries.
#
# ::
#
#   CURL::curl          - library to link with.
#   CURL_INCLUDE_DIR    - where to find curl/curl.h, etc.
#   CURL_LIBRARY        - CURL library.
#   CURL_FOUND          - true if CURL was found.
#   CURL_VERSION_STRING - the version of CURL.

find_path( CURL_INCLUDE_DIR NAMES curl/curl.h )
find_library( CURL_LIBRARY NAMES
  curl curllib libcurl_imp curllib_static libcurl )

function( _curl_get_version )
  set( regex "^#define[\t ]+LIBCURL_VERSION[\t ]+\"([^\"]*)\".*" )
  foreach( header_name curlver.h curl.h )
    set( header "${CURL_INCLUDE_DIR}/curl/${header_name}" )
    if( EXISTS "${header}" )
      file( STRINGS "${header}" version REGEX "${regex}" )
      string( REGEX REPLACE "${regex}" "\\1" version "${version}" )
      set( CURL_VERSION_STRING "${version}" PARENT_SCOPE )
      break( )
    endif( )
  endforeach( )
endfunction( )

_curl_get_version( )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( CURL
  REQUIRED_VARS CURL_LIBRARY CURL_INCLUDE_DIR
  VERSION_VAR CURL_VERSION_STRING )

if( CURL_FOUND )
  add_library( CURL::curl UNKNOWN IMPORTED )
  set_target_properties( CURL::curl PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${CURL_INCLUDE_DIR}"
    IMPORTED_LOCATION "${CURL_LIBRARY}" )
endif()

mark_as_advanced( CURL_INCLUDE_DIR CURL_LIBRARY )
