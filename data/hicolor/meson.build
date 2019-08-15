actions_icons = [
  ['16x16', 'soft-app-store.png'],
  ['22x22', 'soft-app-store.png'],
  ['24x24', 'soft-app-store.png'],
  ['32x32', 'soft-app-store.png'],
  ['48x48', 'soft-app-store.png'],
  ['64x64', 'soft-app-store.png']
]

foreach icon: actions_icons
  install_data(
    join_paths(icon[0], 'apps', icon[1]),
    install_dir: join_paths(get_option('prefix'), 'share/icons', 'hicolor', icon[0], 'apps')
  )
endforeach
