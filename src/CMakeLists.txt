add_library(common STATIC
    IBusChewingApplier.c
    IBusChewingProperties.c
    IBusChewingLookupTable.c
    IBusConfigBackend.c
    MakerDialogBackend.c
    MakerDialogProperty.c
    MakerDialogUtil.c

    IBusChewingLookupTable.h
    IBusChewingProperties.h
    IBusConfigBackend.h
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
    maker-dialog.c
    maker-dialog.h
    maker-dialog-private.h
    IBusConfigBackend.c
    IBusConfigBackend.h
    MakerDialogWidget.c
    MakerDialogWidget.h
)
if(GSETTINGS_SUPPORT)
    target_sources(gui-core PRIVATE
        GSettingsBackend.c
        GSettingsBackend.h
	)
endif()
target_link_libraries(gui-core PUBLIC
    common
    PkgConfig::GLIB2
    PkgConfig::GTK3
    PkgConfig::X11
    PkgConfig::IBUS
    PkgConfig::CHEWING
)

add_executable(ibus-setup-chewing ibus-setup-chewing.c)
target_link_libraries(ibus-setup-chewing gui-core)

add_executable(ibus-engine-chewing
    ibus-chewing-engine.c
    ibus-chewing-engine.h
    IBusChewingEngine-signal.c
    IBusChewingEngine-input-events.c
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

if(GSETTINGS_SUPPORT)
    set(PROJECT_GSCHEMA_XML ${PROJECT_SCHEMA_ID}.gschema.xml)
    set(GSETTINGS_SCHEMAS_DIR ${CMAKE_INSTALL_DATADIR}/glib-2.0/schemas)

    add_executable(generate-gsettings-schemas
        GSettingsBackend.c
        GSettingsBackend.h
        generate-gsettings-schemas.c
	)
    target_link_libraries(generate-gsettings-schemas common)

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_GSCHEMA_XML}
	    DESTINATION ${GSETTINGS_SCHEMAS_DIR}
	)

    add_custom_target(schemas_gsettings ALL
        COMMAND G_MESSAGES_DEBUG=all "${CMAKE_CURRENT_BINARY_DIR}/generate-gsettings-schemas" -v 5 "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_GSCHEMA_XML}"
        COMMAND G_MESSAGES_DEBUG=all glib-compile-schemas "${CMAKE_CURRENT_BINARY_DIR}"
        COMMENT "Generating gsettings schemas ${PROJECT_GSCHEMA_XML}"
        VERBATIM
	)

    add_dependencies(schemas_gsettings generate-gsettings-schemas)
endif()

install(TARGETS ibus-engine-chewing ibus-setup-chewing
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR})