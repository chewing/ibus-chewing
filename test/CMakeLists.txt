add_compile_definitions(UNIT_TEST)
include_directories(${CMAKE_SOURCE_DIR}/src)

# ==================
add_executable(MakerDialogUtil-test MakerDialogUtil-test.c)
target_link_libraries(MakerDialogUtil-test common)
add_test(NAME MakerDialogUtil
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/MakerDialogUtil-test)

# ==================
add_executable(MakerDialogBackend-test MakerDialogBackend-test.c)
target_link_libraries(MakerDialogBackend-test gui-core)
add_test(NAME MakerDialogBackend
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/MakerDialogBackend-test)
set_tests_properties(MakerDialogBackend PROPERTIES
    ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_BINARY_DIR}/bin")

# ==================
add_executable(IBusChewingUtil-test IBusChewingUtil-test.c
    ../src/IBusChewingUtil.c
    ../src/IBusChewingUtil.h
)
target_link_libraries(IBusChewingUtil-test gui-core)
add_test(NAME IBusChewingUtil
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/IBusChewingUtil-test)

# ==================
add_executable(IBusChewingPreEdit-test IBusChewingPreEdit-test.c
    ../src/IBusChewingUtil.c
    ../src/IBusChewingUtil.h
    ../src/IBusChewingPreEdit.c
    ../src/IBusChewingPreEdit.h
)
target_link_libraries(IBusChewingPreEdit-test gui-core)
add_test(NAME IBusChewingPreEdit
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/IBusChewingUtil-test)

# ==================
add_executable(ibus-chewing-engine-test ibus-chewing-engine-test.c
    ../src/ibus-chewing-engine.c
    ../src/ibus-chewing-engine.h
    ../src/IBusChewingEngine-signal.c
    ../src/IBusChewingEngine-input-events.c
    ../src/IBusChewingLookupTable.c
    ../src/IBusChewingLookupTable.h
    ../src/IBusChewingPreEdit.c
    ../src/IBusChewingPreEdit.h
    ../src/IBusChewingSystray.c
    ../src/IBusChewingSystray.h
    ../src/IBusChewingUtil.c
    ../src/IBusChewingUtil.h
)
target_link_libraries(ibus-chewing-engine-test gui-core)
add_test(NAME ibus-chewing-engine
    COMMAND gtester ${CMAKE_CURRENT_BINARY_DIR}/ibus-chewing-engine-test)
set_tests_properties(ibus-chewing-engine PROPERTIES
    ENVIRONMENT "GSETTINGS_SCHEMA_DIR=${CMAKE_BINARY_DIR}/bin")