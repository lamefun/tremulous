#!/usr/bin/env python3

import argparse
import zipfile
import os.path

parser = argparse.ArgumentParser()
parser.add_argument( '--game-qvm' )
parser.add_argument( '--cgame-qvm' )
parser.add_argument( '--ui-qvm' )
parser.add_argument( '--cgame-11-qvm' )
parser.add_argument( '--ui-11-qvm' )
parser.add_argument( '--gpp-vms-pk3' )
parser.add_argument( '--v11-vms-pk3' )

args = parser.parse_args()

with zipfile.ZipFile( args.gpp_vms_pk3, 'w' ) as archive:
    archive.write( args.game_qvm, 'vm/game.qvm' )
    archive.write( args.cgame_qvm, 'vm/cgame.qvm' )
    archive.write( args.ui_qvm, 'vm/ui.qvm' )

with zipfile.ZipFile( args.v11_vms_pk3, 'w' ) as archive:
    archive.write( args.cgame_11_qvm, 'vm/cgame.qvm' )
    archive.write( args.ui_11_qvm, 'vm/ui.qvm' )
