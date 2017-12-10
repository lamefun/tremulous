# Official website: https://libjpeg-turbo.org
set( ARCHIVE libjpeg-turbo-1.5.2.tar.gz )

if( MSVC )
  # TODO: MSVC support
else( )
  ExternalProject_Add( libjpeg-turbo
    URL "${CMAKE_CURRENT_SOURCE_DIR}/${ARCHIVE}"
    PREFIX "${BUILD_PREFIX}"
    CONFIGURE_COMMAND
      ${EXTERNAL_ENV} "${BUILD_PREFIX}/src/libjpeg-turbo/configure"
      ${EXTERNAL_CONFIGURE_ARGS} --without-turbojpeg
    BUILD_COMMAND ${BUILD_MAKE_COMMAND} )
endif( )
