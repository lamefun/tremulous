bgame_headers = files(
  'bg_lib.h',
  'bg_local.h'
)

game_headers = files(
  'tremulous.h',
  'g_local.h',
  'g_public.h',
  'g_admin.h'
)

bgame_src = files(
  'bg_alloc.c',
  'bg_lib.c',
  'bg_misc.c',
  'bg_voice.c'
)

bgame_move_src = files( 'bg_slidemove.c', 'bg_pmove.c' )

game_src = files(
  'g_main.c', # Must be listed first!
  'g_active.c',
  'g_admin.c',
  'g_buildable.c',
  'g_client.c',
  'g_cmds.c',
  'g_combat.c',
  'g_maprotation.c',
  'g_misc.c',
  'g_missile.c',
  'g_mover.c',
  'g_namelog.c',
  'g_physics.c',
  'g_playermodel.c',
  'g_session.c',
  'g_spawn.c',
  'g_svcmds.c',
  'g_target.c',
  'g_team.c',
  'g_trigger.c',
  'g_utils.c',
  'g_weapon.c',
  'g_weapondrop.c'
)

game_asms = q3lcc_gen.process(
  game_src,
  bgame_src,
  bgame_move_src,
  qcommon_qvm_src,
  extra_args : game_q3lcc_args + [ '-DGAME' ]
)

game_qvm = custom_target(
  'game_qvm',
  input : [ game_asms, 'g_syscalls.asm' ],
  depend_files : [ game_headers, bgame_headers, qcommon_qvm_headers ],
  output : 'game.qvm',
  command : [ q3asm_exe, '-o', '@OUTPUT@', '@INPUT@' ],
  build_by_default : true
)
