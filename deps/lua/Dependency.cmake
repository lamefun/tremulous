acquire_dependency( lua REQUIRED
  PKG_CONFIG "lua >= 5.2"
  INTERNAL_SOURCE_DIRS deps/lua/lua-5.3.3
  INTERNAL_SOURCE_LIBRARIES lua )
