include( Assets.cmake )

set( PK3_SUFFIX "${GAME_PK3_NAME}-${PRODUCT_VERSION}.pk3" )
set( DATA_PK3_NAME "data-${PK3_SUFFIX}" )
set( GPP_VMS_PK3_NAME "gpp-vms-${PK3_SUFFIX}" )
set( V11_VMS_PK3_NAME "v11-vms-${PK3_SUFFIX}" )

add_custom_command(
  OUTPUT "${DATA_PK3_NAME}"
  DEPENDS ${ALL_ASSETS}
  COMMAND
    ${PYTHON3_COMMAND} "${CMAKE_CURRENT_SOURCE_DIR}/pack_data.py"
    --assets-dir "${CMAKE_CURRENT_SOURCE_DIR}"
    --data-files ${ALL_ASSETS}
    --data-pk3 "${DATA_PK3_NAME}" )

get_property( GAME_BINARY_DIR
  DIRECTORY "${PROJECT_SOURCE_DIR}/src/game"
  PROPERTY BINARY_DIR )
get_property( CGAME_BINARY_DIR
  DIRECTORY "${PROJECT_SOURCE_DIR}/src/cgame"
  PROPERTY BINARY_DIR )
get_property( UI_BINARY_DIR
  DIRECTORY "${PROJECT_SOURCE_DIR}/src/ui"
  PROPERTY BINARY_DIR )

add_custom_command(
  OUTPUT
    "${GPP_VMS_PK3_NAME}"
    "${V11_VMS_PK3_NAME}"
  DEPENDS
    "${GAME_BINARY_DIR}/game.qvm"
    "${CGAME_BINARY_DIR}/cgame.qvm"
    "${CGAME_BINARY_DIR}/cgame_11.qvm"
    "${UI_BINARY_DIR}/ui.qvm"
    "${UI_BINARY_DIR}/ui_11.qvm"
  COMMAND
    ${PYTHON3_COMMAND} "${CMAKE_CURRENT_SOURCE_DIR}/pack_vms.py"
    --game-qvm "${GAME_BINARY_DIR}/game.qvm"
    --cgame-qvm "${CGAME_BINARY_DIR}/cgame.qvm"
    --cgame-11-qvm "${CGAME_BINARY_DIR}/cgame_11.qvm"
    --ui-qvm "${UI_BINARY_DIR}/ui.qvm"
    --ui-11-qvm "${UI_BINARY_DIR}/ui_11.qvm"
    --gpp-vms-pk3 "${GPP_VMS_PK3_NAME}"
    --v11-vms-pk3 "${V11_VMS_PK3_NAME}" )

add_custom_target( game_pk3s ALL DEPENDS
  "${DATA_PK3_NAME}"
  "${GPP_VMS_PK3_NAME}"
  "${V11_VMS_PK3_NAME}" )

install( FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${DATA_PK3_NAME}"
  "${CMAKE_CURRENT_BINARY_DIR}/${GPP_VMS_PK3_NAME}"
  "${CMAKE_CURRENT_BINARY_DIR}/${V11_VMS_PK3_NAME}"
  DESTINATION "${GAME_DATA_INSTALL_DIR}" )
