#!/usr/bin/env python3

import argparse
import zipfile
import os.path

parser = argparse.ArgumentParser()
parser.add_argument( '--assets-dir' )
parser.add_argument( '--data-files', nargs='*' )
parser.add_argument( '--data-pk3' )

args = parser.parse_args()

with zipfile.ZipFile( args.data_pk3, 'w' ) as archive:
    for name in args.data_files:
        path = os.path.join( args.assets_dir, name )
        archive.write( path, name )
