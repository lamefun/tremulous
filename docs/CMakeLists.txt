if( INSTALL_LAYOUT STREQUAL Portable AND UNIX )
  install( FILES server.sh.template DESTINATION "${DOC_INSTALL_DIR}" )
endif( )
