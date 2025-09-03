gnome = import('gnome')

ibus_setup_chewing_gresource = gnome.compile_resources(
    'ibus-setup-chewing-resources',
    'ibus-setup-chewing.gresource.xml',
)

ibus_setup_chewing = executable(
    'ibus-setup-chewing',
    'main.c',
    'ibus-setup-chewing-application.c',
    'ibus-setup-chewing-window.c',
    'ibus-setup-chewing-about.c',
    ibus_setup_chewing_gresource,
    dependencies: [libadwaita_dep, chewing_dep],
    c_args: [
        '-DGETTEXT_PACKAGE="ibus-chewing"',
        '-DLOCALEDIR="' + ibus_chewing_localedir + '"',
    ],
    install: true,
    install_dir: ibus_chewing_libexecdir,
)

gsettings_schemas = project_id + '.gschema.xml'
install_data(gsettings_schemas, install_dir: gschema_dir)
gnome.compile_schemas()
gnome.post_install(glib_compile_schemas: true)
