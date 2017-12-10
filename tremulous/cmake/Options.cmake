set( ENGINE_OPTIONS
  BUILD_CLIENT
  BUILD_SERVER
  BUILD_SDK
  INSTALL_CMAKE_PACKAGE
  INSTALL_DOCS
  PRIMARY_MOD
  INSTALL_TYPE )

set( INSTALL_TYPES Release|Debug )

option( BUILD_CLIENT "Build Tremulous client" ON )
option( BUILD_SERVER "Build Tremulous server" ON )
option( BUILD_SDK "Build game modification SDK" ON )
option( INSTALL_HEADERS "Install C++ headers" ON )
option( INSTALL_CMAKE_PACKAGE "Install CMake package" ON )
option( INSTALL_DOCS "Install documentation" ON )
set( PRIMARY_MOD "gpp" CACHE STRING "Primary game mod" )
set( INSTALL_TYPE "Release" CACHE STRING "Install type (${INSTALL_TYPES})" )
