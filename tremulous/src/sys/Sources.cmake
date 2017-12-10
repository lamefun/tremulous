expand_paths( SYS_SOURCES
  ROOT "${CMAKE_CURRENT_LIST_DIR}"
  dialog.h
  sys_shared.h
  sys_local.h
  sys_loadlib.h
  sys_main.cpp
  con_log.cpp )

if( WIN32 )
  expand_paths_append( SYS_SOURCES
    ROOT "${CMAKE_CURRENT_LIST_DIR}"
    win_resource.h
    sys_win32.cpp
    sys_win32_default_homepath.cpp
    con_passive.cpp )
else( )
  expand_paths_append( SYS_SOURCES
    ROOT "${CMAKE_CURRENT_LIST_DIR}"
    sys_unix.cpp
    con_tty.cpp )
endif( )
