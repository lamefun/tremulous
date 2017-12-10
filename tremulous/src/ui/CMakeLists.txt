if( BUILD_SDK )
  install( FILES
    ui_public.h
    ui_syscalls.c
    ui_syscalls.asm
    ui_syscalls_11.asm
    DESTINATION "${INSTDIR_INCLUDE}/ui" )
endif( )
