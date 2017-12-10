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
  glsl/tonemap_vp.glsl )

set( SHADER_SOURCES )

file( MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/glsl" )

foreach( filename IN LISTS GLSL_SHADERS )
  get_filename_component( shader "${filename}" NAME_WE )

  set( script "${CMAKE_CURRENT_SOURCE_DIR}/PackGLSL.cmake" )
  set( input "${CMAKE_CURRENT_SOURCE_DIR}/${filename}" )
  set( output "${CMAKE_CURRENT_BINARY_DIR}/glsl/${shader}.c" )

  list( APPEND SHADER_SOURCES "${output}" )

  add_custom_command(
    DEPENDS "${input}" "${script}"
    OUTPUT "${output}"
    COMMENT "Packing GLSL shader: ${filename}"
    COMMAND
      ${CMAKE_COMMAND}
      "-Dshader=${shader}"
      "-Dinput=${input}"
      "-Doutput=${output}"
      -P "${script}" )
endforeach( )

add_library( renderer_opengl2 SHARED
  tr_animation.cpp
  tr_backend.cpp
  tr_bsp.cpp
  tr_cmds.cpp
  tr_curve.cpp
  tr_dsa.cpp
  tr_dsa.h
  tr_extensions.cpp
  tr_extramath.cpp
  tr_extramath.h
  tr_extratypes.h
  tr_fbo.cpp
  tr_fbo.h
  tr_flares.cpp
  tr_glsl.cpp
  tr_image.cpp
  tr_image_dds.cpp
  tr_init.cpp
  tr_light.cpp
  tr_local.h
  tr_main.cpp
  tr_marks.cpp
  tr_mesh.cpp
  tr_model.cpp
  tr_model_iqm.cpp
  tr_postprocess.cpp
  tr_postprocess.h
  tr_scene.cpp
  tr_shade_calc.cpp
  tr_shade.cpp
  tr_shader.cpp
  tr_shadows.cpp
  tr_sky.cpp
  tr_subs.cpp
  tr_surface.cpp
  tr_vbo.cpp
  tr_world.cpp
  ${SHADER_SOURCES} )

target_link_libraries( renderer_opengl2 renderercommon )

set_target_properties( renderer_opengl2 PROPERTIES PREFIX "" )

install( TARGETS renderer_opengl2 LIBRARY DESTINATION "${INSTDIR_DLL}" )
