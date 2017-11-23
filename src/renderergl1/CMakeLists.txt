add_library( renderer_opengl1 SHARED
  tr_animation.cpp
  tr_backend.cpp
  tr_bsp.cpp
  tr_cmds.cpp
  tr_curve.cpp
  tr_flares.cpp
  tr_image.cpp
  tr_init.cpp
  tr_light.cpp
  tr_main.cpp
  tr_marks.cpp
  tr_mesh.cpp
  tr_model.cpp
  tr_model_iqm.cpp
  tr_scene.cpp
  tr_shade.cpp
  tr_shade_calc.cpp
  tr_shader.cpp
  tr_shadows.cpp
  tr_sky.cpp
  tr_subs.cpp
  tr_surface.cpp
  tr_world.cpp
)

target_link_libraries( renderer_opengl1 renderercommon )

set_target_properties( renderer_opengl1 PROPERTIES PREFIX "" )

install( TARGETS renderer_opengl1
  LIBRARY DESTINATION "${SHARED_MODULE_INSTALL_DIR}" )
