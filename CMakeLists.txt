cmake_minimum_required(VERSION 2.6.2)


# CMP0011 should be set here, otherwise policy set in module won't affect
# here.
CMAKE_POLICY(SET CMP0011 OLD)

# Default CMAKE_INSTALL_PREFIX should be set before PROJECT()
SET(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install dir prefix")

# Whether to build on fedora
SET(CMAKE_FEDORA_ENABLE_FEDORA_BUILD "0" CACHE STRING "Enable fedora build")

# Message level M_OFF (4)
SET(MANAGE_MESSAGE_LEVEL 5 CACHE STRING "Message (Verbose) Level")

####################################################################
# Project specific information
#
PROJECT(ibus-chewing)
SET(PRJ_SUMMARY "The Chewing engine for IBus input platform")
SET(SUMMARY_TRANSLATIONS
    "zh_TW" "IBus新酷音輸入法")

SET(PRJ_DESCRIPTION
"IBus-chewing is an IBus front-end of Chewing, an intelligent Chinese input
method for Zhuyin (BoPoMoFo) users.
It supports various Zhuyin keyboard layout, such as standard (DaChen),
IBM, Gin-Yeah, Eten, Eten 26, Hsu, Dvorak, Dvorak-Hsu, and DaChen26.

Chewing also support toned Hanyu pinyin input.")
SET(DESCRIPTION_TRANSLATIONS
"zh_TW"
"IBus-chewing 是新酷音輸入法的IBus前端。
新酷音輸入法是個智慧型注音輸入法，支援多種鍵盤布局，諸如：
標準注音鍵盤、IBM、精業、倚天、倚天26鍵、許氏、Dvorak、Dvorak許氏
及大千26鍵。

本輸入法也同時支援帶調漢語拼音輸入。")

SET(CMAKE_C_FLAGS "-Wall ${CMAKE_C_FLAGS}")

SET(AUTHORS "Peng Huang, Ding-Yi Chen")
SET(MAINTAINER "Ding-Yi Chen <dchen at redhat.com>")
SET(VENDOR "Red Hat, APAC, Inc.")
SET(LICENSE "GPLv2+")
SET(PRJ_GROUP "System Environment/Libraries")
SET(URL_TEMPLATE "http://code.google.com/p/ibus/")
SET(SOURCE_DOWNLOAD_URL_TEMPLATE "http://ibus.googlecode.com/files/%{name}-%{version}-Source.tar.gz")
SET(HAS_TRANSLATION 1)

####################################################################
# Includes
#
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Modules ${CMAKE_ROOT}/Modules ${CMAKE_SOURCE_DIR})

# Create a link if Modules/ does not exists
IF(EXISTS ${CMAKE_SOURCE_DIR}/cmake-fedora/Modules)
    IF(NOT EXISTS ${CMAKE_SOURCE_DIR}/Modules)
	EXECUTE_PROCESS(COMMAND ln -s ${CMAKE_SOURCE_DIR}/cmake-fedora/Modules .)
    ENDIF(NOT EXISTS ${CMAKE_SOURCE_DIR}/Modules)
ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/cmake-fedora/Modules)

IF(EXISTS ${CMAKE_SOURCE_DIR}/cmake-fedora/Templates/fedora/project.spec.in)
    IF(NOT EXISTS ${CMAKE_SOURCE_DIR}/rpm/project.spec.in)
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/cmake-fedora/Templates/fedora/project.spec.in ${CMAKE_SOURCE_DIR}/rpm
	    )
    ENDIF(NOT EXISTS ${CMAKE_SOURCE_DIR}/rpm/project.spec.in)
ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/cmake-fedora/Templates/fedora/project.spec.in)

# Use standard FHS if Fedora build is not enabled
IF(NOT CMAKE_FEDORA_ENABLE_FEDORA_BUILD)
    IF(CMAKE_SYSTEM_PROCESSOR MATCHES "64")
	SET(LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib64" CACHE PATH "Library dir")
    ELSE(CMAKE_SYSTEM_PROCESSOR MATCHES "64")
	SET(LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib" CACHE PATH "Library dir")
    ENDIF(CMAKE_SYSTEM_PROCESSOR MATCHES "64")
    SET(LIBEXEC_DIR "${LIB_DIR}" CACHE PATH "LIBEXEC dir")
ENDIF(NOT CMAKE_FEDORA_ENABLE_FEDORA_BUILD)

INCLUDE(ManageEnvironment RESULT_VARIABLE MANAGE_ENVIRONMENT_PATH)
IF(MANAGE_ENVIRONMENT_PATH STREQUAL "NOTFOUND")
    MESSAGE(FATAL_ERROR "ManageEnvironment is not found in CMAKE_MODULE_PATH,
    please:
    1) git submodule init; git submodule update
    2) or install cmake-fedora/
    3) wget -P SOURCES https://fedorahosted.org/releases/c/m/cmake-fedora/cmake-fedora-modules-only-latest.tar.gz
    tar zxvf SOURCES/cmake-fedora-modules-only-latest.tar.gz")
ENDIF(MANAGE_ENVIRONMENT_PATH STREQUAL "NOTFOUND")

INCLUDE(ManageMessage)
INCLUDE(ManageVersion)
RELEASE_NOTES_READ_FILE()
INCLUDE(ManageFile)
INCLUDE(ManageSourceVersionControl)
INCLUDE(ManageUninstall)
INCLUDE(ManageVariable)
INCLUDE(ManageDependency)

####################################################################
# Dependencies
#

# Enable GNOME_SHELL support by set the value to 1
SET_COMPILE_ENV(GNOME_SHELL 1 DISPLAY BOOL "Enable GNOME Shell support")

SET(IBUS_MIN_VER "1.3")
SET(CHEWING_MIN_VER "0.3.3")
## Requires
MANAGE_DEPENDENCY(REQUIRES GTK2  REQUIRED)
MANAGE_DEPENDENCY(REQUIRES IBUS VER "${IBUS_MIN_VER}" REQUIRED)
MANAGE_DEPENDENCY(REQUIRES CHEWING VER "${CHEWING_MIN_VER}" REQUIRED 
    FEDORA_NAME "libchewing"
    )

SET(BUILD_REQUIRES "" CACHE INTERNAL "Build Requires")
## Build requires
MANAGE_DEPENDENCY(BUILD_REQUIRES CMAKE VER "2.6.2" REQUIRED)
MANAGE_DEPENDENCY(BUILD_REQUIRES GOB2 VER "2.0.16" REQUIRED
    PROGRAM_NAMES "gob2"
    )
MANAGE_DEPENDENCY(BUILD_REQUIRES GTK2 REQUIRED 
    PKG_CONFIG "gtk+-2.0" DEVEL
    )
MANAGE_DEPENDENCY(BUILD_REQUIRES IBUS VER "${IBUS_MIN_VER}" REQUIRED
    PKG_CONFIG "ibus-1.0" DEVEL
    )
MANAGE_DEPENDENCY(BUILD_REQUIRES CHEWING VER "${CHEWING_MIN_VER}" REQUIRED 
    PKG_CONFIG "chewing" FEDORA_NAME "libchewing" DEVEL
    )
MANAGE_DEPENDENCY(BUILD_REQUIRES X11 REQUIRED
    PKG_CONFIG "x11" FEDORA_NAME "libX11" DEVEL
    )
MANAGE_DEPENDENCY(BUILD_REQUIRES XTST REQUIRED
    PKG_CONFIG "xtst" FEDORA_NAME "libXtst" DEVEL
    )
M_MSG(${M_INFO1} "CHEWING_DATADIR=${CHEWING_DATADIR}")

STRING(REGEX REPLACE "^([1-9][0-9]*)\\.([0-9]+)\\.([0-9]+).*" "\\1" IBUS_VERSION_MAJOR "${IBUS_VERSION}")
STRING(REGEX REPLACE "^([1-9][0-9]*)\\.([0-9]+)\\.([0-9]+).*" "\\2" IBUS_VERSION_MINOR "${IBUS_VERSION}")
STRING(REGEX REPLACE "^([1-9][0-9]*)\\.([0-9]+)\\.([0-9]+).*" "\\3" IBUS_VERSION_PATCH "${IBUS_VERSION}")
MATH(EXPR IBUS_COMPAT_VERSION
    "${IBUS_VERSION_MAJOR}*10000+${IBUS_VERSION_MINOR}*100+${IBUS_VERSION_PATCH}")

MESSAGE("IBUS_VERSION=${IBUS_VERSION} IBUS_COMPAT_VERSION=${IBUS_COMPAT_VERSION}")
SET_COMPILE_ENV(IBUS_COMPAT_VERSION "${IBUS_COMPAT_VERSION}"
    DISPLAY STRING "IBUS_COMPAT_VERSION")

SET_COMPILE_ENV(CHEWING_DATADIR "${CHEWING_DATADIR}" DISPLAY PATH
    "Chewing data dir")

####################################################################
# Building
#

# Generate chewing.xml
SET(SYMBOL_XML "<symbol>&#x9177;</symbol>")
IF(NOT GNOME_SHELL)
    SET(SYMBOL_XML "<!-- ${SYMBOL_XML} -->")
ENDIF(NOT GNOME_SHELL)

SET_COMPILE_ENV(CHEWING_HOTKEYS "Control+space")
SET(HOTKEYS_XML "<hotkeys>${CHEWING_HOTKEYS}</hotkeys>")
IF(NOT GNOME_SHELL)
    SET(HOTKEYS_XML "<!-- ${HOTKEYS_XML} -->")
ENDIF(NOT GNOME_SHELL)

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/chewing.xml.in ${CMAKE_BINARY_DIR}/data/chewing.xml)
# Sub directories
ADD_SUBDIRECTORY(po)
ADD_SUBDIRECTORY(src bin)

####################################################################
# Installing
#

MANAGE_SOURCE_VERSION_CONTROL_GIT()
MANAGE_FILE_INSTALL(PRJ_DOC AUTHORS README ChangeLog COPYING USER-GUIDE)
MANAGE_FILE_INSTALL(PRJ_DATA icons)
MANAGE_FILE_INSTALL(DATA ${CMAKE_BINARY_DIR}/data/chewing.xml DEST_SUBDIR "ibus/component")

# Schemas
SET(GCONF_SCHEMAS_FILE ${IBUS_CHEWING_SCHEMAS})
INCLUDE(ManageGConf)

####################################################################
# Packing
#
INCLUDE(ManageArchive)

SET(SOURCE_ARCHIVE_DIR ".")
IF(CMAKE_FEDORA_ENABLE_FEDORA_BUILD)
    ADD_SUBDIRECTORY(rpm)
ENDIF(CMAKE_FEDORA_ENABLE_FEDORA_BUILD)
INCLUDE(ManageRelease)

# GOB_GENERATED source codes are now excluded,
# As Bug 519108 (https://bugzilla.redhat.com/show_bug.cgi?id=519108)
# Is fixed from Fedora 11

PACK_SOURCE_ARCHIVE(${RPM_BUILD_SOURCES}
    GITIGNORE ${CMAKE_SOURCE_DIR}/.gitignore
    INCLUDE ChangeLog ${PROJECT_NAME}.pot
    )

IF(CMAKE_FEDORA_ENABLE_FEDORA_BUILD)
    M_MSG(${M_INFO1} "BUILD_REQUIRES=${BUILD_REQUIRES}")
    M_MSG(${M_INFO1} "REQUIRES=${REQUIRES}")
    ## PACK_RPM NEED to be here, otherwise it cannot find the tarball
    PACK_RPM()
    RELEASE_FEDORA(fedora)
    # "Off" Warning is displayed if mock is not installed.
    RPM_MOCK_BUILD()
    MANAGE_RELEASE(release_fedora)
ELSE(CMAKE_FEDORA_ENABLE_FEDORA_BUILD)
    MANAGE_RELEASE()
ENDIF(CMAKE_FEDORA_ENABLE_FEDORA_BUILD)

#####################################################################
# Hosting and Release
#

#INCLUDE(ManageUpload)
#ADD_CUSTOM_TARGET(upload
#    COMMENT "Uploading all files"
#    )

#MANAGE_UPLOAD_CMD(scp src UPLOAD_FILES ${SOURCE_ARCHIVE_FILE})
#ADD_DEPENDENCIES(upload upload_src)


####################################################################
# Test Suites.
#
ENABLE_TESTING()


