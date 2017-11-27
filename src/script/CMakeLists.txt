add_library( script_api STATIC
  lnettlelib.c
  rapidjson/document.cpp
  rapidjson/rapidjson.cpp
  rapidjson/schema.cpp
  rapidjson/values.cpp )

target_link_libraries( script_api
  DEPS::lua
  DEPS::sol
  DEPS::rapidjson
  DEPS::restclient
  DEPS::nettle )
