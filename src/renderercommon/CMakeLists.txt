add_library( renderercommon STATIC
  tr_font.cpp
  tr_image_bmp.cpp
  tr_image_jpg.cpp
  tr_image_pcx.cpp
  tr_image_png.cpp
  tr_image_tga.cpp
  tr_noise.cpp
  ${SDL_RENDERERCOMMON_SOURCES}
)

target_include_directories( renderercommon PUBLIC
  ${TREMULOUS_INCLUDE_DIRECTORIES} )

target_compile_definitions( renderercommon PUBLIC
  ${TREMULOUS_COMPILE_DEFINITIONS} )

target_link_libraries( renderercommon PUBLIC
  OpenGL::GL
  ${SDL2_LIB_TARGETS}
  ${LIBJPEG_LIB_TARGETS}
  tremulous )
