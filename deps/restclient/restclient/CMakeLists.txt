add_library( restclient STATIC
  connection.cpp
  helpers.cpp
  restclient.cpp
)

target_include_directories( restclient PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${TOP_SRCDIR}"
)

target_link_libraries( restclient DEPS::curl )
