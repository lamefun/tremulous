add_library( renderercommon STATIC
  ${SDL_RENDERER_SOURCES}
  iqm.h
  qgl.h
  tr_common.h
  tr_font.cpp
  tr_image_bmp.cpp
  tr_image_jpg.cpp
  tr_image_pcx.cpp
  tr_image_png.cpp
  tr_image_tga.cpp
  tr_noise.cpp
  tr_public.h
  tr_types.h )

target_link_libraries( renderercommon PUBLIC tremulous )

if( BUILD_SDK )
  install( FILES tr_types.h DESTINATION "${INSTDIR_INCLUDE}/renderercommon" )
endif( )
