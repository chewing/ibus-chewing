# Location of library include files
INCLUDE_DIRECTORIES(${GTK2_INCLUDE_DIRS} ${IBUS_INCLUDE_DIRS}
    ${CHEWING_INCLUDE_DIRS} ${GCONF2_INCLUDE_DIRS} ${XTST_INCLUDE_DIRS})

# Library location for the linker
LINK_DIRECTORIES(${GTK2_LIBRARY_DIRS} ${IBUS_LIBRARY_DIRS}
    ${CHEWING_LIBRARY_DIRS} ${GCONF2_LIBRARY_DIRS} ${XTST_LIBRARY_DIRS})

#==================================================================
# Sources
#
SET(GOB_GENERATED ${CMAKE_HOME_DIRECTORY}/src/ibus-chewing-engine.c
    ${CMAKE_HOME_DIRECTORY}/src/ibus-chewing-engine.h
    ${CMAKE_HOME_DIRECTORY}/src/ibus-chewing-engine-private.h
    ${CMAKE_HOME_DIRECTORY}/src/maker-dialog.c
    ${CMAKE_HOME_DIRECTORY}/src/maker-dialog.h
    ${CMAKE_HOME_DIRECTORY}/src/maker-dialog-private.h
    )

ADD_CUSTOM_COMMAND(OUTPUT ${GOB_GENERATED}
    COMMAND ${GOB2} ${CMAKE_CURRENT_SOURCE_DIR}/IBusChewingEngine.gob -o ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${GOB2} ${CMAKE_CURRENT_SOURCE_DIR}/MakerDialog.gob -o ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS IBusChewingEngine.gob MakerDialog.gob IBusChewingEngine-def.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

ADD_CUSTOM_TARGET(gob2
    DEPENDS ${GOB_GENERATED}
    COMMENT "Preprocess with gob2."
)


SET_SOURCE_FILES_PROPERTIES(${GOB_GENERATED}
    PROPERTIES GENERATED TRUE)

# executable built from some source
ADD_EXECUTABLE(ibus-engine-chewing main.c
    ibus-chewing-engine.c
    maker-dialog.c
    )
ADD_EXECUTABLE(generate-gconf-schemas
    generate-gconf-schemas.c
    maker-dialog.c
    )
ADD_EXECUTABLE(dialogTest dialogTest.c
    maker-dialog.c
    )

# Link the executable to the library.
TARGET_LINK_LIBRARIES(ibus-engine-chewing ${GTK2_LIBRARIES} ${IBUS_LIBRARIES}
    ${CHEWING_LIBRARIES} ${GCONF2_LIBRARIES} ${X11_LIBRARIES} ${XTST_LIBRARIES})

TARGET_LINK_LIBRARIES(generate-gconf-schemas ${GTK2_LIBRARIES})

TARGET_LINK_LIBRARIES(dialogTest ${GTK2_LIBRARIES} ${IBUS_LIBRARIES}
    ${CHEWING_LIBRARIES} ${GCONF2_LIBRARIES} ${X11_LIBRARIES} ${XTST_LIBRARIES})


IF(NOT DEFINED LIB_INSTALL_DIR)
    IF( $ENV{MACHTYPE} MATCHES "64")
	SET(LIB_INSTALL_DIR lib64)
	ADD_DEFINITIONS(-DLIB_INSTALL_DIR='"${LIB_INSTALL_DIR}"')
    ELSE()
	SET(LIB_INSTALL_DIR lib)
	ADD_DEFINITIONS(-DLIB_INSTALL_DIR='"${LIB_INSTALL_DIR}"')
    ENDIF()
ENDIF()

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

