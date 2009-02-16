# Find xgettext
FIND_PROGRAM( XGETTEXT
        NAMES "xgettext"
)

IF(${XGETTEXT} STREQUAL "XGETTEXT-NOTFOUND")
    MESSAGE(STATUS "xgettext not found.  You will not be able to run 'make extract_messages' and 'make join_messages' in the 'po' directory.")
ELSE()
    MESSAGE(STATUS "Found xgettext: ${XGETTEXT}")
ENDIF()

FILE(GLOB po_files *.po)
GETTEXT_CREATE_TRANSLATIONS(${PROJECT_NAME}.pot ${po_files})

ADD_CUSTOM_TARGET(extract_messages
    COMMAND ${XGETTEXT} --keyword=_ ${CMAKE_HOME_DIRECTORY}/src/*.c
    DEPENDS ${CMAKE_HOME_DIRECTORY}/src/ibus-chewing-engine.c
    COMMENT "Extract translatable messages to messages.po"
)

