acquire_dependency( libjpeg REQUIRED
  PKG_CONFIG libjpeg
  BUNDLED_CMAKE_INCLUDES deps/libjpeg/ExternalProject.cmake
  BUNDLED_SOURCE_LIBRARIES EXT::libjpeg-turbo )
