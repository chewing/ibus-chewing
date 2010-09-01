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
INCLUDE(ManageVariable)
SET_USUAL_COMPILE_ENV()


#==================================================================
# Developer setting.
#
SET(HOSTING_SERVICE_PROVIDER "GitHub" )
SET(HOSTING_SERVICE_TYPE "git")

####################################################################
# Includes
####################################################################

####################################################################
# Post Include setting
####################################################################


####################################################################
# Definitions
####################################################################
ADD_DEFINITIONS(-DSYSTEMDIR='"${PROJECT_DATADIR}"')
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/chewing.xml.in ${CMAKE_BINARY_DIR}/data/chewing.xml)

INCLUDE(UseUninstall)
####################################################################
# Required
####################################################################
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0)
PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0)
STRING(REGEX MATCH "[1-9][0-9]*\\.[1-9][0-9]*\\.[0-9]*" IBUS_COMPAT_VERSION "${IBUS_VERSION}")
MESSAGE("IBUS_VERSION=${IBUS_VERSION} IBUS_COMPAT_VERSION=${IBUS_COMPAT_VERSION}")
IF ( IBUS_COMPAT_VERSION VERSION_GREATER  1.2.999 )
    ADD_DEFINITIONS(-DIBUS_VERSION=10300)
    MESSAGE("IBus 1.3 detected! -DIBUS_VERSION=10300")
ELSEIF ( IBUS_COMPAT_VERSION VERSION_GREATER  1.2.98 )
    ADD_DEFINITIONS(-DIBUS_VERSION=10299)
    MESSAGE("IBus 1.2.99 detected! -DIBUS_VERSION=10299")
ELSEIF ( IBUS_COMPAT_VERSION VERSION_GREATER  1.1.98 )
    ADD_DEFINITIONS(-DIBUS_VERSION=10200)
    MESSAGE("IBus 1.2 detected! -DIBUS_VERSION=10200")
ELSEIF ( IBUS_COMPAT_VERSION VERSION_GREATER  1.0.98 )
    ADD_DEFINITIONS(-DIBUS_VERSION=10100)
    MESSAGE("IBus 1.1 detected! -DIBUS_VERSION=10100")
ELSE()
    MESSAGE(FATAL "IBus development package is not installed!")
ENDIF()
PKG_CHECK_MODULES(XTST REQUIRED xtst x11)

PKG_CHECK_MODULES(CHEWING chewing>=0.3.2)
COMMAND_OUTPUT_TO_VARIABLE(CHEWING_DATADIR ${PKG_CONFIG_EXECUTABLE} --variable=datadir chewing)
ADD_DEFINITIONS(-DCHEWING_DATADIR='"${CHEWING_DATADIR}"')

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
SET(MAIN_DOCS AUTHORS README ChangeLog NEWS COPYING)

#INSTALL(FILES ${MAIN_DOCS}
#    DESTINATION "${PROJECT_DOCDIR}")

INSTALL(DIRECTORY icons
    DESTINATION  ${PROJECT_DATADIR})

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/data/chewing.xml
    DESTINATION "${datadir}/ibus/component")

#====================================================================
# Schemas
#
SET(GCONF_SCHEMAS_FILE ${CMAKE_BINARY_DIR}/data/${PROJECT_NAME}.schemas)
INCLUDE(UseGConf)

#====================================================================
# Pack configuration
#
SET(CPACK_PACKAGE_VENDOR "Red Hat, APAC, Inc.")

INCLUDE(PackSource)
INCLUDE(PackRPM)

SET(MY_IGNORE_FILES_COMMON
    "/docs/"  "\\\\.spec$" "messages.po$" "/FedPkg/"
)

# GOB_GENERATED source codes are now excluded,
# As Bug 519108 (https://bugzilla.redhat.com/show_bug.cgi?id=519108)
# Is fixed from Fedora 11
SET(PRJ_IGNORE_FILES_COMMON
    ${MY_IGNORE_FILES_COMMON}  "src/maker-dialog.*\\\\.[ch]" "src/ibus-chewing-engine.*\\\\.[ch]"
)

SET(PACK_SOURCE_IGNORE_FILES ${PRJ_IGNORE_FILES_COMMON} "/bin/" "\\\\.xml$" "\\\\.schemas")

#MESSAGE("PRJ_IGNORE_FILES_SOURCE=${PRJ_IGNORE_FILES_SOURCE}")
PACK_SOURCE(PACK_SOURCE_FILE_NAME "${RPM_BUILD_SOURCES}")
MESSAGE("PACK_SOURCE_FILE_NAME=${PACK_SOURCE_FILE_NAME}")
PACK_RPM(PRJ_SRPM_FILE_NAME "${RPM_BUILD_SPECS}/ibus-chewing.spec.in" "${PACK_SOURCE_FILE_NAME}")
MESSAGE("PRJ_SRPM_FILE_NAME=${PRJ_SRPM_FILE_NAME}")

INCLUDE(UseKojiFedora)
USE_KOJI_FEDORA("${RPM_BUILD_SRPMS}/${PRJ_SRPM_FILE_NAME}")

ADD_DEPENDENCIES(pack_src pot_file)

#====================================================================
# Hosting Service
#
INCLUDE(UseHostingService)
USE_HOSTING_SERVICE_READ_SETTING_FILE("MAINTAINER_SETTING_NO_PACK")

