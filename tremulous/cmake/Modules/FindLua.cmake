# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindLua
# -------
#
# Locates the Lua library.
#
# ::
#
#   Lua::lua - library to link with.
#   Lua_FOUND - TRUE if Lua was found.
#   Lua_LIBRARY - full path to Lua library.
#   Lua_INCLUDE_DIR - where to find Lua headers.

find_package( PkgConfig )
pkg_check_modules( PC_LUA QUIET lua>=5.2 )

find_path( Lua_INCLUDE_DIR lua.h
  HINTS ${PC_LUA_INCLUDE_DIRS} )
find_library( Lua_LIBRARY
  NAMES lua ${PC_LUA_LIBRARIES}
  HINTS ${PC_LUA_LIBRARY_DIRS} )
mark_as_advanced( Lua_LIBRARY Lua_INCLUDE_DIR )

include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Lua REQUIRED_ARGS Lua_LIBRARY Lua_INCLUDE_DIR )

if( Lua_FOUND )
  add_library( Lua::lua UNKNOWN IMPORTED )
  set_target_properties( Lua::lua PROPERTIES
    IMPORTED_INCLUDE_DIRECTORIES "${Lua_INCLUDE_DIR}"
    IMPORTED_LOCATION "${Lua_LIBRARY}" )
endif( )
