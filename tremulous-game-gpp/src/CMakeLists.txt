set( GAME_SHARED_SOURCES
  game/bg_alloc.c
  game/bg_lib.c
  game/bg_lib.h
  game/bg_misc.c
  game/bg_public.h
  game/bg_voice.c )

set( GAME_PLAYER_MOVEMENT_SOURCES
  game/bg_pmove.c
  game/bg_slidemove.c )

set( UI_SHARED_SOURCES
  ui/ui_shared.c
  ui/ui_shared.h )

set( GAME_SOURCES
  game/g_main.c
  game/g_client.c
  game/g_namelog.c
  game/g_trigger.c
  game/g_combat.c
  game/g_svcmds.c
  game/g_buildable.c
  game/g_physics.c
  game/g_weapondrop.c
  game/g_misc.c
  game/g_team.c
  game/bg_local.h
  game/g_mover.c
  game/g_admin.c
  game/g_playermodel.c
  game/g_session.c
  game/tremulous.h
  game/g_spawn.c
  game/g_maprotation.c
  game/bg_shared.h
  game/g_weapon.c
  game/g_cmds.c
  game/g_admin.h
  game/g_target.c
  game/g_active.c
  game/g_utils.c
  game/g_local.h
  game/g_missile.c
  ${GAME_SHARED_SOURCES}
  ${GAME_PLAYER_MOVEMENT_SOURCES})

set( CGAME_SOURCES
  cgame/cg_main.c
  cgame/cg_view.c
  cgame/cg_rangemarker.c
  cgame/cg_attachment.c
  cgame/cg_marks.c
  cgame/cg_trails.c
  cgame/cg_buildable.c
  cgame/cg_servercmds.c
  cgame/cg_predict.c
  cgame/cg_event.c
  cgame/cg_drawtools.c
  cgame/cg_weapons.c
  cgame/cg_ents.c
  cgame/cg_snapshot.c
  cgame/cg_players.c
  cgame/cg_consolecmds.c
  cgame/cg_tutorial.c
  cgame/cg_animation.c
  cgame/cg_particles.c
  cgame/cg_playerstate.c
  cgame/cg_animmapobj.c
  cgame/cg_scanner.c
  cgame/binaryshader.h
  cgame/cg_local.h
  cgame/cg_draw.c
  ${UI_SHARED_SOURCES}
  ${GAME_SHARED_SOURCES}
  ${GAME_PLAYER_MOVEMENT_SOURCES} )

set( UI_SOURCES
  ui/ui_main.c
  ui/ui_gameinfo.c
  ui/ui_atoms.c
  ui/ui_local.h
  ${UI_SHARED_SOURCES}
  ${GAME_SHARED_SOURCES})

add_tremulous_game( gpp
  GPP_VMS_PK3 "gpp-vms-tremulous-${Tremulous_VERSION_STRING}.pk3"
  11_VMS_PK3 "11-vms-tremulous-${Tremulous_VERSION_STRING}.pk3"
  DESTINATION "${Tremulous_DATA_DIR}/gpp"
  GAME_SOURCES ${GAME_SOURCES}
  CGAME_SOURCES ${CGAME_SOURCES}
  UI_SOURCES ${UI_SOURCES}
  INCLUDE_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${PROJECT_SOURCE_DIR}/data" )
