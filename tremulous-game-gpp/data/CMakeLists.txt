include( Assets.cmake )

set( PK3_NAME "data-tremulous-${Tremulous_VERSION_STRING}.pk3" )

add_tremulous_pk3( gpp_data OUTPUT "${PK3_NAME}" FILES ${ASSETS} )

install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PK3_NAME}"
         DESTINATION "${Tremulous_DATA_DIR}/gpp" )
