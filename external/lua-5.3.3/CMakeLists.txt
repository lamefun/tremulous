add_library( lua STATIC 
  src/lapi.c
  src/lcode.c
  src/lctype.c
  src/ldebug.c
  src/ldo.c
  src/ldump.c
  src/lfunc.c
  src/lgc.c
  src/llex.c
  src/lmem.c
  src/lobject.c
  src/lopcodes.c
  src/lparser.c
  src/lstate.c
  src/lstring.c
  src/ltable.c
  src/ltm.c
  src/lundump.c
  src/lvm.c
  src/lzio.c
  src/lauxlib.c
  src/lbaselib.c
  src/lbitlib.c
  src/lcorolib.c
  src/ldblib.c
  src/liolib.c
  src/lmathlib.c
  src/loslib.c
  src/lstrlib.c
  src/ltablib.c
  src/lutf8lib.c
  src/loadlib.c
  src/linit.c
)

target_include_directories( lua PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if( APPLE )
  target_compile_definitions( lua PRIVATE -DLUA_USE_MACOSX )
endif( )

if( "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" )
  target_compile_definitions( lua PRIVATE -DLUA_USE_LINUX )
endif( )

target_compile_definitions( lua PUBLIC -DLUA_COMPAT_5_2 )
target_compile_options( lua PRIVATE -mfpmath=sse -ffast-math )
