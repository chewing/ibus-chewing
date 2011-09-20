cmake_minimum_required(VERSION 2.6)

# CMP0011 should be set here, otherwise policy set in module won't affect
# here.
CMAKE_POLICY(SET CMP0011 OLD)

# Default CMAKE_INSTALL_PREFIX should be set before PROJECT()
SET(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install dir prefix")

####################################################################
# Project specific information
#
PROJECT(ibus-chewing)
SET(PRJ_SUMMARY "The Chewing engine for IBus input platform")
SET(PRJ_DESCRIPTION
"IBus-chewing is an IBus front-end of Chewing, an intelligent Chinese input
method for Zhuyin (BoPoMoFo) users.
It supports various Zhuyin keyboard layout, such as standard (DaChen),
IBM, Gin-Yeah, Eten, Eten 26, Hsu, Dvorak, Dvorak-Hsu, and DaChen26.

Chewing also support toned Hanyu pinyin input.")

SET(CMAKE_C_FLAGS "-Wall")

SET(AUTHORS "Peng Huang, Ding-Yi Chen")
SET(MAINTAINER "Ding-Yi Chen <dchen at redhat.com>")
SET(VENDOR "Red Hat, APAC, Inc.")
SET(LICENSE "GPLv2+")

####################################################################
# Includes
#
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/Modules ${CMAKE_ROOT}/Modules )

INCLUDE(ManageEnvironment RESULT_VARIABLE MANAGE_ENVIRONMENT_PATH)
IF(MANAGE_ENVIRONMENT_PATH STREQUAL "NOTFOUND")
    MESSAGE(FATAL_ERROR "ManageEnvironment is not found in CMAKE_MODULE_PATH,
    please:
    1) install cmake-fedora, or;
    2) wget -P SOURCES https://fedorahosted.org/releases/c/m/cmake-fedora/cmake-fedora-modules-only-latest.tar.gz
    tar zxvf SOURCES/cmake-fedora-modules-only-latest.tar.gz")
ENDIF(MANAGE_ENVIRONMENT_PATH STREQUAL "NOTFOUND")

INCLUDE(ManageVersion)
LOAD_RELEASE_FILE("RELEASE-NOTES.txt")
INCLUDE(ManageUninstall)


####################################################################
# Required
#
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0)
PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0)
STRING(REGEX REPLACE "^([1-9][0-9]*)\\.([0-9]+)\\.([0-9]+).*" "\\1" IBUS_VERSION_MAJOR "${IBUS_VERSION}")
STRING(REGEX REPLACE "^([1-9][0-9]*)\\.([0-9]+)\\.([0-9]+).*" "\\2" IBUS_VERSION_MINOR "${IBUS_VERSION}")
STRING(REGEX REPLACE "^([1-9][0-9]*)\\.([0-9]+)\\.([0-9]+).*" "\\3" IBUS_VERSION_PATCH "${IBUS_VERSION}")
MATH(EXPR IBUS_COMPAT_VERSION
    "${IBUS_VERSION_MAJOR}*10000+${IBUS_VERSION_MINOR}*100+${IBUS_VERSION_PATCH}")

MESSAGE("IBUS_VERSION=${IBUS_VERSION} IBUS_COMPAT_VERSION=${IBUS_COMPAT_VERSION}")
SET_COMPILE_ENV(IBUS_COMPAT_VERSION "${IBUS_COMPAT_VERSION}"
    DISPLAY STRING "IBUS_COMPAT_VERSION")

PKG_CHECK_MODULES(XTST REQUIRED xtst x11)
PKG_CHECK_MODULES(CHEWING chewing>=0.3.2)
EXECUTE_PROCESS(COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=datadir chewing
    OUTPUT_VARIABLE CHEWING_DATA_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
SET_COMPILE_ENV(CHEWING_DATA_DIR "${CHEWING_DATA_DIR}" DISPLAY PATH
    "Chewing data dir")

FIND_PROGRAM(GOB2 gob2)
IF(${GOB2} STREQUAL "GOB2-NOTFOUND")
    MESSAGE(FATAL_ERROR "gob2 not found, install gob2 please.")
ENDIF()
####################################################################
# Building
#

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/chewing.xml.in ${CMAKE_BINARY_DIR}/data/chewing.xml)
# Sub directories
ADD_SUBDIRECTORY(po)
ADD_SUBDIRECTORY(src bin)

####################################################################
# Installing
#

SET(INSTALL_DOCS ${RELEASE_FILE} AUTHORS README ChangeLog COPYING USER-GUIDE)
STRING_JOIN(PRJ_DOC_LIST " " ${INSTALL_DOCS})

INSTALL(FILES ${INSTALL_DOCS}
    DESTINATION "${PRJ_DOC_DIR}")

INSTALL(DIRECTORY icons
    DESTINATION  ${PRJ_DATA_DIR})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/data/chewing.xml
    DESTINATION "${DATA_DIR}/ibus/component")

# Schemas
SET(GCONF_SCHEMAS_FILE ${CMAKE_BINARY_DIR}/data/${PROJECT_NAME}.schemas)
INCLUDE(UseGConf)

####################################################################
# Packing
#
SET(MAINTAINER_SETTING "${CMAKE_SOURCE_DIR}/MAINTAINER_SETTING_NO_PACK")
INCLUDE(PackSource)
INCLUDE(PackRPM)
INCLUDE(ManageRelease)
INCLUDE(ManageReleaseOnFedora)

SET(PRJ_IGNORE_FILES_COMMON
    "/doc/"  "\\\\.spec$" "messages.po$" "\\\\.orig$" "/Modules/"
    "/\\\\.cache/"
    )

# GOB_GENERATED source codes are now excluded,
# As Bug 519108 (https://bugzilla.redhat.com/show_bug.cgi?id=519108)
# Is fixed from Fedora 11

LIST(APPEND PRJ_IGNORE_FILES_COMMON
    "src/maker-dialog.*\\\\.[ch]"
    "src/ibus-chewing-engine.*\\\\.[ch]"
    )

LIST(APPEND PACK_SOURCE_IGNORE_FILES
    ${PRJ_IGNORE_FILES_COMMON} "/bin/" "\\\\.xml$" "\\\\.schemas")

PACK_SOURCE(PACK_SOURCE_FILE_NAME "${RPM_BUILD_SOURCES}")
PACK_RPM(PRJ_SRPM_FILE_NAME "${RPM_BUILD_SPECS}/project.spec.in" "${PACK_SOURCE_FILE_NAME}")

# "Off" Warning is displayed if mock is not installed.
USE_MOCK("${RPM_BUILD_SPECS}/project.spec.in")


#####################################################################
# Hosting and Release
#

# "Off" Warning is displayed if ${MAINTAINER_SETTING} does not exist.
MANAGE_RELEASE(${MAINTAINER_SETTING} UPLOAD src ${RPM_BUILD_SOURCES}/${PACK_SOURCE_FILE_NAME})

# Set to "True" to build against updates-candidate if possible.
#SET(FEDORA_CANDIDATE_PREFERRED 1 CACHE BOOL FEDORA_CANDIDATE_PREFERRED)

# Set to "True" to build against testing-candidate if possible.
#SET(EPEL_CANDIDATE_PREFERRED 1 CACHE BOOL EPEL_CANDIDATE_PREFERRED)
RELEASE_ON_FEDORA("${RPM_BUILD_SRPMS}/${PRJ_SRPM_FILE_NAME}"
    TAGS ${FEDORA_CURRENT_RELEASE_TAGS})


####################################################################
# Test Suites.
#
ENABLE_TESTING()


