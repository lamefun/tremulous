#===============================================================================
#
# Function for building the game mods.
#
#===============================================================================

#-------------------------------------------------------------------------------
# add_tremulous_game - builds and packages a game mod
#-------------------------------------------------------------------------------

function( add_tremulous_game target )

  set( FLAGS NO_11_COMPATIBILITY )
  set( OPTS1 GPP_VMS_PK3 11_VMS_PK3 DESTINATION )
  set( OPTSN INCLUDE_DIRECTORIES GAME_SOURCES CGAME_SOURCES UI_SOURCES )
  cmake_parse_arguments( ARG "${FLAGS}" "${OPTS1}" "${OPTSN}" ${ARGN} )

  if( NOT Tremulous_SDK_FOUND )
    message( FATAL_ERROR "Tremulous SDK is missing" )
  endif( )

  set( vms_gpp_files )
  set( vms_11_files )

  function( add_module module prefix )
    string( TOUPPER "${module}" module_uppercase )

    set( common_sources
      "${Tremulous_INCLUDE_DIR}/qcommon/q_shared.c"
      "${Tremulous_INCLUDE_DIR}/qcommon/q_math.c" )
    set( sources ${ARG_${module_uppercase}_SOURCES} ${common_sources} )
    set( include_dirs "${Tremulous_INCLUDE_DIR}" ${ARG_INCLUDE_DIRECTORIES} )

    set( native_target "${target}_${module}" )
    set( qvm_gpp_target "${target}_${module}_qvm" )
    set( qvm_gpp_file "${target}_${module}.qvm" )
    set( qvm_11_target "${target}_${module}_11_qvm" )
    set( qvm_11_file "${target}_${module}_11.qvm" )

    if( NOT sources )
      message( STATUS "No sources added for the ${module} module of the game"
                      "mod \"${target}\": skipping" )
      return( )
    endif( )

    set( module_includes "${Tremulous_INCLUDE_DIR}/${module}" )
    set( syscalls_c "${module_includes}/${prefix}_syscalls.c" )
    set( syscalls_asm "${module_includes}/${prefix}_syscalls.asm" )
    set( syscalls_11_asm "${module_includes}/${prefix}_syscalls_11.asm" )

    add_library( "${native_target}" SHARED ${sources} "${syscalls_c}" )
    set_target_properties( "${native_target}" PROPERTIES PREFIX "" )
    target_include_directories( "${native_target}" PRIVATE ${include_dirs} )

    if( ARG_DESTINATION )
      install( TARGETS "${native_target}"
        RUNTIME DESTINATION "${ARG_DESTINATION}"
        LIBRARY DESTINATION "${ARG_DESTINATION}" )
    endif( )

    add_tremulous_qvm( "${qvm_gpp_target}"
      OUTPUT "${qvm_gpp_file}"
      NATIVE "${native_target}"
      SOURCES ${sources} "${syscalls_asm}"
      INCLUDE_DIRECTORIES ${include_dirs}
      COMPILE_DEFINITIONS ${module_uppercase} )
    list( APPEND vms_gpp_files
      FILE "${CMAKE_CURRENT_BINARY_DIR}/${qvm_gpp_file}"
      AS "vm/${module}.qvm" )

    if( NOT NO_11_COMPATIBILITY AND NOT module STREQUAL game )
      add_tremulous_qvm( "${qvm_11_target}"
        OUTPUT "${qvm_11_file}"
        NATIVE ${native_target}
        SOURCES ${sources} ${syscalls_11_asm}
        INCLUDE_DIRECTORIES ${include_dirs}
        COMPILE_DEFINITIONS ${module_uppercase} MODULE_INTERFACE_11 )
      list( APPEND vms_11_files
        FILE "${CMAKE_CURRENT_BINARY_DIR}/${qvm_11_file}"
        AS vm/${module}.qvm )
    endif( )

    set( vms_gpp_files ${vms_gpp_files} PARENT_SCOPE )
    set( vms_11_files ${vms_11_files} PARENT_SCOPE )
  endfunction( )

  add_module( game g )
  add_module( cgame cg )
  add_module( ui ui )

  if( vms_gpp_files )
    add_tremulous_pk3( "${target}_vms_gpp_pk3"
      OUTPUT "${ARG_GPP_VMS_PK3}" ${vms_gpp_files} )
    if( ARG_DESTINATION )
      install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${ARG_GPP_VMS_PK3}"
               DESTINATION "${ARG_DESTINATION}" )
    endif( )
  endif( )

  if( vms_11_files )
    add_tremulous_pk3( "${target}_vms_11_pk3"
    OUTPUT "${ARG_11_VMS_PK3}" ${vms_11_files} )
    if( ARG_DESTINATION )
      install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${ARG_11_VMS_PK3}"
               DESTINATION "${ARG_DESTINATION}" )
    endif( )
  endif( )

endfunction( )

#-------------------------------------------------------------------------------
# add_tremulous_qvm - builds a QVM
#-------------------------------------------------------------------------------

function( add_tremulous_qvm target )
  set( ARGS_1 OUTPUT NATIVE )
  set( ARGS_N SOURCES INCLUDE_DIRECTORIES COMPILE_DEFINITIONS )
  cmake_parse_arguments( ARG "" "${ARGS_1}" "${ARGS_N}" ${ARGN} )

  if( NOT Tremulous_SDK_FOUND )
    message( FATAL_ERROR "Tremulous SDK is missing" )
  endif( )

  set( include_args )
  foreach( dir IN LISTS ARG_INCLUDE_DIRECTORIES )
    list( APPEND include_args "-I${dir}" )
  endforeach( )

  set( define_args )
  foreach( def IN LISTS ARG_COMPILE_DEFINITIONS )
    list( APPEND define_args "-D${def}" )
  endforeach( )

  set( asm_files )
  set( asm_dir "${CMAKE_CURRENT_BINARY_DIR}/${target}.dir" )
  file( MAKE_DIRECTORY "${asm_dir}" )
  foreach( source IN LISTS ARG_SOURCES )
    if( NOT IS_ABSOLUTE "${source}" )
      set( source "${CMAKE_CURRENT_SOURCE_DIR}/${source}" )
    endif( )
    get_filename_component( ext "${source}" EXT )
    if( ext STREQUAL .asm )
      list( APPEND asm_files "${source}" )
    elseif( ext STREQUAL .c )
      get_filename_component( name "${source}" NAME_WE )
      set( output "${asm_dir}/${name}.asm" )
      add_custom_command(
        OUTPUT "${output}"
        COMMENT "Q3LCC ${name}.asm"
        DEPENDS "${source}" "${native_target}"
        COMMAND "${Tremulous_Q3LCC}"
          ${include_args} ${define_args} -DQ3_VM
          -o "${output}" "${source}" )
      list( APPEND asm_files "${output}" )
    endif( )
  endforeach( )

  add_custom_command(
    OUTPUT "${ARG_OUTPUT}"
    DEPENDS ${asm_files}
    COMMAND "${Tremulous_Q3ASM}" -o "${ARG_OUTPUT}" ${asm_files} )

  add_custom_target( "${target}" ALL DEPENDS "${ARG_OUTPUT}" )
endfunction( )
