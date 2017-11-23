if( WIN32 )
  list_append_sources( SYS_SOURCES
    sys_win32.cpp
    sys_win32_default_homepath.cpp
    con_passive.cpp
  )
else( )
  list_append_sources( SYS_SOURCES sys_unix.cpp con_tty.cpp )
endif( )

list_append_sources( SYS_SOURCES sys_main.cpp con_log.cpp )

list( APPEND SYS_SOURCES ${QCOMMON_SYS_SOURCES} )

message( STATUS "!! ${SYS_SOURCES}" "!! ${QCOMMON_SYS_SOURCES}" )
