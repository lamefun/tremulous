renderergl2_glsl = [
  'bokeh_fp.glsl',
  'bokeh_vp.glsl',
  'calclevels4x_fp.glsl',
  'calclevels4x_vp.glsl',
  'depthblur_fp.glsl',
  'depthblur_vp.glsl',
  'dlight_fp.glsl',
  'dlight_vp.glsl',
  'down4x_fp.glsl',
  'down4x_vp.glsl',
  'fogpass_fp.glsl',
  'fogpass_vp.glsl',
  'generic_fp.glsl',
  'generic_vp.glsl',
  'lightall_fp.glsl',
  'lightall_vp.glsl',
  'pshadow_fp.glsl',
  'pshadow_vp.glsl',
  'shadowfill_fp.glsl',
  'shadowfill_vp.glsl',
  'shadowmask_fp.glsl',
  'shadowmask_vp.glsl',
  'ssao_fp.glsl',
  'ssao_vp.glsl',
  'texturecolor_fp.glsl',
  'texturecolor_vp.glsl',
  'tonemap_fp.glsl',
  'tonemap_vp.glsl'
]

pack_glsl_script = find_program( 'pack_glsl.py' )

pack_glsl_gen = generator(
  pack_glsl_script,
  output : '@BASENAME@.c',
  arguments : [ '@INPUT@', '@OUTPUT@' ]
)

renderergl2_glsl_c = pack_glsl_gen.process( renderergl2_glsl )
