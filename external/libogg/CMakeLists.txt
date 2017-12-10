# Official website: https://xiph.org
set( ARCHIVE libogg-1.3.3.tar.gz )

if( MSVC )
  # TODO: MSVC support
else( )
  ExternalProject_Add( libogg
    URL "${CMAKE_CURRENT_SOURCE_DIR}/${ARCHIVE}"
    PREFIX "${BUILD_PREFIX}"
    CONFIGURE_COMMAND
      ${EXTERNAL_ENV} "${BUILD_PREFIX}/src/libogg/configure"
      ${EXTERNAL_CONFIGURE_ARGS}
    BUILD_COMMAND ${BUILD_MAKE} )
endif( )
