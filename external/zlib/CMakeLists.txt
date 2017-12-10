# Official website: https://www.zlib.net
set( ARCHIVE zlib-1.2.11.tar.gz )

if( MSVC )
  # TODO: MSVC support
else( )
  ExternalProject_Add( zlib
    URL "${CMAKE_CURRENT_SOURCE_DIR}/${ARCHIVE}"
    PREFIX "${BUILD_PREFIX}"
    CMAKE_CACHE_ARGS ${EXTERNAL_CMAKE_CACHE_ARGS}
    CMAKE_ARGS ${EXTERNAL_CMAKE_ARGS} )
endif( )
