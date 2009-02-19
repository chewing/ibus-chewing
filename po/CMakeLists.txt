# Translated languages.
SET(TRANSLATED_PO_FILES zh_CN.po zh_TW.po)

SET(SOURCES_I18N ../src/IBusChewingEngine.gob ../src/main.c)

SET(XGETTEXT_OPTIONS --language=C --keyword=_)

IF (XGETTEXT_FOUND)
    GETTEXT_CREATE_POT(${PROJECT_NAME}.pot OPTION ${XGETTEXT_OPTIONS} SRC ${SOURCES_I18N})
    GETTEXT_CREATE_TRANSLATIONS(${PROJECT_NAME}.pot ALL ${TRANSLATED_PO_FILES}
	COMMENT "Creating translations.")

    ADD_CUSTOM_TARGET(po_template
	COMMAND ${XGETTEXT} ${XGETTEXT_OPTIONS} ${SOURCES_I18N}
	DEPENDS ${SOURCES_I18N} 
	COMMENT "Extract translatable messages to messages.po"
    )
ENDIF()

