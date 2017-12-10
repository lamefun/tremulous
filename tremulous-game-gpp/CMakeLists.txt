cmake_minimum_required( VERSION 3.6 )
project( tremulous-game-gpp )

find_package( Tremulous COMPONENTS PK3 SDK REQUIRED )

add_subdirectory( data )
add_subdirectory( src )
