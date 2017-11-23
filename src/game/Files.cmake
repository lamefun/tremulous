list_append_prefixed( BGAME_SOURCES
  PREFIX "${CMAKE_CURRENT_LIST_DIR}/"
  bg_alloc.c
  bg_lib.c
  bg_misc.c
  bg_voice.c
)

list_append_prefixed( BGAME_PMOVE_SOURCES
  PREFIX "${CMAKE_CURRENT_LIST_DIR}/"
  bg_pmove.c
  bg_slidemove.c
)
