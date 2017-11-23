set( GAME_SOURCES
  g_main.c # Must be listed first!
  g_active.c
  g_admin.c
  g_buildable.c
  g_client.c
  g_cmds.c
  g_combat.c
  g_maprotation.c
  g_misc.c
  g_missile.c
  g_mover.c
  g_namelog.c
  g_physics.c
  g_playermodel.c
  g_session.c
  g_spawn.c
  g_svcmds.c
  g_target.c
  g_team.c
  g_trigger.c
  g_utils.c
  g_weapon.c
  g_weapondrop.c
  ${BGAME_SOURCES}
  ${BGAME_PMOVE_SOURCES}
)

add_library( game SHARED ${GAME_SOURCES} g_syscalls.c )

target_include_directories( game PRIVATE ${TREMULOUS_INCLUDE_DIRECTORIES} )

target_compile_definitions( game PRIVATE
  ${TREMULOUS_COMPILE_DEFINITIONS}
  GAME )

set_target_properties( game PROPERTIES PREFIX "" )

install( TARGETS game LIBRARY DESTINATION "${SHARED_MODULE_INSTALL_DIR}" )

build_qvm( game_qvm
  NATIVE game
  OUTPUT game.qvm
  SOURCES ${GAME_SOURCES} ${COMMON_QVM_SOURCES} g_syscalls.asm
  INCLUDE_DIRECTORIES ..
  COMPILE_DEFINITIONS ${TREMULOUS_COMPILE_DEFINITIONS} GAME )
