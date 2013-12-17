# Translated languages.
SET(SUPPORTED_TRANSLATION zh_CN zh_TW)
INCLUDE(ManageTranslation)

SET(SOURCES_I18N ${CMAKE_SOURCE_DIR}/src/IBusChewingEngine.gob
    ${CMAKE_SOURCE_DIR}/src/IBusChewingEngine-input-events.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingEngine-def.c 
    ${CMAKE_SOURCE_DIR}/src/main.c)

MANAGE_GETTEXT(ALL SRCS ${SOURCES_I18N})

SET(ZANATA_SERVER "https://translate.zanata.org/zanata/")
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/zanata.xml.in ${CMAKE_CURRENT_BINARY_DIR}/zanata.xml @ONLY)

SET(ZANATA_XML_FILE ${CMAKE_CURRENT_BINARY_DIR}/zanata.xml)
MANAGE_ZANATA(${ZANATA_SERVER} YES)

