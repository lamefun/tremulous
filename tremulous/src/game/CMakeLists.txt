if( BUILD_SDK )
  install( FILES
    g_public.h
    g_syscalls.c
    g_syscalls.asm
    DESTINATION "${INSTDIR_INCLUDE}/game" )
endif( )
