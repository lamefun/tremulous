add_library( script_api STATIC
  lnettlelib.c
  rapidjson/document.cpp
  rapidjson/rapidjson.cpp
  rapidjson/schema.cpp
  rapidjson/values.cpp
)

target_link_libraries( script_api
  ${LUA_LIB_TARGETS}
  ${SOL_LIB_TARGETS}
  ${RAPIDJSON_LIB_TARGETS}
  ${RESTCLIENT_LIB_TARGETS}
  ${NETTLE_LIB_TARGETS}
)
