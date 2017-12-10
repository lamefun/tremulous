# Official website: https://xiph.org
set( ARCHIVE openssl-1.1.0g.tar.gz )

if( MSVC )
  # TODO: MSVC support
else( )
  ExternalProject_Add( openssl
    URL "${CMAKE_CURRENT_SOURCE_DIR}/${ARCHIVE}"
    PREFIX "${BUILD_PREFIX}"
    CONFIGURE_COMMAND
      "${BUILD_PREFIX}/src/openssl/config" no-shared
      "--prefix=${BUILD_PREFIX}"
      "--with-zlib-include=${BUILD_PREFIX}/include"
      "--with-zlib-lib=${BUILD_PREFIX}/lib"
    BUILD_COMMAND ${BUILD_MAKE_COMMAND}
    INSTALL_COMMAND ${BUILD_MAKE_COMMAND} install_sw )
endif( )
