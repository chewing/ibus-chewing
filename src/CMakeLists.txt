SET(IBUS_ENGINE_CHEWING ibus-engine-chewing)
SET(IBUS_SETUP_CHEWING ibus-setup-chewing)
SET(IBUS_CHEWING_SCHEMAS ${CMAKE_CURRENT_BINARY_DIR}/ibus-chewing.schemas
    CACHE INTERNAL "ibus-chewing.schemas")

# Location of library include files
SET(DEPENDENCIES CHEWING GCONF2 GLIB2 GTK2 IBUS X11 XTST)
FOREACH(_d ${DEPENDENCIES})
    INCLUDE_DIRECTORIES(${${_d}_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	)
    LINK_DIRECTORIES(${${_d}_LIBRARY_DIRS})
    LIST(APPEND _ibus_chewing_target_link_libraries ${${_d}_LIBRARIES})
ENDFOREACH(_d ${DEPENDENCIES})

#==================================================================
# Sources
#
SET(GOB2_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/")
MACRO(GOB_GENERATE name gobFile)
    LIST(APPEND GOB_GENERATED_C_${name} 
	${GOB2_OUTPUT_DIR}${name}.c)
    LIST(APPEND GOB_GENERATED_${name} 
	${GOB2_OUTPUT_DIR}${name}.c)
    LIST(APPEND GOB_GENERATED_${name} 
	${GOB2_OUTPUT_DIR}${name}.h)
    LIST(APPEND GOB_GENERATED_${name} 
	${GOB2_OUTPUT_DIR}${name}-private.h)

    ADD_CUSTOM_COMMAND(OUTPUT ${GOB_GENERATED_${name}}
	COMMAND ${GOB2_EXECUTABLE} "${gobFile}" 
	-o "${GOB2_OUTPUT_DIR}"
	DEPENDS ${gobFile} ${ARGN}
	)
ENDMACRO(GOB_GENERATE name gobFile)

GOB_GENERATE(ibus-chewing-engine ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingEngine.gob 
    ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingEngine-def.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingEngine-input-events.c
    )
GOB_GENERATE(maker-dialog ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialog.gob 
    ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialog-def.c
    )

SET_SOURCE_FILES_PROPERTIES(${GOB_GENERATED_ibus-chewing-engine} 
    ${GOB_GENERATED_maker-dialog} PROPERTIES GENERATED true
    )

SET(IBUS_CHEWING_SOURCE_COMMON_C
    ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingConfig.c
    ${CMAKE_CURRENT_SOURCE_DIR}/IBusConfigBackend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialogBackend.c
    ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialogProperty.c
    ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialogUtil.c
    CACHE INTERNAL "IBUS_CHEWING_SOURCE_COMMON_C"
    )

SET(IBUS_CHEWING_SOURCE_COMMON
    ${IBUS_CHEWING_SOURCE_COMMON_C}
    ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingConfig.h
    ${CMAKE_CURRENT_SOURCE_DIR}/IBusConfigBackend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ibus-chewing-util.h 
    ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialogBackend.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialogProperty.h
    ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialogUtil.h
    ${GOB2_OUTPUT_DIR}ibus-chewing-engine.h
    )

ADD_CUSTOM_TARGET(gob2
    DEPENDS ${GOB_GENERATED_ibus-chewing-engine} 
    ${GOB_GENERATED_maker-dialog}
    COMMENT "Preprocess with gob2."
    )


ADD_EXECUTABLE(${IBUS_ENGINE_CHEWING}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingApplier.c
    ${IBUS_CHEWING_SOURCE_COMMON}
    ${GOB_GENERATED_ibus-chewing-engine}
    ${GOB_GENERATED_maker-dialog}
    )

ADD_EXECUTABLE(${IBUS_SETUP_CHEWING}
    ${IBUS_CHEWING_SOURCE_COMMON}
    ${GOB_GENERATED_maker-dialog}
    )

ADD_DEFINITIONS(-DMKDG_SPEC_ONLY)
ADD_EXECUTABLE(generate-gconf-schemas
    ${CMAKE_CURRENT_SOURCE_DIR}/generate-gconf-schemas.c
    ${IBUS_CHEWING_SOURCE_COMMON}
    ${GOB2_OUTPUT_DIR}ibus-chewing-engine.h
    ${GOB2_OUTPUT_DIR}maker-dialog.h
    )

# Link the executable to the library.
TARGET_LINK_LIBRARIES(${IBUS_ENGINE_CHEWING}
    ${_ibus_chewing_target_link_libraries}
    )

TARGET_LINK_LIBRARIES(${IBUS_SETUP_CHEWING}
    ${_ibus_chewing_target_link_libraries}
    )

TARGET_LINK_LIBRARIES(generate-gconf-schemas 
    ${IBUS_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${CHEWING_LIBRARIES}
    )

IF(NOT DEFINED LIB_INSTALL_DIR)
    SET(LIB_INSTALL_DIR "lib${IS_64}")
ENDIF()
SET_COMPILE_ENV(LIB_INSTALL_DIR  "${LIB_INSTALL_DIR}"
    DISPLAY STRING "Chewing library directory")

FILE(GLOB poFiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/../po" "../po/*.po")
STRING(REPLACE ".po" ""  TRANSLATED "${poFiles}")
M_MSG(${M_INFO2} "TRANSLATED=${TRANSLATED}")


ADD_CUSTOM_COMMAND(TARGET generate-gconf-schemas POST_BUILD
    COMMAND G_MESSAGES_DEBUG=all ${CMAKE_CURRENT_BINARY_DIR}/generate-gconf-schemas -v 5 -l
    "C;${TRANSLATED}" ${IBUS_CHEWING_SCHEMAS}
    COMMENT "Generating gconf-schemas"
    VERBATIM
    )

# Schemas
INCLUDE(ManageGConf)
MANAGE_GCONF_SCHEMAS()


MANAGE_FILE_INSTALL(TARGETS ${IBUS_ENGINE_CHEWING} ARGS
    LIBEXEC DESTINATION ${LIBEXEC_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    )


