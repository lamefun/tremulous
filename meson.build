# =================
# Basic Definitions
# =================

# Project definition.
project( 'tremulous', 'c', 'cpp', 
         version : '1.3.0',
         default_options : [ 'cpp_std=c++14', 'c_std=c99' ] )

# Toplevel 'src' directory.
root_srcdir_inc = include_directories( 'src' )

# Change these when working on a mod.
game_mod_name = 'gpp'
game_pk3_name = 'tremulous'
game_version = meson.project_version( )

# Platform convenience variables. Currently we assume that everything that is
# not Windows is a UNIX-like platform.
platform_windows = target_machine.system( ) == 'windows'
platform_unix = not platform_windows

# C compiler flags for Tremulous code.
tremulous_cflags = [
  '-DUSE_LOCAL_HEADERS',
  '-DUSE_RESTCLIENT',
  '-DUSE_OPENAL',
  '-DUSE_RENDERER_DLOPEN',
  '-DPRODUCT_VERSION="' + meson.project_version( ) + '"',
  '-DGAMEMOD="' + game_mod_name + '"'
]


# ============
# Dependencies
# ============

cc = meson.get_compiler( 'c' )

i18n = import( 'i18n' )

python3 = import( 'python3' )
python3_prog = python3.find_python( )

lua_dep = dependency( 'lua', required : false )
if not lua_dep.found( )
  subdir( 'external/lua-5.3.3' )
endif

libjpeg_dep = dependency( 'libjpeg', required : false )
if not libjpeg_dep.found( )
  subdir( 'external/jpeg-8c' )
  tremulous_cflags += '-DUSE_INTERNAL_JPEG'
endif

dl_dep = cc.find_library( 'dl', required : false )
m_dep = cc.find_library( 'm', required : false )
thread_dep = dependency('threads')

gmp_lib = cc.find_library( 'gmp' )
zlib_dep = dependency( 'zlib' )
libcurl_dep = dependency( 'libcurl' )

if get_option( 'build_client' )
  sdl2_dep = dependency( 'sdl2' )
  gl_dep = dependency( 'gl' )
  vorbisfile_dep = dependency( 'vorbisfile' )
  openal_dep = dependency( 'openal' )

  opusfile_dep = dependency( 'opusfile', required : false )
  if opusfile_dep.found( )
    tremulous_cflags += '-DUSE_CODEC_OPUS'
  endif
endif

subdir( 'external/rapidjson' )
subdir( 'external/nettle-3.3' )
subdir( 'external/restclient' )
subdir( 'external/semver' )
subdir( 'external/sol' )


# ========================
# Installation Directories
# ========================

# Installation prefixes:
# game_exe_dir - Where to put game programs.
# tool_exe_dir - Where to put additional tools.
# base_assets_dir - General assets (maps).
# game_assets_dir - Game assets.
# dynlib_dir - Where to put loadable dlls.
if get_option( 'portable' )
  game_exe_dir = ''
  tool_exe_dir = 'tools'
  base_assets_dir = 'base'
  game_assets_dir = game_mod_name
  dynlib_dir = ''
  doc_dir = 'docs'
else
  game_exe_dir = get_option( 'bindir' )
  tool_exe_dir = get_option( 'bindir' )
  data_dir = join_paths( get_option( 'datadir' ), 'tremulous' )
  data_dir_abs = join_paths( get_option( 'prefix' ), data_dir )
  base_assets_dir = join_paths( data_dir, 'gpp' )
  game_assets_dir = join_paths( data_dir, game_mod_name )
  dynlib_dir = join_paths( get_option( 'libdir' ), 'tremulous' )
  dynlib_dir_abs = join_paths( get_option( 'prefix' ), dynlib_dir )
  doc_dir = join_paths( get_option( 'datadir' ), 'doc', 'tremulous' )
  tremulous_cflags += '-DSYS_DYNLIB_DIR="' + dynlib_dir_abs + '"'
  tremulous_cflags += '-DFS_DEFAULT_BASEPATH="' + data_dir_abs + '"'
endif


# ===============
# Game Components
# ===============

# Tools for building QVMs.
subdir( 'src/tools/lcc' )
subdir( 'src/tools/asm' )

# Common code for the client and the server.
subdir( 'src/asm' )
subdir( 'src/qcommon' )
subdir( 'src/sys' )

# Scripting API.
subdir( 'src/script' )

# Stub functions for the server.
subdir( 'src/null' )

# Tremulous server.
subdir( 'src/server' )

# Tremulous client.
if get_option( 'build_client' )
  # Client executable.
  subdir( 'src/sdl' )
  subdir( 'src/client' )
  # Renderer libraries.
  subdir( 'src/renderercommon' )
  subdir( 'src/renderergl1' )
  subdir( 'src/renderergl2' )
endif

# Game mod.
if get_option( 'build_game' )
  # Extra q3lcc arguments.
  game_q3lcc_args = [ '-I..', '-DQ3_VM' ]
  # Build game logic.
  subdir( 'src/game' )
  subdir( 'src/ui' )
  subdir( 'src/cgame' )
  # Build game data.
  subdir( 'data' )
endif

# Documentation.
subdir( 'docs' )

# Install COPYING.
install_data( 'COPYING', install_dir : doc_dir )
