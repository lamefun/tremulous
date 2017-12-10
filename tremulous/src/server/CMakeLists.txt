add_executable( tremulous-server
  ${ASM_SOURCES}
  ${QCOMMON_SOURCES}
  ${SYS_SOURCES}
  ${SERVER_SOURCES}
  ${NULL_SOURCES} )

target_include_directories( tremulous-server PRIVATE
  ${COMMON_INCLUDE_DIRECTORIES} )

target_compile_definitions( tremulous-server PRIVATE
  ${COMMON_COMPILE_DEFINITIONS}
  DEDICATED )

target_link_libraries( tremulous-server
  ${COMMON_LINK_LIBRARIES} )
