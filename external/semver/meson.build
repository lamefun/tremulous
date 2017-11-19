semver_inc = include_directories( 'src/include' )

semver_lib = static_library(
  'semver',
  'src/lib/semantic_version_v1.cpp',
  'src/lib/semantic_version_v2.cpp',
  include_directories : semver_inc
)

semver_dep = declare_dependency(
  include_directories : semver_inc,
  link_with : semver_lib
)
