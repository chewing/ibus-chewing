test_c_args = ['-DUNIT_TEST']
inc = include_directories('../src', '../src/setup')

chewing_test_common_src = ['../src/ibus-chewing-lookup-table.c']

chewing_test_common_dep = [chewing_dep, glib_dep, ibus_dep]

chewing_test_common = static_library(
    'chewing-test-common',
    chewing_test_common_src,
    dependencies: chewing_test_common_dep,
    include_directories: inc,
)

# ==================
ibus_chewing_util_test_src = [
    'ibus-chewing-util-test.c',
    '../src/ibus-chewing-util.c',
]
ibus_chewing_util_test = executable(
    'ibus-chewing-util-test',
    ibus_chewing_util_test_src,
    dependencies: chewing_test_common_dep,
    link_with: chewing_test_common,
    c_args: test_c_args,
    include_directories: inc,
)
test(
    'ibus-chewing-util',
    ibus_chewing_util_test,
    env: [
        'GSETTINGS_SCHEMA_DIR=@0@'.format(
            meson.project_build_root() / 'src/setup',
        ),
    ],
    protocol: 'tap',
)

# ==================
ibus_chewing_preedit_test_src = [
    'ibus-chewing-preedit-test.c',
    '../src/ibus-chewing-engine.c',
    '../src/ibus-chewing-util.c',
    '../src/ibus-chewing-preedit.c',
]
ibus_chewing_preedit_test = executable(
    'ibus-chewing-preedit-test',
    ibus_chewing_preedit_test_src,
    dependencies: chewing_test_common_dep + [gtk_dep],
    link_with: chewing_test_common,
    c_args: test_c_args,
    include_directories: inc,
)
test(
    'ibus-chewing-preedit',
    ibus_chewing_preedit_test,
    env: [
        'GSETTINGS_SCHEMA_DIR=@0@'.format(
            meson.project_build_root() / 'src/setup',
        ),
    ],
    protocol: 'tap',
)

# ==================
ibus_chewing_engine_test_src = [
    'ibus-chewing-engine-test.c',
    '../src/ibus-chewing-engine.c',
    '../src/ibus-chewing-lookup-table.c',
    '../src/ibus-chewing-preedit.c',
    '../src/ibus-chewing-util.c',
]
ibus_chewing_engine_test = executable(
    'ibus-chewing-engine-test',
    ibus_chewing_engine_test_src,
    dependencies: chewing_test_common_dep + [gtk_dep],
    link_with: chewing_test_common,
    c_args: test_c_args,
    include_directories: inc,
)
test(
    'ibus-chewing-engine',
    ibus_chewing_engine_test,
    env: [
        'GSETTINGS_SCHEMA_DIR=@0@'.format(
            meson.project_build_root() / 'src/setup',
        ),
    ],
    protocol: 'tap',
)

test(
    'ibus-setup-chewing',
    ibus_setup_chewing,
    args: '-q',
    env: [
        'GSETTINGS_SCHEMA_DIR=@0@'.format(
            meson.project_build_root() / 'src/setup',
        ),
        'GSETTINGS_BACKEND=memory',
    ],
    is_parallel: false,
)

test(
    'ibus-setup-chewing-about',
    ibus_setup_chewing,
    args: ['--about', '-q'],
    env: [
        'GSETTINGS_SCHEMA_DIR=@0@'.format(
            meson.project_build_root() / 'src/setup',
        ),
        'GSETTINGS_BACKEND=memory',
    ],
    is_parallel: false,
)
