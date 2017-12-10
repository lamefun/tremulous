# Official website: https://xiph.org
set( ARCHIVE libvorbis-1.3.5.tar.gz )

if( MSVC )
  # TODO: MSVC support
else( )
  ExternalProject_Add( libvorbis
    DEPENDS libogg
    URL "${CMAKE_CURRENT_SOURCE_DIR}/${ARCHIVE}"
    PREFIX "${BUILD_PREFIX}"
    CONFIGURE_COMMAND
      ${EXTERNAL_ENV} "${BUILD_PREFIX}/src/libvorbis/configure"
      ${EXTERNAL_CONFIGURE_ARGS}
    BUILD_COMMAND ${BUILD_MAKE} )
endif( )
