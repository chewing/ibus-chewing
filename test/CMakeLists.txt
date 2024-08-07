add_compile_definitions(UNIT_TEST)
include_directories(${CMAKE_SOURCE_DIR}/src)

# ==================
add_executable(MakerDialogUtil-test MakerDialogUtil-test.c)
target_link_libraries(MakerDialogUtil-test common)
add_test(NAME MakerDialogUtil
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/MakerDialogUtil-test)

# ==================
add_executable(IBusChewingUtil-test IBusChewingUtil-test.c
    ../src/IBusChewingUtil.c
    ../src/IBusChewingUtil.h
)
target_link_libraries(IBusChewingUtil-test common)
add_test(NAME IBusChewingUtil
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/IBusChewingUtil-test)

# ==================
add_executable(IBusChewingPreEdit-test IBusChewingPreEdit-test.c
    ../src/ibus-chewing-engine.c
    ../src/IBusChewingUtil.c
    ../src/IBusChewingUtil.h
    ../src/IBusChewingPreEdit.c
    ../src/IBusChewingPreEdit.h
)
target_link_libraries(IBusChewingPreEdit-test common PkgConfig::GTK4)
add_test(NAME IBusChewingPreEdit
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/IBusChewingUtil-test)

# ==================
add_executable(ibus-chewing-engine-test ibus-chewing-engine-test.c
    ../src/ibus-chewing-engine.c
    ../src/ibus-chewing-engine.h
    ../src/IBusChewingLookupTable.c
    ../src/IBusChewingLookupTable.h
    ../src/IBusChewingPreEdit.c
    ../src/IBusChewingPreEdit.h
    ../src/IBusChewingUtil.c
    ../src/IBusChewingUtil.h
)
target_link_libraries(ibus-chewing-engine-test common PkgConfig::GTK4)
add_test(NAME ibus-chewing-engine
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/ibus-chewing-engine-test)
set_tests_properties(ibus-chewing-engine PROPERTIES
    ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_BINARY_DIR}/bin")

# ==================
add_test(NAME ibus-setup-chewing
    COMMAND ${CMAKE_BINARY_DIR}/bin/ibus-setup-chewing -q)
set_tests_properties(ibus-setup-chewing PROPERTIES
    ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_BINARY_DIR}/bin")
add_test(NAME ibus-setup-chewing-about
    COMMAND ${CMAKE_BINARY_DIR}/bin/ibus-setup-chewing --about -q)
set_tests_properties(ibus-setup-chewing-about PROPERTIES
    ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_BINARY_DIR}/bin")
