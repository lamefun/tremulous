add_library( zlib STATIC
  adler32.c
  crc32.c
  inffast.c
  inflate.c
  inftrees.c
  zutil.c
)

target_include_directories ( zlib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" )
