add_library( ogg STATIC
  include/ogg/config_types.h
  include/ogg/ogg.h
  include/ogg/os_types.h
  src/bitwise.c
  src/framing.c )

target_include_directories( ogg PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include" )
