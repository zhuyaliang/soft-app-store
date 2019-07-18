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
]

executable('soft-app-store',
  sources : sources,
  dependencies : [gtk_deps,pkgkit_deps],
  include_directories: top_srcdir,
  install : true,
  install_dir : get_option('bindir')
  )
