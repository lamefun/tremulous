# Install core assets.
subdir( 'core' )

# Install other essential packages.
install_data(
  install_dir : game_assets_dir,
  sources : [
    'assets/data-1.1.0.pk3',
    'assets/data-gpp1.pk3',
  ]
)

# Install built-in maps.
install_data(
  install_dir : base_assets_dir,
  sources : [
    'assets/map-arachnid2-1.1.0.pk3',
    'assets/map-atcs-1.1.0.pk3',
    'assets/map-karith-1.1.0.pk3',
    'assets/map-nexus6-1.1.0.pk3',
    'assets/map-niveus-1.1.0.pk3',
    'assets/map-transit-1.1.0.pk3',
    'assets/map-tremor-1.1.0.pk3',
    'assets/map-uncreation-1.1.0.pk3'
  ]
)

# Install Linux application metadata.
if not get_option( 'portable' ) and target_machine.system( ) == 'linux'
  i18n.merge_file(
    input : 'com.grangerhub.Tremulous.desktop.in.in',
    output : 'com.grangerhub.Tremulous.desktop',
    type : 'desktop',
    po_dir : '../po',
    install : true,
    install_dir : join_paths( get_option( 'datadir' ), 'applications' )
  )

  install_subdir( 'icons', install_dir : get_option( 'datadir' ) )
endif
