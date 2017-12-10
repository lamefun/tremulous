#===============================================================================
#
# Converts GLSL shaders into C sources.
#
# Arguments:
#
#   -Dshader=<shader name>
#   -Dinput=<GLSL file>
#   -Doutput=<C file>
#
# Output format:
#
#   const char *fallbackShader_${shader} = "${hex data}";
#
#===============================================================================

file( READ ${input} content HEX )
string( LENGTH "${content}" length )
math( EXPR num_bytes "${length} / 2" )
math( EXPR max_i "${num_bytes} - 1" )

set( hex_data )
foreach( i RANGE 0 ${max_i} )
  math( EXPR j "${i} * 2" )
  string( SUBSTRING ${content} ${j} 2 hex_byte )
  string( APPEND hex_data "\\x${hex_byte}" )
endforeach( )

set( result "const char *fallbackShader_${shader} = \"${hex_data}\";\n" )
file( WRITE "${output}" "${result}" )
