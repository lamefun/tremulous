ui_shared_headers = files( 'ui_shared.h' )

ui_headers = files( 'ui_local.h' )

ui_shared_src = files( 'ui_shared.c' )

ui_src = files(
  'ui_main.c', # Must be listed first!
  'ui_atoms.c',
  'ui_gameinfo.c'
)

ui_all_src = [
  ui_src,
  ui_shared_src,
  bgame_src,
  qcommon_qvm_src
]

ui_all_headers = [
  ui_headers,
  ui_shared_headers,
  bgame_headers,
  qcommon_qvm_headers,
  keycodes_h
]

ui_asms = q3lcc_gen.process(
  ui_all_src,
  extra_args : game_q3lcc_args + [ '-DUI' ]
)

ui_qvm = custom_target(
  'ui_qvm',
  input : [ ui_asms, 'ui_syscalls.asm' ],
  depend_files : ui_all_src,
  output : 'ui.qvm',
  command : [ q3asm_exe, '-o', '@OUTPUT@', '@INPUT@' ],
  build_by_default : true
)

ui_11_asms = q3lcc_gen.process(
  ui_all_src,
  extra_args : game_q3lcc_args + [ '-DUI', '-DMODULE_INTERFACE_11' ]
)

ui_11_qvm = custom_target(
  'ui_11_qvm',
  input : [ ui_11_asms, 'ui_syscalls_11.asm' ],
  depend_files : ui_all_src,
  output : 'ui_11.qvm',
  command : [ q3asm_exe, '-o', '@OUTPUT@', '@INPUT@' ],
  build_by_default : true
)
