# Translated languages.
INCLUDE(ManageZanata)

SET(SOURCES_I18N ${CMAKE_SOURCE_DIR}/src/IBusChewingEngine.gob
    ${CMAKE_SOURCE_DIR}/src/IBusChewingEngine-input-events.c
    ${CMAKE_SOURCE_DIR}/src/IBusChewingEngine-def.c 
    ${CMAKE_SOURCE_DIR}/src/main.c ${IBUS_CHEWING_SOURCE_COMMON_C}
    )

MANAGE_GETTEXT(ALL SRCS ${SOURCES_I18N})

MANAGE_ZANATA("https://translate.zanata.org/zanata/" 
    YES
    VERSION "master"
    GENERATE_ZANATA_XML
    )

ADD_DEPENDENCIES(pack_src_pre translations)

