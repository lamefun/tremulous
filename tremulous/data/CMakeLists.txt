# Install the Freedesktop application metadata used by Linux and most other
# UNIX-like systems (for example, the icons and the application launcher).
if( UNIX AND NOT APPLE AND INSTALL_TYPE STREQUAL Release )
  install( DIRECTORY icons DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}" )
  install( FILES "com.grangerhub.Tremulous.desktop"
           DESTINATION "${CMAKE_INSTALL_FULL_DATADIR}/applications" )
endif( )
