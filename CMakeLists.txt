cmake_minimum_required(VERSION 2.6.2)

# Included scripts do automatic cmake_policy PUSH and POP.
# OLD: CMake policy in included script will affect the invoker.
# CMAKE_POLICY(SET CMP0011 OLD)


####################################################################
# Find cmake-fedora
#
LIST(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_SOURCE_DIR}/Modules")
INCLUDE(ManageEnvironmentCommon OPTIONAL RESULT_VARIABLE MANAGE_ENVIRONMENT_PATH)

IF(MANAGE_ENVIRONMENT_PATH STREQUAL "NOTFOUND")
    # Is cmake-fedora a git submodule?    
    IF(NOT EXISTS ${CMAKE_SOURCE_DIR}/Modules)
	## Create a link if Modules/ does not exists
	IF(NOT EXISTS ${CMAKE_SOURCE_DIR}/cmake-fedora)
	    IF(EXISTS ${CMAKE_SOURCE_DIR}/.gitmodules)
                ## cmake-fedora may be a git submodules
		EXECUTE_PROCESS(COMMAND git submodule init
		    COMMAND git submodule update
		    RESULT_VARIABLE gitSubmoduleFailed
		    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} 
		    )	
	    ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/.gitmodules)
	ENDIF(NOT EXISTS ${CMAKE_SOURCE_DIR}/cmake-fedora)

	IF(EXISTS ${CMAKE_SOURCE_DIR}/cmake-fedora/Modules)
            ## cmake-fedora is now available as subdirectory, try again
	    EXECUTE_PROCESS(COMMAND ln -s ${CMAKE_SOURCE_DIR}/cmake-fedora/Modules .)
	    INCLUDE(ManageEnvironmentCommon RESULT_VARIABLE MANAGE_ENVIRONMENT_PATH)
	ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/cmake-fedora/Modules)
    ENDIF(NOT EXISTS ${CMAKE_SOURCE_DIR}/Modules)
ENDIF(MANAGE_ENVIRONMENT_PATH STREQUAL "NOTFOUND")

IF(MANAGE_ENVIRONMENT_PATH STREQUAL "NOTFOUND")
    MESSAGE(FATAL_ERROR "ManageEnvironmentCommon.cmake is not found in CMAKE_MODULE_PATH.
Please either:
    1) Use cmake-fedora as a git submodule:
       git submodule add https://git.fedorahosted.org/git/cmake-fedora.git 
    2) Install cmake-fedora from repository:
       yum -y install cmake-fedora
    3) Download the source:
       wget https://fedorahosted.org/releases/c/m/cmake-fedora/cmake-fedora-modules-only-latest.tar.gz
       tar zxvf SOURCES/cmake-fedora-modules-only-latest.tar.gz")
ENDIF(MANAGE_ENVIRONMENT_PATH STREQUAL "NOTFOUND")

GET_FILENAME_COMPONENT(CMAKE_FEDORA_MODULE_DIR 
    "${MANAGE_ENVIRONMENT_PATH}" PATH CACHE
    )

# Whether to build on fedora
SET(CMAKE_FEDORA_ENABLE_FEDORA_BUILD "0" CACHE STRING "Enable fedora build")

# Default Message level M_OFF (4)
SET(MANAGE_MESSAGE_LEVEL 7 CACHE STRING "Message (Verbose) Level")

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

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

SET(AUTHORS "Peng Huang, Ding-Yi Chen")
SET(MAINTAINER "Ding-Yi Chen <dchen at redhat.com>")
SET(VENDOR "Red Hat, APAC, Inc.")
SET(LICENSE "GPLv2+")
SET(PRJ_GROUP "System Environment/Libraries")
SET(RPM_SPEC_URL "https://github.com/definite/${PROJECT_NAME}")
SET(RPM_SPEC_SOURCES "https://github.com/definite/${PROJECT_NAME}/releases/download/${PROJECT_NAME}/${PROJECT_NAME}-%{version}-Source.tar.gz")


####################################################################
# Includes
#

# Use standard FHS if Fedora build is not enabled
IF(NOT CMAKE_FEDORA_ENABLE_FEDORA_BUILD)
    SET(LIBEXEC_DIR "${LIB_DIR}" CACHE PATH "LIBEXEC dir")
ENDIF(NOT CMAKE_FEDORA_ENABLE_FEDORA_BUILD)

INCLUDE(ManageEnvironment)
MANAGE_CMAKE_POLICY(CMP0017 NEW)

INCLUDE(ManageMessage)
INCLUDE(ManageVersion)
RELEASE_NOTES_READ_FILE()
INCLUDE(ManageArchive)
INCLUDE(ManageDependency)
INCLUDE(ManageFile)
INCLUDE(ManageSourceVersionControl)
INCLUDE(ManageTarget)
INCLUDE(ManageTranslation)
INCLUDE(ManageUninstall)

ENABLE_TESTING()

####################################################################
# Dependencies
#

# Enable GNOME_SHELL support by set the value to 1
SET_COMPILE_ENV(GNOME_SHELL 1 CACHE BOOL "Enable GNOME Shell support")

# Enable GCONF2 support by set the value to 1
SET_COMPILE_ENV(GCONF2_SUPPORT 0 CACHE BOOL "Enable GConf2 support")

# Enable GSETTINGS support by set the value to 1
SET_COMPILE_ENV(GSETTINGS_SUPPORT 1 CACHE BOOL "Enable GSettings support")

SET(IBUS_MIN_VER "1.3")
SET(CHEWING_MIN_VER "0.3.3")

########## Package Dependencies ##########

MANAGE_DEPENDENCY(BUILD_REQUIRES CMAKE VER "2.6.2" REQUIRED)

### ibus
MANAGE_DEPENDENCY(REQUIRES IBUS VER "${IBUS_MIN_VER}" REQUIRED PKG_CONFIG ${IBUS_PKG_CONFIG})

SET(IBUS_PKG_CONFIG "ibus-1.0" CACHE STRING "ibus pkg-config name")
MANAGE_DEPENDENCY(BUILD_REQUIRES IBUS VER "${IBUS_MIN_VER}" REQUIRED
    PKG_CONFIG ${IBUS_PKG_CONFIG} DEVEL
    )

### libchewing
SET(CHEWING_PACKAGE_NAME "libchewing" CACHE STRING "chewing package name")
MANAGE_DEPENDENCY(REQUIRES CHEWING VER "${CHEWING_MIN_VER}" REQUIRED 
    PACKAGE_NAME "${CHEWING_PACKAGE_NAME}"
    )

SET(CHEWING_DEVEL_PACKAGE_NAME "${CHEWING_PACKAGE_NAME}-devel" CACHE STRING "chewing devel package name")
SET(CHEWING_PKG_CONFIG "chewing" CACHE STRING "chewing pkg-config name")
MANAGE_DEPENDENCY(BUILD_REQUIRES CHEWING VER "${CHEWING_MIN_VER}" REQUIRED 
    PKG_CONFIG ${CHEWING_PKG_CONFIG} 
    PACKAGE_NAME "${CHEWING_DEVEL_PACKAGE_NAME}"
    )

M_MSG(${M_INFO1} "CHEWING_VERSION=${CHEWING_VERSION}")
STRING_SPLIT(chewingVerList "." "${CHEWING_VERSION}")
LIST(GET chewingVerList 0 CHEWING_VERSION_MAJOR)
LIST(GET chewingVerList 1 CHEWING_VERSION_MINOR)
LIST(GET chewingVerList 2 CHEWING_VERSION_PATCH)
M_MSG(${M_INFO1} "CHEWING_VERSION_MAJOR=${CHEWING_VERSION_MAJOR}")
M_MSG(${M_INFO1} "CHEWING_VERSION_MINOR=${CHEWING_VERSION_MINOR}")
M_MSG(${M_INFO1} "CHEWING_VERSION_PATCH=${CHEWING_VERSION_PATCH}")
ADD_DEFINITIONS("-DCHEWING_VERSION_MAJOR=${CHEWING_VERSION_MAJOR}"
    "-DCHEWING_VERSION_MINOR=${CHEWING_VERSION_MINOR}"
    "-DCHEWING_VERSION_PATCH=${CHEWING_VERSION_PATCH}"
    )

## In case CHEWING_DATADIR is incorrect
FOREACH(d ${CHEWING_DATADIR} /usr/share/chewing ${CHEWING_LIBDIR}/libchewing)
    IF(EXISTS "${d}/symbols.dat")
	SET(CHEWING_DATADIR_REAL "${d}" CACHE STRING "CHEWING_DATADIR_REAL")
	BREAK()
    ENDIF()
ENDFOREACH()

IF("${CHEWING_DATADIR_REAL}" STREQUAL "")
    M_MSG(${M_ERROR} "Chewing: symbols.dat cannot be found, please reinstall chewing package")
ENDIF()

M_MSG(${M_INFO1} "CHEWING_DATADIR_REAL=${CHEWING_DATADIR_REAL}")
SET_COMPILE_ENV(CHEWING_DATADIR_REAL)

### Glib, Gtk and GOB
MANAGE_DEPENDENCY(REQUIRES GLIB2  REQUIRED)
SET(GLIB2_PKG_CONFIG "gio-2.0;glib-2.0;gmodule-2.0;gobject-2.0;gthread-2.0" 
    CACHE STRING "glib2 pkg-config name")
MANAGE_DEPENDENCY(BUILD_REQUIRES GLIB2 REQUIRED 
    PKG_CONFIG ${GLIB2_PKG_CONFIG} glib-2.0 DEVEL
    )

MANAGE_DEPENDENCY(REQUIRES GTK2  REQUIRED)
SET(GTK2_PKG_CONFIG "gtk+-2.0" CACHE STRING "gtk pkg-config name")
MANAGE_DEPENDENCY(BUILD_REQUIRES GTK2 REQUIRED 
    PKG_CONFIG ${GTK2_PKG_CONFIG} DEVEL
    )

MANAGE_DEPENDENCY(BUILD_REQUIRES GOB2 VER "2.0.16" REQUIRED
    PROGRAM_NAMES "gob2"
    )

### X11
SET(X11_PKG_CONFIG "x11" CACHE STRING "x11 pkg-config name")
MANAGE_DEPENDENCY(BUILD_REQUIRES X11 REQUIRED
    PKG_CONFIG ${X11_PKG_CONFIG} PACKAGE_NAME "libX11-devel"
    )

SET(XTST_PKG_CONFIG "xtst" CACHE STRING "xtst pkg-config name")
MANAGE_DEPENDENCY(BUILD_REQUIRES XTST REQUIRED
    PKG_CONFIG "${XTST_PKG_CONFIG}" PACKAGE_NAME "libXtst-devel"
    )

####################################################################
# Building
#

SET_COMPILE_ENV(PROJECT_SCHEMA_ID "org.freedesktop.IBus.Chewing" CACHE STRING "PROJECT_SCHEMA_ID")

SET_COMPILE_ENV(PROJECT_SCHEMA_PATH "/desktop/ibus/engine/chewing/" CACHE STRING "PROJECT_SCHEMA_PATH")
# Generate chewing.xml
SET(SYMBOL_XML "<symbol>&#x9177;</symbol>")
IF(NOT GNOME_SHELL)
    SET(SYMBOL_XML "<!-- ${SYMBOL_XML} -->")
ENDIF(NOT GNOME_SHELL)

SET_COMPILE_ENV(CHEWING_HOTKEYS "Super+space")
SET(HOTKEYS_XML "<hotkeys>${CHEWING_HOTKEYS}</hotkeys>")
IF(NOT GNOME_SHELL)
    SET(HOTKEYS_XML "<!-- ${HOTKEYS_XML} -->")
ENDIF(NOT GNOME_SHELL)

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/chewing.xml.in ${CMAKE_BINARY_DIR}/data/chewing.xml)
# Sub directories
ADD_SUBDIRECTORY(src bin)
ADD_SUBDIRECTORY(po)
ADD_SUBDIRECTORY(test test-bin)

####################################################################
# Installing
#

INSTALL(FILES AUTHORS README ChangeLog COPYING USER-GUIDE
    DESTINATION "${PRJ_DOC_DIR}"
    )

INSTALL(FILES ${CMAKE_BINARY_DIR}/data/chewing.xml 
    DESTINATION "${DATA_DIR}/ibus/component")

INSTALL(FILES ${CMAKE_SOURCE_DIR}/data/ibus-setup-chewing.desktop 
    DESTINATION "${DATA_DIR}/applications")

INSTALL(DIRECTORY icons
    DESTINATION "${PRJ_DATA_DIR}"
    )

MANAGE_SOURCE_VERSION_CONTROL_GIT()

####################################################################
# Packing
#
SET(SOURCE_ARCHIVE_DIR ${CMAKE_BINARY_DIR} CACHE PATH "Source Archive Dir")
PACK_SOURCE_ARCHIVE("${SOURCE_ARCHIVE_DIR}"
    GITIGNORE ${CMAKE_SOURCE_DIR}/.gitignore
    INCLUDE ${PROJECT_NAME}.pot
    )

INCLUDE(ManageRelease)

IF(CMAKE_FEDORA_ENABLE_FEDORA_BUILD)
    SET(RPM_BUILD_SOURCES ${SOURCE_ARCHIVE_DIR})
    ADD_SUBDIRECTORY(rpm)
    LIST(APPEND _releaset_targets "release_fedora")
ENDIF(CMAKE_FEDORA_ENABLE_FEDORA_BUILD)


#####################################################################
# Hosting
#

#INCLUDE(ManageUpload)
#ADD_CUSTOM_TARGET(upload
#    COMMENT "Uploading all files"
#    )

####################################################################
# Custom
#

####################################################################
# Release
#

INCLUDE(ManageRelease)
SET(releaseTarget "tag_push")
IF(CMAKE_FEDORA_ENABLE_FEDORA_BUILD)
    LIST(APPEND releaseTargets release_fedora)
ENDIF(CMAKE_FEDORA_ENABLE_FEDORA_BUILD)
MANAGE_RELEASE(${releaseTargets})

