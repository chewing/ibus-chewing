add_compile_definitions(UNIT_TEST)
include_directories(${CMAKE_SOURCE_DIR}/src)

# ==================
add_executable(ibus-chewing-util-test ibus-chewing-util-test.c
    ../src/ibus-chewing-util.c
    ../src/ibus-chewing-util.h
    ./test-util.h
)
target_link_libraries(ibus-chewing-util-test common)
add_test(NAME ibus-chewing-util
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/ibus-chewing-util-test)

# ==================
add_executable(ibus-chewing-preedit-test ibus-chewing-preedit-test.c
    ../src/ibus-chewing-engine.c
    ../src/ibus-chewing-util.c
    ../src/ibus-chewing-util.h
    ../src/ibus-chewing-preedit.c
    ../src/ibus-chewing-preedit.h
)
target_link_libraries(ibus-chewing-preedit-test common PkgConfig::GTK4)
add_test(NAME ibus-chewing-preedit
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/ibus-chewing-preedit-test)
set_tests_properties(ibus-chewing-preedit PROPERTIES
    ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_BINARY_DIR}/bin"
)

# ==================
add_executable(ibus-chewing-engine-test ibus-chewing-engine-test.c
    ../src/ibus-chewing-engine.c
    ../src/ibus-chewing-engine.h
    ../src/ibus-chewing-lookup-table.c
    ../src/ibus-chewing-lookup-table.h
    ../src/ibus-chewing-preedit.c
    ../src/ibus-chewing-preedit.h
    ../src/ibus-chewing-util.c
    ../src/ibus-chewing-util.h
)
target_link_libraries(ibus-chewing-engine-test common PkgConfig::GTK4)
add_test(NAME ibus-chewing-engine
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/ibus-chewing-engine-test)
set_tests_properties(ibus-chewing-engine PROPERTIES
    ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_BINARY_DIR}/bin"
)

# ==================
add_test(NAME ibus-setup-chewing
    COMMAND ${CMAKE_BINARY_DIR}/bin/ibus-setup-chewing -q)
set_tests_properties(ibus-setup-chewing PROPERTIES
    ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_BINARY_DIR}/bin"
    ENVIRONMENT "GSETTINGS_BACKEND=memory"
)
add_test(NAME ibus-setup-chewing-about
    COMMAND ${CMAKE_BINARY_DIR}/bin/ibus-setup-chewing --about -q)
set_tests_properties(ibus-setup-chewing-about PROPERTIES
    ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_BINARY_DIR}/bin"
    ENVIRONMENT "GSETTINGS_BACKEND=memory"
)
