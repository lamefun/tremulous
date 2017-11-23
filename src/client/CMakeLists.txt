add_executable( tremulous
  ${SERVER_SOURCES}
  ${COMMON_SOURCES}
  ${SDL_CLIENT_SOURCES}
  cl_avi.cpp
  cl_cin.cpp
  cl_curl.cpp
  cl_keys.cpp
  cl_parse.cpp
  cl_scrn.cpp
  snd_codec.cpp
  snd_codec_wav.cpp
  snd_dma.cpp
  snd_main.cpp
  snd_openal.cpp
  snd_wavelet.cpp
  cl_cgame.cpp
  cl_console.cpp
  cl_input.cpp
  cl_main.cpp
  cl_net_chan.cpp
  cl_rest.cpp
  cl_ui.cpp
  cl_updates.cpp
  qal.cpp
  snd_adpcm.cpp
  snd_codec_ogg.cpp
  snd_codec_opus.cpp
  snd_mem.cpp
  snd_mix.cpp
)

target_include_directories( tremulous PRIVATE
  ${TREMULOUS_INCLUDE_DIRECTORIES} )

target_compile_definitions( tremulous PRIVATE
  ${TREMULOUS_COMPILE_DEFINITIONS} )

target_link_libraries( tremulous
  ${COMMON_LINK_LIBRARIES}
  ${RAPIDJSON_LIB_TARGETS}
  ${RESTCLIENT_LIB_TARGETS}
  ${SEMVER_LIB_TARGETS}
  ${SDL2_LIB_TARGETS}
  ${LIBJPEG_LIB_TARGETS}
  ${OPENAL_LIB_TARGETS}
  ${VORBISFILE_LIB_TARGETS}
)

set_target_properties( tremulous PROPERTIES ENABLE_EXPORTS ON )

install( TARGETS tremulous RUNTIME DESTINATION "${GAME_EXE_INSTALL_DIR}" )
