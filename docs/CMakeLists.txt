if( INSTALL_LAYOUT STREQUAL Portable AND UNIX )
  install( FILES server.sh.template DESTINATION "${CMAKE_INSTALL_PREFIX}" )
endif( )
