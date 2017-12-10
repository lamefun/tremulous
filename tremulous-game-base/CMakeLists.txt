cmake_minimum_required( VERSION 3.6 )
project( tremulous-game-base )

find_package( Tremulous REQUIRED )

install( FILES
  data/data-1.1.0.pk3
  data/data-gpp1.pk3
  data/map-arachnid2-1.1.0.pk3
  data/map-atcs-1.1.0.pk3
  data/map-karith-1.1.0.pk3
  data/map-nexus6-1.1.0.pk3
  data/map-niveus-1.1.0.pk3
  data/map-transit-1.1.0.pk3
  data/map-tremor-1.1.0.pk3
  data/map-uncreation-1.1.0.pk3
  DESTINATION "${Tremulous_DATA_DIR}/base" )
