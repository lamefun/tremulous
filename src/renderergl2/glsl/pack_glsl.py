#!/usr/bin/env python3
import sys
import os.path

in_file = sys.argv[ 1 ]
out_file = sys.argv[ 2 ]

with open( in_file, 'rb' ) as f:
    glsl = f.read()

with open( out_file, 'w' ) as f:
    template = 'const char *fallbackShader_{} = "{}";\n'
    name = os.path.basename( in_file ).split( '.' )[ 0 ]
    string = ''.join( [ '\\x{:02x}'.format( b ) for b in glsl ] )
    f.write( template.format( name, string ) )
