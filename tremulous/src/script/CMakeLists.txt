add_library( script_api STATIC
  bind.h
  client.h
  cmd.h
  cvar.h
  http_client.h
  lnettlelib.c
  lnettlelib.h
  nettle.h
  rapidjson/document.cpp
  rapidjson.h
  rapidjson/rapidjson.cpp
  rapidjson/schema.cpp
  rapidjson/values.cpp )

target_include_directories( script_api PUBLIC "${TOP_SRCDIR}" )

target_link_libraries( script_api
  Lua::lua
  Sol::sol
  EXT::RapidJSON
  nettle )
