set( GLSL_SHADERS
  glsl/bokeh_fp.glsl
  glsl/bokeh_vp.glsl
  glsl/calclevels4x_fp.glsl
  glsl/calclevels4x_vp.glsl
  glsl/depthblur_fp.glsl
  glsl/depthblur_vp.glsl
  glsl/dlight_fp.glsl
  glsl/dlight_vp.glsl
  glsl/down4x_fp.glsl
  glsl/down4x_vp.glsl
  glsl/fogpass_fp.glsl
  glsl/fogpass_vp.glsl
  glsl/generic_fp.glsl
  glsl/generic_vp.glsl
  glsl/lightall_fp.glsl
  glsl/lightall_vp.glsl
  glsl/pshadow_fp.glsl
  glsl/pshadow_vp.glsl
  glsl/shadowfill_fp.glsl
  glsl/shadowfill_vp.glsl
  glsl/shadowmask_fp.glsl
  glsl/shadowmask_vp.glsl
  glsl/ssao_fp.glsl
  glsl/ssao_vp.glsl
  glsl/texturecolor_fp.glsl
  glsl/texturecolor_vp.glsl
  glsl/tonemap_fp.glsl
  glsl/tonemap_vp.glsl
)

file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/glsl" )

foreach( SHARER_GLSL_REL IN LISTS GLSL_SHADERS )
  set( SHADER_GLSL "${CMAKE_CURRENT_SOURCE_DIR}/${SHARER_GLSL_REL}" )
  get_filename_component( SHADER_NAME "${SHADER_GLSL}" NAME_WE )
  set( SHADER_C "${CMAKE_CURRENT_BINARY_DIR}/glsl/${SHADER_NAME}.c" )

  add_custom_command(
    DEPENDS "${SHADER_GLSL}"
    OUTPUT "${SHADER_C}"
    COMMAND
      ${PYTHON3_COMMAND} "${CMAKE_CURRENT_SOURCE_DIR}/pack_glsl.py"
      "${SHADER_GLSL}" "${SHADER_C}" )

  list( APPEND SHADER_SOURCES "${SHADER_C}" )
endforeach( )

add_library( renderer_opengl2 SHARED
  tr_animation.cpp
  tr_backend.cpp
  tr_bsp.cpp
  tr_cmds.cpp
  tr_curve.cpp
  tr_dsa.cpp
  tr_extensions.cpp
  tr_extramath.cpp
  tr_fbo.cpp
  tr_flares.cpp
  tr_glsl.cpp
  tr_image.cpp
  tr_image_dds.cpp
  tr_init.cpp
  tr_light.cpp
  tr_main.cpp
  tr_marks.cpp
  tr_mesh.cpp
  tr_model.cpp
  tr_model_iqm.cpp
  tr_postprocess.cpp
  tr_scene.cpp
  tr_shade.cpp
  tr_shade_calc.cpp
  tr_shader.cpp
  tr_shadows.cpp
  tr_sky.cpp
  tr_subs.cpp
  tr_surface.cpp
  tr_vbo.cpp
  tr_world.cpp
  ${SHADER_SOURCES}
)

target_link_libraries( renderer_opengl2 renderercommon )

set_target_properties( renderer_opengl2 PROPERTIES PREFIX "" )

install( TARGETS renderer_opengl2
  LIBRARY DESTINATION "${SHARED_MODULE_INSTALL_DIR}" )
