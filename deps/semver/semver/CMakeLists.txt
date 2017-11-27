add_library( semver STATIC
  src/lib/semantic_version_v1.cpp
  src/lib/semantic_version_v2.cpp
)

target_include_directories( semver PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/src/include" 
)
