ibus_engine_chewing = executable(
    'ibus-engine-chewing',
    'main.c',
    'ibus-chewing-engine.c',
    'ibus-chewing-lookup-table.c',
    'ibus-chewing-preedit.c',
    'ibus-chewing-util.c',
    dependencies: [ibus_dep, chewing_dep, glib_dep, gtk_dep],
    c_args: ['-DG_LOG_DOMAIN="ibus-chewing"'],
    install: true,
    install_dir: ibus_chewing_libexecdir,
)

subdir('setup')
