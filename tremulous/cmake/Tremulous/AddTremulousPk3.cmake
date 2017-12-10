#===============================================================================
#
# Function for building data packages.
#
# See docs/CMake/Package.md for documentation.
#
#===============================================================================

#-------------------------------------------------------------------------------
# add_tremulous_pk3 - creates a data package
#-------------------------------------------------------------------------------

function( add_tremulous_pk3 target )

  if( NOT Tremulous_PK3_FOUND )
    message( FATAL_ERROR "Tremulous packaging tools were not found" )
  endif( )

  set( added_files FALSE )
  set( keywords FROM FILES FILE )
  set( output )
  set( output_specified FALSE )
  set( from "${CMAKE_CURRENT_SOURCE_DIR}" )
  set( from_specified FALSE )
  set( files )
  set( custom_file_paths )
  set( custom_file_names )

  list( LENGTH ARGN num_args )
  set( i 0 )
  while( i LESS num_args )
    list( GET ARGN ${i} option )
    if( option STREQUAL OUTPUT )
      math( EXPR output_i "${i} + 1" )
      if( NOT output_i LESS num_args )
        message( FATAL_ERROR "OUTPUT is missing an argument" )
      endif( )
      if( output_specified )
        message( FATAL_ERROR "Duplicate OUTPUT option" )
      endif( )
      list( GET ARGN ${output_i} output )
      set( output_specified TRUE )
      math( EXPR i "${output_i} + 1" )
    elseif( option STREQUAL FROM )
      math( EXPR path_i "${i} + 1" )
      if( NOT path_i LESS num_args )
        message( FATAL_ERROR "FROM is missing an argument" )
      endif( )
      if( from_specified )
        message( FATAL_ERROR "Duplicate FROM option" )
      endif( )
      list( GET ARGN ${path_i} path )
      if( IS_ABSOLUTE path )
        set( from "${path}" )
      else( )
        set( from "${CMAKE_CURRENT_SOURCE_DIR}/${path}" )
      endif( )
      set( from_specified TRUE )
      math( EXPR i "${path_i} + 1" )
    elseif( option STREQUAL FILES )
      math( EXPR i "${i} + 1" )
      while( i LESS num_args )
        list( GET ARGN ${i} path )
        if( IS_ABSOLUTE "${path}" )
          message( FATAL_ERROR "FILE does not accept absolute paths"
                               "(path: ${path})" )
        elseif( path IN_LIST keywords )
          break( )
        else( )
          list( APPEND files "${path}" )
          math( EXPR i "${i} + 1" )
        endif( )
      endwhile( )
    elseif( option STREQUAL FILE )
      math( EXPR path_i "${i} + 1" )
      math( EXPR keyword_i "${i} + 2" )
      math( EXPR arg_i "${i} + 3" )
      if( NOT arg_i LESS num_args )
        message( FATAL_ERROR "FILE requires 3 arguments" )
      endif( )
      list( GET ARGN ${path_i} path )
      list( GET ARGN ${keyword_i} keyword )
      list( GET ARGN ${arg_i} arg )
      list( APPEND custom_file_paths "${path}" )
      if( keyword STREQUAL INTO )
        get_filename_component( name "${path}" NAME )
        list( APPEND custom_file_names "${arg}/${name}" )
      elseif( keyword STREQUAL AS )
        list( APPEND custom_file_names "${arg}" )
      else( )
        message( FATAL_ERROR "FILE is missing an INTO or AS argument"
                             "(path: ${path})" )
      endif( )
      math( EXPR i "${arg_i} + 1" )
    else( )
      message( FATAL_ERROR "Unknown option ${option}" )
    endif( )
  endwhile( )

  if( NOT output_specified )
    message( FATAL_ERROR "OUTPUT option was not specified" )
  endif( )

  set( ziptool_commands )
  set( depends )

  foreach( file IN LISTS files )
    set( path "${from}/${file}" )
    list( APPEND depends "${path}" )
    list( APPEND ziptool_commands add_file "${file}" "${path}" 0 0 )
  endforeach( )

  set( i 0 )
  list( LENGTH custom_file_paths num_custom_files )
  while( i LESS num_custom_files )
    list( GET custom_file_paths ${i} path )
    list( GET custom_file_names ${i} name )
    if( NOT IS_ABSOLUTE "${path}" )
      set( path "${from}/${path}" )
    endif( )
    list( APPEND depends "${path}" )
    list( APPEND ziptool_commands add_file "${name}" "${path}" 0 0 )
    math( EXPR i "${i} + 1" )
  endwhile( )

  add_custom_command(
    OUTPUT "${output}"
    DEPENDS ${depends}
    COMMENT "Creating package ${output}"
    COMMAND "${Tremulous_ZIPTOOL}" -nt "${output}" ${ziptool_commands} )

  add_custom_target( ${target} ALL DEPENDS "${output}" )

endfunction( )

#-------------------------------------------------------------------------------
