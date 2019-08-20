subdir('icon')
subdir('hicolor')
subdir('soft-app-store')

i18n = import('i18n')

i18n.merge_file(
  'mate-user-admin-desktop',
  input: 'soft-app-store.desktop.in',
  output: 'soft-app-store.desktop',
  install: true,
  po_dir: join_paths(meson.source_root(), 'po'),
  install_dir : join_paths(get_option('prefix'), 'share/applications'),
  type: 'desktop',
)
