set( UI_SOURCES
  ui_main.c # Must be listed first!
  ui_atoms.c
  ui_gameinfo.c
  ${BGAME_SOURCES}
  ${UI_SHARED_SOURCES}
)

add_library( ui SHARED ${UI_SOURCES} ui_syscalls.c )

target_include_directories( ui PRIVATE ${TREMULOUS_INCLUDE_DIRECTORIES} )

target_compile_definitions( ui PRIVATE
  ${TREMULOUS_COMPILE_DEFINITIONS}
  UI )

set_target_properties( ui PROPERTIES PREFIX "" )

install( TARGETS ui LIBRARY DESTINATION "${SHARED_MODULE_INSTALL_DIR}" )

build_qvm( ui_qvm
  NATIVE ui
  OUTPUT ui.qvm
  SOURCES ${UI_SOURCES} ${COMMON_QVM_SOURCES} ui_syscalls.asm
  INCLUDE_DIRECTORIES ..
  COMPILE_DEFINITIONS ${TREMULOUS_COMPILE_DEFINITIONS} UI )

build_qvm( ui_11_qvm
  NATIVE ui
  OUTPUT ui_11.qvm
  SOURCES ${UI_SOURCES} ${COMMON_QVM_SOURCES} ui_syscalls_11.asm
  INCLUDE_DIRECTORIES ..
  COMPILE_DEFINITIONS ${TREMULOUS_COMPILE_DEFINITIONS} MODULE_INTERFACE_11 UI )
