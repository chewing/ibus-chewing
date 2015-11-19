# Translated languages.
INCLUDE(ManageZanata)

MANAGE_GETTEXT(ALL SRCS ${IBUS_CHEWING_SOURCE_TO_TRANSLATE} MO_DIR ${CMAKE_SOURCE_DIR}/po)

MANAGE_ZANATA("https://fedora.zanata.org/" 
    YES
    VERSION "master"
    )

ADD_DEPENDENCIES(pack_src_pre translations)

