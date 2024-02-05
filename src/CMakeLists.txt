add_library(common STATIC
    IBusChewingApplier.c
    IBusChewingProperties.c
    IBusChewingLookupTable.c
    MakerDialogBackend.c
    MakerDialogProperty.c
    MakerDialogUtil.c

    IBusChewingLookupTable.h
    IBusChewingProperties.h
    MakerDialogBackend.h
    MakerDialogProperty.h
    MakerDialogPropertySpec.h
    MakerDialogUtil.h
    ibus-chewing-engine.h
)
target_link_libraries(common PUBLIC
    PkgConfig::CHEWING
    PkgConfig::GLIB2
    PkgConfig::IBUS
)

add_library(gui-core STATIC
    GSettingsBackend.c
    GSettingsBackend.h
)
target_link_libraries(gui-core PUBLIC
    common
    PkgConfig::GLIB2
    PkgConfig::GTK4
    PkgConfig::LIBADWAITA
    PkgConfig::X11
    PkgConfig::IBUS
    PkgConfig::CHEWING
)

add_custom_command(
    OUTPUT ibus-setup-chewing-window-ui.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/setup
    COMMAND ${GLIB_COMPILE_RESOURCES}
    ARGS
        --generate
        --target=${CMAKE_CURRENT_BINARY_DIR}/ibus-setup-chewing-window-ui.c
        ibus-setup-chewing.gresource.xml
    VERBATIM
    MAIN_DEPENDENCY setup/ibus-setup-chewing.gresource.xml
    DEPENDS
        setup/ibus-setup-chewing-window.ui
)
set_source_files_properties(
    ${CMAKE_CURRENT_BINARY_DIR}/ibus-setup-chewing-window-ui.c
    PROPERTIES GENERATED TRUE
)
add_executable(ibus-setup-chewing
    setup/ibus-setup-chewing-application.c
    setup/ibus-setup-chewing-application.h
    setup/ibus-setup-chewing-window.c
    setup/ibus-setup-chewing-window.h
    setup/main.c
    ${CMAKE_CURRENT_BINARY_DIR}/ibus-setup-chewing-window-ui.c
)
target_compile_definitions(ibus-setup-chewing PRIVATE
    GETTEXT_PACKAGE="ibus-setup-chewing"
    LOCALEDIR="${CMAKE_INSTALL_LOCALEDIR}"
)
target_include_directories(ibus-setup-chewing PRIVATE setup)
target_link_libraries(ibus-setup-chewing PkgConfig::LIBADWAITA)

add_executable(ibus-engine-chewing
    ibus-chewing-engine.c
    ibus-chewing-engine.h
    IBusChewingLookupTable.c
    IBusChewingLookupTable.h
    IBusChewingPreEdit.c
    IBusChewingPreEdit.h
    IBusChewingUtil.c
    IBusChewingUtil.h
    main.c
)
target_compile_definitions(ibus-engine-chewing
    PRIVATE VERBOSE_LEVEL=$<IF:$<CONFIG:Debug>,DEBUG,MSG>)
target_include_directories(ibus-engine-chewing
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ibus-engine-chewing gui-core)

set(PROJECT_GSCHEMA_XML ${PROJECT_SCHEMA_ID}.gschema.xml)
set(GSETTINGS_SCHEMAS_DIR ${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas)

install(FILES setup/${PROJECT_GSCHEMA_XML}
    DESTINATION ${GSETTINGS_SCHEMAS_DIR}
)

install(TARGETS ibus-engine-chewing ibus-setup-chewing
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})