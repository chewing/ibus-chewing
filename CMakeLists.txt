cmake_minimum_required(VERSION 3.21.0)

project(ibus-chewing VERSION 1.6.2 LANGUAGES C)
set(PRJ_SUMMARY "The Chewing engine for IBus input platform")
set(SUMMARY_TRANSLATIONS "zh_TW" "IBus新酷音輸入法")

set(PRJ_DESCRIPTION
"IBus-chewing is an IBus front-end of Chewing, an intelligent Chinese input
method for Zhuyin (BoPoMoFo) users.
It supports various Zhuyin keyboard layout, such as standard (DaChen),
IBM, Gin-Yeah, Eten, Eten 26, Hsu, Dvorak, Dvorak-Hsu, and DaChen26.

Chewing also support toned Hanyu pinyin input.")
set(DESCRIPTION_TRANSLATIONS
"zh_TW"
"IBus-chewing 是新酷音輸入法的IBus前端。
新酷音輸入法是個智慧型注音輸入法，支援多種鍵盤布局，諸如：
標準注音鍵盤、IBM、精業、倚天、倚天26鍵、許氏、Dvorak、Dvorak許氏
及大千26鍵。

本輸入法也同時支援帶調漢語拼音輸入。")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(AUTHORS "Peng Huang, Ding-Yi Chen")
set(MAINTAINER "Ding-Yi Chen <dchen at redhat.com>")
set(VENDOR "Red Hat, APAC, Inc.")
set(LICENSE "GPLv2+")
set(PRJ_GROUP "System Environment/Libraries")
set(RPM_SPEC_URL "https://github.com/definite/${PROJECT_NAME}")
set(RPM_SPEC_SOURCES "https://github.com/definite/%{name}/releases/download/%{version}/%{name}-%{version}-Source.tar.gz")

enable_testing()
include(GNUInstallDirs)

option(GNOME_SHELL "Enable GNOME Shell support" ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(IBUS REQUIRED IMPORTED_TARGET ibus-1.0>=1.3)
pkg_check_modules(CHEWING REQUIRED IMPORTED_TARGET chewing>=0.5.1)
pkg_get_variable(CHEWING_DATADIR_REAL chewing datadir)

add_compile_definitions(
    CHEWING_VERSION_MAJOR=0
    CHEWING_VERSION_MINOR=5
    CHEWING_VERSION_PATCH=1
)

pkg_check_modules(GLIB2 REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(LIBADWAITA REQUIRED IMPORTED_TARGET libadwaita-1)
pkg_check_modules(X11 REQUIRED IMPORTED_TARGET x11)

find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)

set(CMAKE_C_STANDARD 99)
add_compile_definitions(_XOPEN_SOURCE)

# Directory that store ibus-chewing icons
add_compile_definitions(PRJ_ICON_DIR="${CMAKE_INSTALL_DATADIR}/ibus-chewing/icons")

# GSettings/GConf schemas
set(PROJECT_SCHEMA_ID org.freedesktop.IBus.Chewing)
add_compile_definitions(
    PROJECT_SCHEMA_ID=${PROJECT_SCHEMA_ID}
    # TODO path deprecated
    PROJECT_SCHEMA_BASE=/desktop/ibus/engine
    PROJECT_SCHEMA_SECTION=chewing
    PROJECT_SCHEMA_DIR=/desktop/ibus/engine/chewing
    PROJECT_SCHEMA_PATH=/desktop/ibus/engine/chewing/
)

# chewing.xml
set(SYMBOL_XML "<symbol>&#x9177;</symbol>")
set(CHEWING_HOTKEYS "Super+space")
set(HOTKEYS_XML "<hotkeys>${CHEWING_HOTKEYS}</hotkeys>")
set(SETUP_PROGRAM_XML "<setup>${CMAKE_INSTALL_LIBEXECDIR}/ibus-setup-chewing</setup>")

if(IBUS_VERSION LESS 1.3.99)
    set(SYMBOL_XML "<!-- ${SYMBOL_XML} -->")
    set(HOTKEYS_XML "<!-- ${HOTKEYS_XML} -->")
    set(SETUP_PROGRAM_XML "<!-- ${SETUP_PROGRAM_XML} -->")
endif()

configure_file(data/chewing.xml.in data/chewing.xml)
configure_file(data/ibus-setup-chewing.desktop.in data/ibus-setup-chewing.desktop)

add_subdirectory(po)
add_subdirectory(src bin)
add_subdirectory(test)

install(FILES AUTHORS README.md ChangeLog COPYING USER-GUIDE
    DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES ${CMAKE_BINARY_DIR}/data/chewing.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/ibus/component)
install(FILES ${CMAKE_BINARY_DIR}/data/ibus-setup-chewing.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(DIRECTORY icons
    DESTINATION ${CMAKE_INSTALL_DATADIR}/ibus-chewing)

set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_SOURCE_IGNORE_FILES "/build" "/out" "/\\\\..*")
set(CPACK_SOURCE_GENERATOR TXZ)
include(CPack)
