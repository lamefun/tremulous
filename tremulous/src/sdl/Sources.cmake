expand_paths( SDL_CLIENT_SOURCES
  ROOT "${CMAKE_CURRENT_LIST_DIR}"
  sdl_input.cpp
  sdl_snd.cpp )

expand_paths( SDL_RENDERER_SOURCES
  ROOT "${CMAKE_CURRENT_LIST_DIR}"
  sdl_gamma.cpp
  sdl_glimp.cpp
  sdl_icon.h )
