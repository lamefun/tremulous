server_src = files(
  'sv_ccmds.cpp',
  'sv_client.cpp',
  'sv_game.cpp',
  'sv_init.cpp',
  'sv_main.cpp',
  'sv_net_chan.cpp',
  'sv_snapshot.cpp',
  'sv_world.cpp'
)

if get_option( 'build_server' )
  tremulous_server_exe = executable(
    'tremulous_server', 
    qcommon_src,
    qcommon_sys_src,
    sys_src,
    server_src,
    null_server_src,
    c_args : [ tremulous_cflags, '-DDEDICATED' ],
    cpp_args : [ tremulous_cflags, '-DDEDICATED' ],
    dependencies : [
      qcommon_deps,
      sys_deps,
      script_api_dep
    ],
    install : true,
    install_dir : game_exe_dir 
  )
endif
