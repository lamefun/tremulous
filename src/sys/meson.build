if platform_windows
  sys_platform_src = files( 'sys_win32.cpp', 'con_passive.cpp' )
elif platform_unix
  sys_platform_src = files( 'sys_unix.cpp', 'con_tty.cpp' )
else
  meson.error('Unsupported platform.')
endif

sys_src = [ files( 'sys_main.cpp', 'con_log.cpp' ), sys_platform_src ]

sys_deps = [ restclient_dep, sol_dep, sdl2_dep ]
