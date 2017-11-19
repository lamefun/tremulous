restclient_inc = [ include_directories( '.' ), root_srcdir_inc ]

restclient_deps = [ libcurl_dep ]

restclient_lib = static_library(
  'restclient',
  'connection.cpp',
  'helpers.cpp',
  'restclient.cpp',
  include_directories : restclient_inc,
  dependencies : restclient_deps
)

restclient_dep = declare_dependency(
  include_directories : restclient_inc,
  link_with : restclient_lib,
  dependencies : restclient_deps
)
