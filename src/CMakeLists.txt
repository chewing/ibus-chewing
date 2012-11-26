# Location of library include files
INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS}
    ${CHEWING_INCLUDE_DIRS} ${GCONF2_INCLUDE_DIRS} ${XTST_INCLUDE_DIRS})

# Library location for the linker
LINK_DIRECTORIES(${GTK2_LIBRARY_DIRS} ${IBUS_LIBRARY_DIRS}
    ${CHEWING_LIBRARY_DIRS} ${GCONF2_LIBRARY_DIRS} ${XTST_LIBRARY_DIRS})

#==================================================================
# Sources
#
SET(IBUS_CHEWING_GOB_GENERATED
    ${CMAKE_CURRENT_SOURCE_DIR}/ibus-chewing-engine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ibus-chewing-engine.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ibus-chewing-engine-private.h
    )

SET(MAKER_DIALOG_GOB_GENERATED
    ${CMAKE_CURRENT_SOURCE_DIR}/maker-dialog.c
    ${CMAKE_CURRENT_SOURCE_DIR}/maker-dialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/maker-dialog-private.h
    )

ADD_CUSTOM_COMMAND(OUTPUT ${IBUS_CHEWING_GOB_GENERATED}
    COMMAND ${GOB2_CMD} ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingEngine.gob -o ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${GOB2_CMD} ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialog.gob -o ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS IBusChewingEngine.gob IBusChewingEngine-def.c ibus-chewing-util.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_COMMAND(OUTPUT ${MAKER_DIALOG_GOB_GENERATED}
    COMMAND ${GOB2_CMD} ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialog.gob -o ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS MakerDialog.gob
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )

SET(GOB_GENERATED
    ${IBUS_CHEWING_GOB_GENERATED} ${MAKER_DIALOG_GOB_GENERATED}
    )

ADD_CUSTOM_TARGET(gob2
    DEPENDS ${GOB_GENERATED}
      ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingEngine-input-events.c
    COMMENT "Preprocess with gob2."
)

SET_SOURCE_FILES_PROPERTIES(${GOB_GENERATED}
    PROPERTIES GENERATED TRUE)

# executable built from some source
ADD_EXECUTABLE(ibus-engine-chewing
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ibus-chewing-engine.c
    ${CMAKE_CURRENT_SOURCE_DIR}/maker-dialog.c
    )

ADD_EXECUTABLE(generate-gconf-schemas
    ${CMAKE_CURRENT_SOURCE_DIR}/generate-gconf-schemas.c
    ${CMAKE_CURRENT_SOURCE_DIR}/maker-dialog.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ibus-chewing-util.h
    )

# Link the executable to the library.
TARGET_LINK_LIBRARIES(ibus-engine-chewing ${GTK2_LIBRARIES} ${IBUS_LIBRARIES}
    ${CHEWING_LIBRARIES} ${GCONF2_LIBRARIES} ${XTST_LIBRARIES})

TARGET_LINK_LIBRARIES(generate-gconf-schemas ${GTK2_LIBRARIES})

IF(NOT DEFINED LIB_INSTALL_DIR)
    SET(LIB_INSTALL_DIR "lib${IS_64}")
ENDIF()
SET_COMPILE_ENV(LIB_INSTALL_DIR  "${LIB_INSTALL_DIR}"
    DISPLAY STRING "Chewing library directory")


ADD_CUSTOM_COMMAND(TARGET generate-gconf-schemas POST_BUILD
    COMMAND ${CMAKE_BINARY_DIR}/bin/generate-gconf-schemas -v 3 -l
    "C;${TRANSLATED}" ${CMAKE_BINARY_DIR}/data/ibus-chewing.schemas
    COMMENT "Generating gconf-schemas"
    VERBATIM
    )

INSTALL(TARGETS ibus-engine-chewing
     RUNTIME DESTINATION ${LIBEXEC_DIR}
     LIBRARY DESTINATION ${LIB_INSTALL_DIR}
)

