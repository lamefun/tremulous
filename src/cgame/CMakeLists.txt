set( CGAME_SOURCES
  cg_main.c # Must be listed first!
  cg_animation.c
  cg_animmapobj.c
  cg_attachment.c
  cg_buildable.c
  cg_consolecmds.c
  cg_draw.c
  cg_drawtools.c
  cg_ents.c
  cg_event.c
  cg_marks.c
  cg_particles.c
  cg_players.c
  cg_playerstate.c
  cg_predict.c
  cg_rangemarker.c
  cg_scanner.c
  cg_servercmds.c
  cg_snapshot.c
  cg_trails.c
  cg_tutorial.c
  cg_view.c
  cg_weapons.c
  ${BGAME_SOURCES}
  ${BGAME_PMOVE_SOURCES}
  ${UI_SHARED_SOURCES} )

add_library( cgame SHARED ${CGAME_SOURCES} cg_syscalls.c )

target_include_directories( cgame PRIVATE ${TREMULOUS_INCLUDE_DIRECTORIES} )

target_compile_definitions( cgame PRIVATE
  ${TREMULOUS_COMPILE_DEFINITIONS}
  CGAME )

set_target_properties( cgame PROPERTIES PREFIX "" )

install( TARGETS cgame LIBRARY DESTINATION "${SHARED_MODULE_INSTALL_DIR}" )

build_qvm( cgame_qvm
  NATIVE cgame
  OUTPUT cgame.qvm
  SOURCES ${CGAME_SOURCES} ${COMMON_QVM_SOURCES} cg_syscalls.asm
  INCLUDE_DIRECTORIES ..
  COMPILE_DEFINITIONS ${TREMULOUS_COMPILE_DEFINITIONS} CGAME )

build_qvm( cgame_11_qvm
  NATIVE cgame
  OUTPUT cgame_11.qvm
  SOURCES ${CGAME_SOURCES} ${COMMON_QVM_SOURCES} cg_syscalls_11.asm
  INCLUDE_DIRECTORIES ..
  COMPILE_DEFINITIONS
    ${TREMULOUS_COMPILE_DEFINITIONS}
    MODULE_INTERFACE_11
    CGAME )
