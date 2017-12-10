# CMake Package

## Usage

~~~
find_package( Tremulous [COMPONENTS <components>] [REQUIRED] )
~~~

Basic variables:

- `Tremulous_FOUND` - whether Tremulous was found.

Engine version:

- `Tremulous_VERSION_STRING` - version of the Tremulous engine.
- `Tremulous_VERSION_MAJOR` - the major version of the engine.
- `Tremulous_VERSION_MINOR` - the minor version of the engine.
- `Tremulous_VERSION_PATCH` - the patch version of the engine.
- `Tremulous_VERSION_EXTRA` - version suffix (eg. `-beta.1`).

Full paths to installation directories:

- `Tremulous_EXE_DIR` - contains primary programs.
- `Tremulous_DLL_DIR` - contains plugins.
- `Tremulous_TOOLS_DIR` - contains tools.
- `Tremulous_INCLUDE_DIR` - contains C++ headers.
- `Tremulous_DATA_DIR` - contains game data.
- `Tremulous_DOCS_DIR` - contains documentation.


## Components

### `CLIENT`

Tremulous client.

- `Tremulous_CLIENT_FOUND` - whether the client was found.
- `Tremulous_CLIENT` - full path to the client.

### `SERVER`

Tremulous server.

- `Tremulous_SERVER_FOUND` - whether the server was found.
- `Tremulous_SERVER` - full path to the server.

### `PK3`

Package creation tools.

- `Tremulous_PK3_FOUND` - whether the package creation tools were found.
- `Tremulous_ZIPTOOL` - full path to `ziptool`.

### `SDK`

Tremulous SDK. Allows building mods.

- `Tremulous_SDK_FOUND` - whether the game logic SDK was found.
- `Tremulous_Q3LCC` - full path to `q3lcc`.
- `Tremulous_Q3ASM` - full path to `q3asm`.


## Functions

### `add_tremulous_pk3`

Builds a `.pk3` package.

~~~
add_tremulous_pk3(
  <target-name>
  OUTPUT <pk3-name>
  [FROM <dir>]
  [FILES <files>]
  [FILE <path> INTO <dir-in-pk3>]
  [FILE <path> AS <path-in-pk3>] )
~~~

`OUTPUT` specifies the full file name of the package (eg. `my-data.pk3`).

`FROM` specifies which directory to package files from.

`FILES` specifies which files to package from the specified directory. Only
relative paths are allowed.

`FILE` puts a single file into the package

### `add_tremulous_game`

Build a Tremulous mod. Requires the `SDK` component.

~~~
add_tremulous_game(
  <target-name>
  [NO_11_COMPATIBILITY]
  [GPP_PK3 <pk3-name>]
  [V11_PK3 <pk3-name>]
  [DESTINATION <pk3-destination>]
  [INCLUDE_DIRECTORIES <dirs>]
  GAME_SOURCES <sources>
  CGAME_SOURCES <sources>
  UI_SOURCES <sources> )
~~~
