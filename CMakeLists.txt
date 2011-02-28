cmake_minimum_required(VERSION 2.4)
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
MESSAGE("CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}")

INCLUDE(ManageVariable RESULT_VARIABLE MANAGE_VARIABLE)
IF(MANAGE_VARIABLE STREQUAL "NOTFOUND")
    MESSAGE(FATAL_ERROR "ManageVariable is not found in CMAKE_MODULE_PATH,
    please:
    1) install cmake-fedora, or;
    2) wget https://fedorahosted.org/releases/c/m/cmake-fedora/cmake-fedora-modules-only-latest.tar.gz; tar zxvf cmake-fedora-modules-only-latest.tar.gz")
ENDIF()

IF(CMAKE_VERSION)
    CMAKE_POLICY(SET CMP0005 OLD)
ENDIF(CMAKE_VERSION)

INCLUDE(ManageVersion)
LOAD_RELEASE_FILE("RELEASE-NOTES.txt")
INCLUDE(UseUninstall)

INCLUDE(CompileEnv)
SET_USUAL_COMPILE_ENVS()

INCLUDE(CMakeVersion)


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
ADD_DEFINITIONS(-DIBUS_VERSION=${IBUS_COMPAT_VERSION})
MESSAGE("ADD_DEFINITIONS(-DIBUS_VERSION=${IBUS_COMPAT_VERSION})")

PKG_CHECK_MODULES(XTST REQUIRED xtst x11)
PKG_CHECK_MODULES(CHEWING chewing>=0.3.2)
COMMAND_OUTPUT_TO_VARIABLE(CHEWING_DATA_DIR ${PKG_CONFIG_EXECUTABLE} --variable=datadir chewing)
ADD_DEFINITIONS(-DCHEWING_DATA_DIR='"${CHEWING_DATA_DIR}"')

FIND_PROGRAM(GOB2 gob2)
IF(${GOB2} STREQUAL "GOB2-NOTFOUND")
    MESSAGE(FATAL_ERROR "gob2 not found, install gob2 please.")
ENDIF()
####################################################################
# Building
#

CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/chewing.xml.in ${CMAKE_BINARY_DIR}/data/chewing.xml)
# Sub directories
SET(TRANSLATED zh_CN zh_TW)
INCLUDE(UseGettext)
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
SET(MAINTAINER_SETTING "MAINTAINER_SETTING_NO_PACK")
INCLUDE(PackSource)
INCLUDE(PackRPM)
INCLUDE(ManageReleaseOnFedora)

SET(PRJ_IGNORE_FILES_COMMON
    "/doc/"  "\\\\.spec$" "messages.po$" "\\\\.orig$" "/Modules/"
    )

# GOB_GENERATED source codes are now excluded,
# As Bug 519108 (https://bugzilla.redhat.com/show_bug.cgi?id=519108)
# Is fixed from Fedora 11

SET(PRJ_IGNORE_FILES_COMMON
    ${PRJ_IGNORE_FILES_COMMON}  "src/maker-dialog.*\\\\.[ch]"
    "src/ibus-chewing-engine.*\\\\.[ch]"
    )

SET(PACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES}
    ${PRJ_IGNORE_FILES_COMMON} "/bin/" "\\\\.xml$" "\\\\.schemas")

PACK_SOURCE(PACK_SOURCE_FILE_NAME "${RPM_BUILD_SOURCES}")
PACK_RPM(PRJ_SRPM_FILE_NAME "${RPM_BUILD_SPECS}/project.spec.in" "${PACK_SOURCE_FILE_NAME}")

# If mock exists, then add mock related targets.
FIND_PROGRAM(MOCK_CMD mock)
IF(NOT MOCK_CMD STREQUAL "MOCK_CMD-NOTFOUND")
    USE_MOCK("${CMAKE_SOURCE_DIR}/SPECS/project.spec.in")
ENDIF(NOT MOCK_CMD STREQUAL "MOCK_CMD-NOTFOUND")

IF(EXISTS ${MAINTAINER_SETTING})
    RELEASE_ON_FEDORA("${RPM_BUILD_SRPMS}/${PRJ_SRPM_FILE_NAME}"
	${FEDORA_CURRENT_RELEASE_TAGS} el6 el5)
ENDIF(EXISTS ${MAINTAINER_SETTING})

####################################################################
# Hosting
#
INCLUDE(ManageMaintainerTargets)
MAINTAINER_SETTING_READ_FILE("${MAINTAINER_SETTING}"
    "${RPM_BUILD_SOURCES}/${PACK_SOURCE_FILE_NAME}")


####################################################################
# Test Suites.
#
ENABLE_TESTING()


