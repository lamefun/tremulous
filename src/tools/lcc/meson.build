lburg_exe = executable(
  'lburg',
  'lburg/lburg.c',
  'lburg/gram.c',
  include_directories : include_directories( 'lburg' ),
  install : false
)

# Generate dagcheck_c with lburg.
subdir( 'src' )

q3cpp_exe = executable(
  'tremulous_q3cpp',
  'cpp/cpp.c',
  'cpp/lex.c',
  'cpp/nlist.c',
  'cpp/tokens.c',
  'cpp/macro.c',
  'cpp/eval.c',
  'cpp/include.c',
  'cpp/hideset.c',
  'cpp/getopt.c',
  'cpp/unix.c',
  include_directories : include_directories( 'cpp' ),
  install : get_option( 'install_qvm_tools' ),
  install_dir : tool_exe_dir
)

q3rcc_exe = executable(
  'tremulous_q3rcc',
  'src/alloc.c',
  'src/bind.c',
  'src/bytecode.c',
  'src/dag.c',
  dagcheck_c,
  'src/decl.c',
  'src/enode.c',
  'src/error.c',
  'src/event.c',
  'src/expr.c',
  'src/gen.c',
  'src/init.c',
  'src/inits.c',
  'src/input.c',
  'src/lex.c',
  'src/list.c',
  'src/main.c',
  'src/null.c',
  'src/output.c',
  'src/prof.c',
  'src/profio.c',
  'src/simp.c',
  'src/stmt.c',
  'src/string.c',
  'src/sym.c',
  'src/symbolic.c',
  'src/trace.c',
  'src/tree.c',
  'src/types.c',
  include_directories : include_directories( 'src' ),
  install : get_option( 'install_qvm_tools' ),
  install_dir : tool_exe_dir
)

q3lcc_exe = executable(
  'tremulous_q3lcc',
  'etc/bytecode.c',
  'etc/lcc.c',
  include_directories : [ include_directories( 'etc' ), root_srcdir_inc ],
  install : get_option( 'install_qvm_tools' ),
  install_dir : tool_exe_dir
)

q3lcc_gen = generator(
  q3lcc_exe,
  output : '@BASENAME@.asm',
  arguments : [ '@EXTRA_ARGS@', '-o', '@OUTPUT@', '@INPUT@' ]
)
