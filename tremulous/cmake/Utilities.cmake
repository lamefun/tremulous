# Turns all paths in the list into absolute paths, relative to the current
# source directory or the given root.
function( expand_paths var )
  cmake_parse_arguments( "ARG" "" "ROOT" "" ${ARGN} )
  if( NOT ARG_ROOT )
    set( ARG_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" )
  endif( )
  set( result )
  foreach( path IN LISTS ARG_UNPARSED_ARGUMENTS )
    if( IS_ABSOLUTE path )
      list( APPEND result "${path}" )
    else( )
      list( APPEND result "${ARG_ROOT}/${path}" )
    endif( )
  endforeach( )
  message( "Added: ${result}" )
  set( ${var} ${result} PARENT_SCOPE )
endfunction( )

# Same as expand_paths, but appends the paths to the variable instead of
# replacing the original content.
function( expand_paths_append var )
  expand_paths( items ${ARGN} )
  list( APPEND ${var} ${items} )
  set( ${var} ${${var}} PARENT_SCOPE )
endfunction( )

# Makes a version string from individual components.
function( make_version_string var )
  set( count ${${var}_VERSION_COUNT} )
  set( result "${${var}_VERSION_MAJOR}" )
  if( count GREATER_EQUAL 2 )
    string( APPEND result ".${${var}_VERSION_MINOR}" )
  endif( )
  if( count GREATER_EQUAL 3 )
    string( APPEND result ".${${var}_VERSION_PATCH}" )
  endif( )
  if( count GREATER_EQUAL 4 )
    string( APPEND result ".${${var}_VERSION_TWEAK}" )
  endif( )
  string( APPEND string "${${var}_VERSION_EXTRA}" )
  set( ${var}_VERSION_STRING "${result}" PARENT_SCOPE)
endfunction( )