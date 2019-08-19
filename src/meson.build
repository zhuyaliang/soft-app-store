gnome = import('gnome')

sources = [
'main.c',
'app-store-local.c',  
'app-store-store.c', 
'app-store-update.c',
'app-store-util.c',
'app-store-category.c',
'app-store-pkgkit.c',
'app-store-thumbnail.c',
'app-store-row.c',
'app-store-details.c',
'app-store-search.c',
'app-store-screenshot.c',
'app-store-message.c',
]

executable('soft-app-store',
  sources : sources,
  dependencies : [gtk_deps,pkgkit_deps,appstream_deps,libsoup_deps,jsonc_deps],
  include_directories: top_srcdir,
  install : true,
  install_dir : get_option('bindir')
  )
