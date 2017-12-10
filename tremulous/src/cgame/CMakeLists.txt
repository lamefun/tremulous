if( BUILD_SDK )
  install( FILES
    cg_public.h
    cg_syscalls.c
    cg_syscalls.asm
    cg_syscalls_11.asm
    DESTINATION "${INSTDIR_INCLUDE}/cgame" )
endif( )
