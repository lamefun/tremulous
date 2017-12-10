install( FILES Index.md DESTINATION "${INSTDIR_DOCS}" )

install( FILES
  CMake/Tutorial.md
  CMake/Package.md
  DESTINATION "${INSTDIR_DOCS}/CMake" )

install( FILES
  Data/ParticleSystem.md
  Data/TrailSystem.md
  Data/VoiceCommands.md
  DESTINATION "${INSTDIR_DOCS}/Data" )

install( FILES
  Servers/RunningServers.md
  Servers/server.sh.template
  DESTINATION "${INSTDIR_DOCS}/Servers" )

install( FILES
  Renderers/OpenGL2.md
  DESTINATION "${INSTDIR_DOCS}/Renderers" )
