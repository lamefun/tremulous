include( CMakeParseArguments )
include( FindPkgConfig )


# list_append_prefixed( LIST_VAR PREFIX prefix item1 item2 ... )
function( list_append_prefixed LIST )
  cmake_parse_arguments( ARG "" PREFIX "" ${ARGN} )
  set( RESULT ${${LIST}} )
  foreach( ITEM IN LISTS ARG_UNPARSED_ARGUMENTS )
    list( APPEND RESULT "${ARG_PREFIX}${ITEM}" )
  endforeach( )
  set( ${LIST} ${RESULT} PARENT_SCOPE )
endfunction( )


# Utility function for finding dependencies.
function( find_dependency NAME )
  set( ${NAME}_FOUND FALSE )
  set( LIB_TARGETS )

  # Parse the arguments.
  set( FLAGS REQUIRED )
  set( ARGS_1 INTERNAL_SOURCE_DIRS )
  set( ARGS_N INTERNAL_LIB_TARGETS PKG_CONFIG )
  cmake_parse_arguments( ARG "${FLAGS}" "${ARGS_1}" "${ARGS_N}" ${ARGN} )

  set( MAYBE_REQUIRED )
  if( ARG_REQUIRED )
    set( MAYBE_REQUIRED REQUIRED )
  endif( )

  set( USE_INTERNAL_DEPENDENCY ${INTERNAL_DEPENDENCIES} )
  if( "${ARG_INTERNAL_SOURCE_DIRS}" STREQUAL "" )
    set( USE_INTERNAL_DEPENDENCY FALSE )
  endif( )

  # Try pkg-config.
  if( NOT USE_INTERNAL_DEPENDENCY AND NOT ${NAME}_FOUND AND
      NOT "${ARG_PKG_CONFIG}" STREQUAL "" )
    pkg_check_modules( ${NAME}
      ${MAYBE_REQUIRED} IMPORTED_TARGET
      ${ARG_PKG_CONFIG} )
    if( ${NAME}_FOUND )
      set( LIB_TARGETS PkgConfig::${NAME} )
    endif( )
  endif( )

  # Try internal sources.
  if( NOT ${NAME}_FOUND AND NOT "${ARG_INTERNAL_SOURCE_DIRS}" STREQUAL "" )
    message( "Using internal sources for ${NAME}: ${ARG_INTERNAL}" )
    foreach( DIR IN LISTS ARG_INTERNAL_SOURCE_DIRS )
      add_subdirectory( "${DIR}" )
    endforeach( )
    set( LIB_TARGETS ${ARG_INTERNAL_LIB_TARGETS} )
    set( ${NAME}_FOUND TRUE )
  endif( )

  if( ARG_REQUIRED AND NOT ${NAME}_FOUND )
    message( FATAL_ERROR "Required dependency ${NAME} not found" )
  endif( )

  set( ${NAME}_LIB_TARGETS ${LIB_TARGETS} PARENT_SCOPE )
  set( ${NAME}_FOUND ${${NAME}_FOUND} PARENT_SCOPE )
endfunction( )


# Utility function for building QVMs.
function( build_qvm TARGET )
  set( ARGS_1 OUTPUT NATIVE )
  set( ARGS_N SOURCES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS )
  cmake_parse_arguments( ARG "" "${ARGS_1}" "${ARGS_N}" ${ARGN} )

  set( INCLUDE_DIRECTORIES )
  foreach( INC IN LISTS ARG_INCLUDE_DIRECTORIES)
    list( APPEND INCLUDE_DIRECTORIES "-I${INC}" )
  endforeach( )

  set( COMPILE_DEFINITIONS )
  foreach( DEF IN LISTS ARG_COMPILE_DEFINITIONS )
    list( APPEND COMPILE_DEFINITIONS "-D${DEF}" )
  endforeach( )

  set( ASMS )
  foreach( SOURCE IN LISTS ARG_SOURCES )
    if( NOT IS_ABSOLUTE "${SOURCE}" )
      set( SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE}" )
    endif( )

    get_filename_component( EXT "${SOURCE}" EXT )
    if( "${EXT}" STREQUAL ".asm" )
      list( APPEND ASMS "${SOURCE}" )
    else( )
      get_filename_component( NAME "${SOURCE}" NAME_WE )
      set( OUTPUT "${TARGET}_${NAME}.asm" )
      add_custom_command(
        OUTPUT "${OUTPUT}"
        DEPENDS "${SOURCE}" $<TARGET_FILE:${ARG_NATIVE}>
        COMMAND $<TARGET_FILE:q3lcc>
                ${INCLUDE_DIRECTORIES} ${COMPILE_DEFINITIONS} -DQ3_VM
                -o "${OUTPUT}" "${SOURCE}" )
      list( APPEND ASMS "${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT}" )
    endif( )
  endforeach( )

  add_custom_command(
    OUTPUT "${ARG_OUTPUT}"
    DEPENDS ${ASMS}
    COMMAND q3asm -o "${ARG_OUTPUT}" ${ASMS} )

  add_custom_target( "${TARGET}" ALL DEPENDS "${ARG_OUTPUT}" )
endfunction( )
