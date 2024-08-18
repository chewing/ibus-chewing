cmake_minimum_required(VERSION 3.21.0)

project(ibus-chewing LANGUAGES C)
set(PRJ_SUMMARY "The Chewing engine for IBus input platform")
set(SUMMARY_TRANSLATIONS "zh_TW" "IBus新酷音輸入法")

set(PRJ_VER_MAJOR 2)
set(PRJ_VER_MINOR 1)
set(PRJ_VER_PATCH 0)
# set(PRJ_VER_EXTRA "")

set(PRJ_VER "${PRJ_VER_MAJOR}.${PRJ_VER_MINOR}.${PRJ_VER_PATCH}")
if(DEFINED PRJ_VER_EXTRA)
  string(APPEND PRJ_VER "-${PRJ_VER_EXTRA}")
endif()

find_package(Git)
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} --work-tree ${CMAKE_SOURCE_DIR} describe --dirty --always
        OUTPUT_STRIP_TRAILING_WHITESPACE
        OUTPUT_VARIABLE GIT_DESCRIBE
        ERROR_VARIABLE GIT_DESCRIBE_ERROR
    )
    if(NOT GIT_DESCRIBE_ERROR)
        string(REPLACE "v" "" PRJ_VER ${GIT_DESCRIBE})
    endif()
endif()

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

option(ERROR_ON_WARNING "Toggle error-on-warning build" OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
if(ERROR_ON_WARNING)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")
endif()

set(AUTHORS "Peng Huang, Ding-Yi Chen")
set(MAINTAINER "Ding-Yi Chen <dchen at redhat.com>")
set(VENDOR "Red Hat, APAC, Inc.")
set(LICENSE "GPLv2+")
set(PRJ_GROUP "System Environment/Libraries")
set(RRJ_URL "https://github.com/chewing/${PROJECT_NAME}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()
include(GNUInstallDirs)

option(GNOME_SHELL "Enable GNOME Shell support" ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(IBUS REQUIRED IMPORTED_TARGET ibus-1.0>=1.3)
pkg_check_modules(CHEWING REQUIRED IMPORTED_TARGET chewing>=0.9.0)
pkg_get_variable(CHEWING_DATADIR_REAL chewing datadir)

add_compile_definitions(
    CHEWING_VERSION_MAJOR=0
    CHEWING_VERSION_MINOR=9
    CHEWING_VERSION_PATCH=0
)

pkg_check_modules(GLIB2 REQUIRED IMPORTED_TARGET glib-2.0)
pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
pkg_check_modules(LIBADWAITA REQUIRED IMPORTED_TARGET libadwaita-1)

find_program(GLIB_COMPILE_RESOURCES NAMES glib-compile-resources REQUIRED)
find_program(GLIB_COMPILE_SCHEMAS NAMES glib-compile-schemas)

set(CMAKE_C_STANDARD 23)
add_compile_definitions(_XOPEN_SOURCE)

# Directory that store ibus-chewing icons
add_compile_definitions(PRJ_ICON_DIR="${CMAKE_INSTALL_DATADIR}/ibus-chewing/icons")

# GSettings/GConf schemas
set(PROJECT_SCHEMA_ID org.freedesktop.IBus.Chewing)
set(LIBEXEC_DIR ${CMAKE_INSTALL_FULL_LIBEXECDIR})
set(PRJ_DATA_DIR ${CMAKE_INSTALL_FULL_DATADIR}/ibus-chewing)
add_compile_definitions(
    PROJECT_NAME=${CMAKE_PROJECT_NAME}
    PROJECT_SCHEMA_ID=${PROJECT_SCHEMA_ID}
    # TODO path deprecated
    PROJECT_SCHEMA_BASE=/desktop/ibus/engine
    PROJECT_SCHEMA_SECTION=chewing
    PROJECT_SCHEMA_DIR=/desktop/ibus/engine/chewing
    PROJECT_SCHEMA_PATH=/desktop/ibus/engine/chewing/
    DATA_DIR=${CMAKE_INSTALL_FULL_DATADIR}
    PRJ_DATA_DIR=${PRJ_DATA_DIR}
    PRJ_VER=${PRJ_VER}
    PRJ_URL=${PRJ_URL}
    IBUS_VERSION=${IBUS_VERSION}
    CHEWING_VERSION=${CHEWING_VERSION}
    LIBEXEC_DIR=${LIBEXEC_DIR}
    CHEWING_DATADIR_REAL=${CHEWING_DATADIR_REAL}
)

# chewing.xml
set(ICON_PROP_KEY "<icon_prop_key>InputMode</icon_prop_key>")
set(SYMBOL_XML "<symbol>&#x9177;</symbol>")
set(CHEWING_HOTKEYS "Super+space")
set(HOTKEYS_XML "<hotkeys>${CHEWING_HOTKEYS}</hotkeys>")
set(SETUP_PROGRAM_XML "<setup>${CMAKE_INSTALL_FULL_LIBEXECDIR}/ibus-setup-chewing</setup>")

if(IBUS_VERSION LESS 1.5.11)
    set(ICON_PROP_KEY "<!-- ${ICON_PROP_KEY} -->")
endif()
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

install(FILES AUTHORS README.md CHANGELOG.md ChangeLog-1.x COPYING USER-GUIDE
    DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(FILES ${CMAKE_BINARY_DIR}/data/chewing.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/ibus/component)
install(FILES ${CMAKE_BINARY_DIR}/data/ibus-setup-chewing.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(DIRECTORY icons
    DESTINATION ${CMAKE_INSTALL_DATADIR}/ibus-chewing)
install(FILES icons/org.freedesktop.IBus.Chewing.Setup.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

set(CPACK_PACKAGE_VERSION ${PRJ_VER})
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_SOURCE_IGNORE_FILES "/build" "/out" "/\\\\..*")
set(CPACK_SOURCE_GENERATOR TXZ)
include(CPack)
