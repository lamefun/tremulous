tr_types_h = files( 'tr_types.h' )

renderercommon_deps = [ libjpeg_dep, sdl2_dep, gl_dep ]

renderercommon_lib = static_library(
  'renderercommon',
  'tr_font.cpp',
  'tr_image_bmp.cpp',
  'tr_image_jpg.cpp',
  'tr_image_pcx.cpp',
  'tr_image_png.cpp',
  'tr_image_tga.cpp',
  'tr_noise.cpp',
  sdl_renderercommon_src,
  include_directories : root_srcdir_inc,
  dependencies : renderercommon_deps
)

renderercommon_dep = declare_dependency(
  include_directories : root_srcdir_inc,
  link_with : renderercommon_lib,
  dependencies : renderercommon_deps
)
