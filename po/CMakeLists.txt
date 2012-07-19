# Translated languages.
SET(SUPPORTED_TRANSLATION zh_CN zh_TW)
INCLUDE(ManageTranslation)

SET(SOURCES_I18N ../src/IBusChewingEngine.gob
  ../src/IBusChewingEngine-input-events.c ../src/IBusChewingEngine-def.c ../src/main.c)

MANAGE_GETTEXT(ALL SRCS ${SOURCES_I18N}
    XGETTEXT_OPTIONS ${XGETTEXT_OPTIONS_C})

SET(ZANATA_SERVER "https://translate.zanata.org")
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/zanata.xml.in ${CMAKE_CURRENT_BINARY_DIR}/zanata.xml @ONLY)

SET(ZANATA_XML_FILE ${CMAKE_CURRENT_BINARY_DIR}/zanata.xml)
MANAGE_ZANATA(${ZANATA_SERVER})

