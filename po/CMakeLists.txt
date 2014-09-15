# Translated languages.
INCLUDE(ManageZanata)

MANAGE_GETTEXT(ALL SRCS ${IBUS_CHEWING_SOURCE_TO_TRANSLATE})

MANAGE_ZANATA("https://translate.zanata.org/zanata/" 
    YES
    VERSION "master"
    GENERATE_ZANATA_XML
    )

ADD_DEPENDENCIES(pack_src_pre translations)

