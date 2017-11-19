qasm_lib = static_library(
  'qasm',
  'snapvector.c',
  include_directories : root_srcdir_inc
)

qasm_dep = declare_dependency(
  include_directories : root_srcdir_inc,
  link_with : qasm_lib
)
