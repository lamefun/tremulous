include( ExternalProject )
set( ARCHIVE libjpeg-turbo-1.5.2.tar.gz )

if( WIN32 )
  # TODO: Win32 support
else( )
  ExternalProject_Add( libjpeg-turbo
    URL "${CMAKE_CURRENT_LIST_DIR}/${ARCHIVE}"
    PREFIX "${DEPS_PREFIX}"
    CONFIGURE_COMMAND
      "${DEPS_PREFIX}/src/libjpeg-turbo/configure"
      "--prefix=${DEPS_PREFIX}"
      --with-pic
      --enable-shared=
      --without-turbojpeg
    BUILD_COMMAND ${MAKE}
    BUILD_BYPRODUCTS "${DEPS_PREFIX}/lib/libjpeg.a" )

  add_library( EXT::libjpeg-turbo STATIC IMPORTED )
  add_dependencies( EXT::libjpeg-turbo libjpeg-turbo )
  set_target_properties( EXT::libjpeg-turbo PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${DEPS_PREFIX}/include"
    IMPORTED_LOCATION "${DEPS_PREFIX}/lib/libjpeg.a" )
endif( )
