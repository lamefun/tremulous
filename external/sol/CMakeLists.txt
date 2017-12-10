# Official website: https://github.com/ThePhD/sol2

add_custom_command(
  DEPENDS sol.hpp
  OUTPUT "${BUILD_PREFIX}/include/sol/sol.hpp"
  COMMAND ${CMAKE_COMMAND} -E make_directory
    "${BUILD_PREFIX}/include/sol"
  COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/sol.hpp"
    "${BUILD_PREFIX}/include/sol/sol.hpp" )

add_custom_target( sol ALL DEPENDS "${BUILD_PREFIX}/include/sol/sol.hpp" )
