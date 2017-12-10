add_executable( tremulous
  ${ASM_SOURCES}
  ${QCOMMON_SOURCES}
  ${SYS_SOURCES}
  ${SERVER_SOURCES}
  ${SDL_CLIENT_SOURCES}
  cl_avi.cpp
  cl_cgame.cpp
  cl_cin.cpp
  cl_console.cpp
  cl_curl.cpp
  cl_curl.h
  client.h
  cl_input.cpp
  cl_keys.cpp
  cl_main.cpp
  cl_net_chan.cpp
  cl_parse.cpp
  cl_rest.cpp
  cl_rest.h
  cl_scrn.cpp
  cl_ui.cpp
  cl_updates.cpp
  cl_updates.h
  keycodes.h
  keys.h
  qal.cpp
  qal.h
  snd_adpcm.cpp
  snd_codec.cpp
  snd_codec.h
  snd_codec_ogg.cpp
  snd_codec_opus.cpp
  snd_codec_wav.cpp
  snd_dma.cpp
  snd_local.h
  snd_main.cpp
  snd_mem.cpp
  snd_mix.cpp
  snd_openal.cpp
  snd_public.h
  snd_wavelet.cpp )

target_include_directories( tremulous PUBLIC
  ${COMMON_INCLUDE_DIRECTORIES} )

target_compile_definitions( tremulous PUBLIC
  ${COMMON_COMPILE_DEFINITIONS} )

target_link_libraries( tremulous PUBLIC
  ${COMMON_LINK_LIBRARIES}
  OpenGL::GL
  OpenAL::AL
  SDL2::SDL2
  JPEG::jpeg
  Ogg::ogg
  Vorbis::vorbisfile )

set_target_properties( tremulous PROPERTIES ENABLE_EXPORTS ON )

install( TARGETS tremulous RUNTIME DESTINATION "${INSTDIR_EXE}" )

if( BUILD_SDK )
  install( FILES keycodes.h DESTINATION "${INSTDIR_INCLUDE}/client" )
endif( )
