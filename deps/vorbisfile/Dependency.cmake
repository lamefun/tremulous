acquire_dependency( vorbisfile REQUIRED
  PKG_CONFIG vorbisfile
  BUNDLED_SOURCE_DIRS
    deps/vorbisfile/libogg-1.3.2
    deps/vorbisfile/libvorbis-1.3.5
  BUNDLED_SOURCE_LIBRARIES vorbis )
