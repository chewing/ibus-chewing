set(languages ca cs de es fr it ja ko pa pl pt_BR uk zh_CN zh_TW)

include(FindGettext)

foreach(lang ${languages})
    GETTEXT_PROCESS_PO_FILES(${lang} ALL
        PO_FILES ${lang}.po)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${lang}.gmo DESTINATION
        ${CMAKE_INSTALL_LOCALEDIR}/${lang}/LC_MESSAGES/ RENAME ibus-chewing.mo)
endforeach()