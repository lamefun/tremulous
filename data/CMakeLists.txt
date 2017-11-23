if( BUILD_GAME )
  add_subdirectory( game )
  install( FILES
    base/data-1.1.0.pk3
    base/data-gpp1.pk3
    base/map-arachnid2-1.1.0.pk3
    base/map-atcs-1.1.0.pk3
    base/map-karith-1.1.0.pk3
    base/map-nexus6-1.1.0.pk3
    base/map-niveus-1.1.0.pk3
    base/map-transit-1.1.0.pk3
    base/map-tremor-1.1.0.pk3
    base/map-uncreation-1.1.0.pk3
    DESTINATION "${BASE_DATA_INSTALL_DIR}" )
endif( )

if( INSTALL_FREEDESKTOP_METADATA )
  install( DIRECTORY icons DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}" )
  install( FILES "com.grangerhub.Tremulous.desktop"
    DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/applications" )
endif( )
