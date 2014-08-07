# Translated languages.
INCLUDE(ManageTranslation)
INCLUDE(ManageZanata)

SET(SOURCES_I18N ${CMAKE_SOURCE_DIR}/src/IBusChewingEngine.gob
    ${CMAKE_SOURCE_DIR}/src/IBusChewingEngine-input-events.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingEngine-def.c 
    ${CMAKE_SOURCE_DIR}/src/main.c)

MANAGE_GETTEXT(ALL SRCS ${SOURCES_I18N} SYSTEM_LOCALES)

MANAGE_ZANATA("https://translate.zanata.org/zanata/" 
    YES
    VERSION "master"
    )

