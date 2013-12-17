SET(IBUS_CHEWING_TARGET ibus-engine-chewing)
SET(IBUS_CHEWING_SCHEMAS ${CMAKE_CURRENT_BINARY_DIR}/ibus-chewing.schemas
    CACHE INTERNAL "ibus-chewing.schemas")

# Location of library include files
SET(DEPENDENCIES CHEWING GCONF2 GTK2 IBUS X11 XTST)
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

GOB_GENERATE(ibus-chewing-engine ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingEngine.gob IBusChewingEngine-def.c ibus-chewing-util.h IBusChewingEngine-input-events.c)
GOB_GENERATE(maker-dialog ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialog.gob)

SET_SOURCE_FILES_PROPERTIES(${GOB_GENERATED_ibus-chewing-engine} 
    ${GOB_GENERATED_maker-dialog} PROPERTIES GENERATED true)

ADD_CUSTOM_TARGET(gob2
    DEPENDS ${GOB_GENERATED_ibus-chewing-engine} 
    ${GOB_GENERATED_maker-dialog}
    COMMENT "Preprocess with gob2."
)


# executable built from some source
ADD_EXECUTABLE(${IBUS_CHEWING_TARGET}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${GOB_GENERATED_ibus-chewing-engine}
    ${GOB_GENERATED_maker-dialog}
    )

ADD_EXECUTABLE(generate-gconf-schemas
    ${CMAKE_CURRENT_SOURCE_DIR}/generate-gconf-schemas.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ibus-chewing-util.h
    ${GOB_GENERATED_maker-dialog}
    )

# Link the executable to the library.
TARGET_LINK_LIBRARIES(${IBUS_CHEWING_TARGET}
    ${_ibus_chewing_target_link_libraries}
    )

TARGET_LINK_LIBRARIES(generate-gconf-schemas 
    ${GTK2_LIBRARIES}
    )

IF(NOT DEFINED LIB_INSTALL_DIR)
    SET(LIB_INSTALL_DIR "lib${IS_64}")
ENDIF()
SET_COMPILE_ENV(LIB_INSTALL_DIR  "${LIB_INSTALL_DIR}"
    DISPLAY STRING "Chewing library directory")

ADD_CUSTOM_COMMAND(TARGET generate-gconf-schemas POST_BUILD
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/generate-gconf-schemas -v 3 -l
    "C;${TRANSLATED}" ${IBUS_CHEWING_SCHEMAS}
    COMMENT "Generating gconf-schemas"
    VERBATIM
    )

MANAGE_FILE_INSTALL(TARGETS ${IBUS_CHEWING_TARGET} ARGS
    LIBEXEC DESTINATION ${LIBEXEC_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    )


