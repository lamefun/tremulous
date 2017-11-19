script_api_lib = static_library(
  'script_api',
  'lnettlelib.c',
  'rapidjson/document.cpp',
  'rapidjson/rapidjson.cpp',
  'rapidjson/schema.cpp',
  'rapidjson/values.cpp',
  include_directories : root_srcdir_inc,
  implicit_include_directories : false,
  dependencies : [
    lua_dep,
    sol_dep,
    rapidjson_dep,
    restclient_dep,
    nettle_dep
  ]
)

script_api_dep = declare_dependency(
  include_directories : root_srcdir_inc,
  link_with : script_api_lib
)
