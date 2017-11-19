cgame_headers = files(
  'cg_local.h',
  'binaryshader.h'
)

cgame_src = files(
  'cg_main.c', # Must be listed first!
  'cg_animation.c',
  'cg_animmapobj.c',
  'cg_attachment.c',
  'cg_buildable.c',
  'cg_consolecmds.c',
  'cg_draw.c',
  'cg_drawtools.c',
  'cg_ents.c',
  'cg_event.c',
  'cg_marks.c',
  'cg_particles.c',
  'cg_players.c',
  'cg_playerstate.c',
  'cg_predict.c',
  'cg_rangemarker.c',
  'cg_scanner.c',
  'cg_servercmds.c',
  'cg_snapshot.c',
  'cg_trails.c',
  'cg_tutorial.c',
  'cg_view.c',
  'cg_weapons.c',
)

cgame_all_src = [
  cgame_src,
  bgame_src,
  bgame_move_src,
  ui_shared_src,
  qcommon_qvm_src
]

cgame_all_headers = [
  cgame_headers,
  bgame_headers,
  ui_shared_headers,
  qcommon_qvm_headers,
  tr_types_h
]

cgame_asms = q3lcc_gen.process(
  cgame_all_src,
  extra_args : game_q3lcc_args + [ '-DCGAME' ]
)

cgame_qvm = custom_target(
  'cgame_qvm',
  input : [ cgame_asms, 'cg_syscalls.asm' ],
  depend_files : cgame_all_headers,
  output : 'cgame.qvm',
  command : [ q3asm_exe, '-o', '@OUTPUT@', '@INPUT@' ],
  build_by_default : true
)

cgame_11_asms = q3lcc_gen.process(
  cgame_all_src,
  extra_args : game_q3lcc_args + [ '-DCGAME', '-DMODULE_INTERFACE_11' ]
)

cgame_11_qvm = custom_target(
  'cgame_11_qvm',
  input : [ cgame_11_asms, 'cg_syscalls_11.asm' ],
  depend_files : cgame_all_headers,
  output : 'cgame_11.qvm',
  command : [ q3asm_exe, '-o', '@OUTPUT@', '@INPUT@' ],
  build_by_default : true
)
