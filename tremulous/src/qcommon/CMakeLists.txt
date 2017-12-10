if( BUILD_SDK )
  install( FILES
    surfaceflags.h
    q_platform.h
    q_shared.h
    q_shared.c
    q_math.c
    DESTINATION "${INSTDIR_INCLUDE}/qcommon" )
endif( )
