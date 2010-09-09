cmake_minimum_required(VERSION 2.4)
####################################################################
# Init Definition
####################################################################
INCLUDE(init.CMakeLists.txt)

####################################################################
# Project basic information
####################################################################
PROJECT(ibus-chewing)
SET(PROJECT_DESCRIPTION "The Chewing engine for IBus input platform")
SET(CMAKE_C_FLAGS "-Wall")

SET(RELEASE_FILE ${CMAKE_SOURCE_DIR}/RELEASE-NOTES.txt)
SET(RPM_RELEASE_FILE ${CMAKE_SOURCE_DIR}/SPECS/RPM-RELEASE-NOTES.txt)
#SETTING_FILE_GET_ATTRIBUTE(PRJ_VER "PRJ_VER" ${RELEASE_FILE})
#MESSAGE("PRJ_VER_PATTERN=${PRJ_VER_PATTERN}")
#SETTING_FILE_GET_ATTRIBUTE(PRJ_VER_RELEASE "RPM_RELEASE" ${RPM_RELEASE_FILE})

SET(SO_VER_MAJOR "1")
SET(SO_VER_MINOR "0")
SET(AUTHORS "Peng Huang, Ding-Yi Chen")
SET(MAINTAINER "Ding-Yi Chen <dchen at redhat.com>")
INCLUDE(ManageVersion)
LOAD_RELEASE_FILE("${RELEASE_FILE}")
INCLUDE(CompileFlags)
SET_USUAL_COMPILE_ENV()

####################################################################
# Includes
####################################################################


####################################################################
# Definitions
####################################################################
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/chewing.xml.in ${CMAKE_BINARY_DIR}/data/chewing.xml)

INCLUDE(UseUninstall)
####################################################################
# Required
####################################################################
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0)
PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0)
STRING(REGEX REPLACE "([1-9][0-9]*)\\.([0-9][0-9]?)[0-9]*\\.([0-9][0-9]?)" "\\1" IBUS_VERSION_MAJOR "${IBUS_VERSION}")
STRING(REGEX REPLACE "([1-9][0-9]*)\\.([0-9][0-9]?)[0-9]*\\.([0-9][0-9]?)" "\\2" IBUS_VERSION_MINOR "${IBUS_VERSION}")
STRING(REGEX REPLACE "([1-9][0-9]*)\\.([0-9][0-9]?)[0-9]*\\.([0-9][0-9]?)" "\\3" IBUS_VERSION_PATCH "${IBUS_VERSION}")
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
# Sub directories
####################################################################
SET(TRANSLATED zh_CN zh_TW)
INCLUDE(UseGettext)
ADD_SUBDIRECTORY(po)
ADD_SUBDIRECTORY(src bin)

####################################################################
# Packing
####################################################################

#====================================================================
# Files to be install.
#
SET(MAIN_DOCS AUTHORS README ChangeLog NEWS COPYING USER-GUIDE)

#INSTALL(FILES ${MAIN_DOCS}
#    DESTINATION "${PROJECT_DOC_DIR}")

INSTALL(DIRECTORY icons
    DESTINATION  ${PROJECT_DATA_DIR})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/data/chewing.xml
    DESTINATION "${DATA_DIR}/ibus/component")

#====================================================================
# Schemas
#
SET(GCONF_SCHEMAS_FILE ${CMAKE_BINARY_DIR}/data/${PROJECT_NAME}.schemas)
INCLUDE(UseGConf)

#====================================================================
# Hosting Service
#
INCLUDE(UseHostingService)
USE_HOSTING_SERVICE_READ_SETTING_FILE("MAINTAINER_SETTING_NO_PACK")

#====================================================================
# Pack configuration
#
SET(CPACK_PACKAGE_VENDOR "Red Hat, APAC, Inc.")

INCLUDE(PackSource)
INCLUDE(PackRPM)
INCLUDE(UseFedpkg)

SET(MY_IGNORE_FILES_COMMON
    "/docs/"  "\\\\.spec$" "messages.po$" "\\\\.orig$"
)

# GOB_GENERATED source codes are now excluded,
# As Bug 519108 (https://bugzilla.redhat.com/show_bug.cgi?id=519108)
# Is fixed from Fedora 11
SET(PRJ_IGNORE_FILES_COMMON
    ${MY_IGNORE_FILES_COMMON}  "src/maker-dialog.*\\\\.[ch]" "src/ibus-chewing-engine.*\\\\.[ch]"
)

SET(PACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES} ${PRJ_IGNORE_FILES_COMMON} "/bin/" "\\\\.xml$" "\\\\.schemas")
#MESSAGE("PACK_SOURCE_IGNORE_FILES=${PACK_SOURCE_IGNORE_FILES}")

PACK_SOURCE(PACK_SOURCE_FILE_NAME "${RPM_BUILD_SOURCES}")
PACK_RPM(PRJ_SRPM_FILE_NAME "${RPM_BUILD_SPECS}/ibus-chewing.spec.in" "${PACK_SOURCE_FILE_NAME}")
USE_MOCK("${RPM_BUILD_SPECS}/ibus-chewing.spec.in")

USE_FEDPKG("${RPM_BUILD_SRPMS}/${PRJ_SRPM_FILE_NAME}")
USE_BODHI()

ADD_DEPENDENCIES(pack_src pot_file)


