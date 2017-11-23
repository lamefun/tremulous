add_executable( tremulous_server
  ${SERVER_SOURCES}
  ${COMMON_SOURCES}
  ${NULL_SOURCES} )

target_include_directories( tremulous_server PRIVATE
  ${TREMULOUS_INCLUDE_DIRECTORIES} )

target_compile_definitions( tremulous_server PRIVATE
  ${TREMULOUS_COMPILE_DEFINITIONS}
  DEDICATED )

target_link_libraries( tremulous_server ${COMMON_LINK_LIBRARIES} )

install( TARGETS tremulous_server
  RUNTIME DESTINATION "${GAME_EXE_INSTALL_DIR}" )
