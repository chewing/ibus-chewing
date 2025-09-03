project(
    'ibus-chewing',
    'c',
    version: '2.1.7',
    meson_version: '>= 0.57.0',
    default_options: ['c_std=c23'],
)

add_project_arguments('-D_XOPEN_SOURCE', language: 'c')

gnome = import('gnome')
i18n = import('i18n')

ibus_chewing_prefix = get_option('prefix')
ibus_chewing_datadir = ibus_chewing_prefix / get_option('datadir')
ibus_chewing_libexecdir = ibus_chewing_prefix / get_option('libexecdir')
ibus_chewing_localedir = ibus_chewing_prefix / get_option('localedir')
ibus_chewing_docdir = ibus_chewing_datadir / 'doc' / 'ibus-chewing'

ibus_component_dir = ibus_chewing_datadir / 'ibus' / 'component'
ibus_chewing_icondir = ibus_chewing_datadir / 'ibus-chewing' / 'icons'
gschema_dir = ibus_chewing_datadir / 'glib-2.0' / 'schemas'

project_id = 'org.freedesktop.IBus.Chewing'

conf = configuration_data()
conf.set('GETTEXT_PACKAGE', 'ibus-chewing')
conf.set('LOCALEDIR', ibus_chewing_localedir)
conf.set('PROJECT_NAME', meson.project_name())
conf.set('PROJECT_SCHEMA_ID', project_id)
conf.set('PROJECT_SCHEMA_DIR', '/desktop/ibus/engine/chewing')
conf.set('PRJ_DATA_DIR', ibus_chewing_datadir / 'ibus-chewing')
conf.set('PRJ_VER', meson.project_version())
conf.set('PRJ_URL', 'https://github.com/chewing/ibus-chewing')
conf.set('LIBEXEC_DIR', ibus_chewing_libexecdir)
conf.set('LICENSE', 'GPLv2+')

ibus_dep = dependency('ibus-1.0', version: '>= 1.3')
chewing_dep = dependency('chewing', version: '>= 0.9.0')
glib_dep = dependency('glib-2.0')
gtk_dep = dependency('gtk4')
libadwaita_dep = dependency('libadwaita-1')

chewing_datadir_real = chewing_dep.get_variable(pkgconfig: 'datadir')
conf.set('CHEWING_DATADIR_REAL', chewing_datadir_real)

if ibus_dep.version().version_compare('>= 1.5.11')
    conf.set('ICON_PROP_KEY', '<icon_prop_key>InputMode</icon_prop_key>')
else
    conf.set(
        'ICON_PROP_KEY',
        '<!-- <icon_prop_key>InputMode</icon_prop_key> -->',
    )
endif

if ibus_dep.version().version_compare('>= 1.3.99')
    conf.set('SYMBOL_XML', '<symbol>&#x9177;</symbol>')
    conf.set('HOTKEYS_XML', '<hotkeys>Super+space</hotkeys>')
    conf.set(
        'SETUP_PROGRAM_XML',
        '<setup>' + ibus_chewing_libexecdir / 'ibus-setup-chewing' + '</setup>',
    )
else
    conf.set('SYMBOL_XML', '<!-- <symbol>&#x9177;</symbol> -->')
    conf.set('HOTKEYS_XML', '<!-- <hotkeys>Super+space</hotkeys> -->')
    conf.set(
        'SETUP_PROGRAM_XML',
        '<!-- <setup>' + ibus_chewing_libexecdir / 'ibus-setup-chewing' + '</setup> -->',
    )
endif

add_project_arguments(
    '-DPROJECT_NAME=' + conf.get('PROJECT_NAME'),
    '-DPROJECT_SCHEMA_ID=' + conf.get('PROJECT_SCHEMA_ID'),
    # TODO path deprecated
    '-DPROJECT_SCHEMA_BASE=/desktop/ibus/engine',
    '-DPROJECT_SCHEMA_SECTION=chewing',
    '-DPROJECT_SCHEMA_DIR=/desktop/ibus/engine/chewing',
    '-DPROJECT_SCHEMA_PATH=/desktop/ibus/engine/chewing',
    '-DDATA_DIR=' + ibus_chewing_datadir,
    '-DPRJ_DATA_DIR=' + conf.get('PRJ_DATA_DIR'),
    '-DPRJ_VER=' + conf.get('PRJ_VER'),
    '-DPRJ_URL=' + conf.get('PRJ_URL'),
    '-DIBUS_VERSION=' + ibus_dep.version(),
    '-DCHEWING_VERSION=' + chewing_dep.version(),
    '-DLIBEXEC_DIR=' + conf.get('LIBEXEC_DIR'),
    '-DCHEWING_DATADIR_REAL=' + conf.get('CHEWING_DATADIR_REAL'),
    language: 'c',
)

configure_file(
    input: 'data/chewing.xml.in',
    output: 'chewing.xml',
    configuration: conf,
    install: true,
    install_dir: ibus_component_dir,
)

configure_file(
    input: 'data/ibus-setup-chewing.desktop.in',
    output: 'ibus-setup-chewing.desktop',
    configuration: conf,
    install: true,
    install_dir: ibus_chewing_datadir / 'applications',
)

install_data(
    'AUTHORS',
    'README.md',
    'CHANGELOG.md',
    'ChangeLog-1.x',
    'COPYING',
    'USER-GUIDE',
    install_dir: ibus_chewing_docdir,
)

install_data(
    'icons/ibus-chewing.png',
    'icons/ibus-setup-chewing.png',
    'icons/org.freedesktop.IBus.Chewing.Setup.svg',
    install_dir: ibus_chewing_icondir,
)

install_data(
    'icons/org.freedesktop.IBus.Chewing.Setup.svg',
    install_dir: ibus_chewing_datadir / 'icons' / 'hicolor' / 'scalable' / 'apps',
)

subdir('po')
subdir('src')
subdir('test')
