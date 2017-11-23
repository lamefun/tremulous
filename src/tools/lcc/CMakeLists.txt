add_executable( lburg
  lburg/lburg.c
  lburg/gram.c
)

target_include_directories( lburg PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/lburg"
  "${TOP_SRCDIR}"
)

add_custom_command(
  OUTPUT dagcheck.c
  INPUT "${CMAKE_CURRENT_SOURCE_DIR}/src/dagcheck.md"
  COMMAND lburg
  ARGS "${CMAKE_CURRENT_SOURCE_DIR}/src/dagcheck.md" dagcheck.c
)

set_source_files_properties( dagcheck.c PROPERTIES GENERATED TRUE )

add_executable( q3cpp
  cpp/cpp.c
  cpp/lex.c
  cpp/nlist.c
  cpp/tokens.c
  cpp/macro.c
  cpp/eval.c
  cpp/include.c
  cpp/hideset.c
  cpp/getopt.c
  cpp/unix.c
)

target_include_directories( q3cpp PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/cpp"
  "${TOP_SRCDIR}"
)

add_executable( q3rcc
  src/alloc.c
  src/bind.c
  src/bytecode.c
  src/dag.c
  "${CMAKE_CURRENT_BINARY_DIR}/dagcheck.c"
  src/decl.c
  src/enode.c
  src/error.c
  src/event.c
  src/expr.c
  src/gen.c
  src/init.c
  src/inits.c
  src/input.c
  src/lex.c
  src/list.c
  src/main.c
  src/null.c
  src/output.c
  src/prof.c
  src/profio.c
  src/simp.c
  src/stmt.c
  src/string.c
  src/sym.c
  src/symbolic.c
  src/trace.c
  src/tree.c
  src/types.c
)

target_include_directories( q3rcc PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/src"
  "${TOP_SRCDIR}"
)

add_executable( q3lcc
  etc/bytecode.c
  etc/lcc.c
)

target_include_directories( q3lcc PRIVATE "${TOP_SRCDIR}" )

add_dependencies( q3lcc q3rcc q3cpp )

if( INSTALL_QVM_TOOLCHAIN )
  install(
    TARGETS q3cpp q3rcc q3lcc
    DESTINATION "${QVM_TOOLCHAIN_INSTALL_DIR}"
  )
endif( )
