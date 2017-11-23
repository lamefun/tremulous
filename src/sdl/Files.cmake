list_append_prefixed( SDL_CLIENT_SOURCES
  PREFIX "${CMAKE_CURRENT_LIST_DIR}/"
  sdl_input.cpp
  sdl_snd.cpp
)

list_append_prefixed( SDL_RENDERERCOMMON_SOURCES
  PREFIX "${CMAKE_CURRENT_LIST_DIR}/"
  sdl_gamma.cpp
  sdl_glimp.cpp
)
