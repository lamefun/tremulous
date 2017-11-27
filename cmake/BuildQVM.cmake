# Utility function for building QVMs.
#
# Usage: build_qvm( TARGET_NAME ... )
#
# Arguments:
#
#   OUTPUT - Name of the .qvm file.
#   NATIVE - Native target to monitor for changes.
#   SOURCES - List of all source files.
#   INCLUDE_DIRECTORIES - Include directories.
#   COMPILE_DEFINITIONS - C compiler definitions.

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
