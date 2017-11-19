dagcheck_c = custom_target(
  'dagcheck_c',
  input : [ 'dagcheck.md' ],
  output : [ 'dagcheck.c' ],
  command : [ lburg_exe, '@INPUT@', '@OUTPUT@' ]
)
