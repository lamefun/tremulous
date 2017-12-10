# Official website: https://www.lua.org
set( ARCHIVE lua-5.3.4.tar.gz )

set( LUA_MAKE ${BUILD_MAKE}
  "INSTALL_TOP=${BUILD_PREFIX}"
  "PLAT=generic"
  "CC=${CMAKE_C_COMPILER}"
  "AR=${CMAKE_AR} rcu"
  "RANLIB=${CMAKE_RANLIB}" )

ExternalProject_Add( lua
  URL "${CMAKE_CURRENT_SOURCE_DIR}/${ARCHIVE}"
  PREFIX "${BUILD_PREFIX}"
  BUILD_IN_SOURCE TRUE
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ${LUA_MAKE}
  INSTALL_COMMAND ${LUA_MAKE} install )
