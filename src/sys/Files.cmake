list_append_prefixed( COMMON_SOURCES
  PREFIX "${CMAKE_CURRENT_LIST_DIR}/"
  sys_main.cpp
  con_log.cpp
)

if( WIN32 )
  list_append_prefixed( COMMON_SOURCES
    PREFIX "${CMAKE_CURRENT_LIST_DIR}/"
    sys_win32.cpp
    sys_win32_default_homepath.cpp
    con_passive.cpp
  )
else( )
  list_append_prefixed( COMMON_SOURCES
    PREFIX "${CMAKE_CURRENT_LIST_DIR}/"
    sys_unix.cpp
    con_tty.cpp
  )
endif( )
