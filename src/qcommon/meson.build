qcommon_deps = [
  m_dep, dl_dep, thread_dep, qasm_dep, zlib_dep, rapidjson_dep, nettle_dep
]

qcommon_src = files(
  'cm_load.cpp',
  'cm_patch.cpp',
  'cm_polylib.cpp',
  'cm_test.cpp',
  'cm_trace.cpp',
  'cmd.cpp',
  'common.cpp',
  'crypto.cpp',
  'cvar.cpp',
  'huffman.cpp',
  'ioapi.cpp',
  'md4.cpp',
  'md5.cpp',
  'msg.cpp',
  'net_chan.cpp',
  'net_ip.cpp',
  'parse.cpp',
  'puff.cpp',
  'q3_lauxlib.cpp',
  'q_math.c',
  'q_shared.c',
  'unzip.cpp',
  'vm.cpp',
  'vm_x86.cpp',
  'vm_interpreted.cpp'
)

# Used by QVMs as well.
qcommon_qvm_src = files( 'q_shared.c', 'q_math.c' )
qcommon_qvm_headers = files( 'q_shared.h' )

# This really belongs to system abstraction.
qcommon_sys_src = files( 'files.cpp' )
