list_append_prefixed( COMMON_SOURCES
  PREFIX "${CMAKE_CURRENT_LIST_DIR}/"
  cm_load.cpp
  cm_patch.cpp
  cm_polylib.cpp
  cmd.cpp
  md5.cpp
  parse.cpp
  q_math.c
  q_shared.c
  vm_interpreted.cpp
  vm_x86.cpp
  cm_test.cpp
  cm_trace.cpp
  common.cpp
  crypto.cpp
  cvar.cpp
  files.cpp
  huffman.cpp
  ioapi.cpp
  md4.cpp
  msg.cpp
  net_chan.cpp
  net_ip.cpp
  puff.cpp
  q3_lauxlib.cpp
  unzip.cpp
  vm.cpp )

list_append_prefixed( COMMON_QVM_SOURCES
  PREFIX "${CMAKE_CURRENT_LIST_DIR}/"
  q_math.c
  q_shared.c )
