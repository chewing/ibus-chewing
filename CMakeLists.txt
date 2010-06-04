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
SETTING_FILE_GET_ATTRIBUTE(PRJ_VER_PATTERN "PRJ_VER" ${RELEASE_FILE})
#MESSAGE("PRJ_VER_PATTERN=${PRJ_VER_PATTERN}")
SETTING_FILE_GET_ATTRIBUTE(PRJ_VER_RELEASE "RPM_RELEASE" ${RPM_RELEASE_FILE})

SET(SO_VER_MAJOR "1")
SET(SO_VER_MINOR "0")
SET(AUTHORS "Peng Huang, Ding-Yi Chen")
SET(MAINTAINER "Ding-Yi Chen <dchen at redhat.com>")

# For CVS/SVN/GIT change message
SETTING_FILE_GET_ATTRIBUTE(CHANGE_SUMMARY "SUMMARY" ${RELEASE_FILE})
#MESSAGE("### CHANGE_SUMMARY=${CHANGE_SUMMARY}")

# Changelog items
COMMAND_OUTPUT_TO_VARIABLE(CHANGELOG_ITEMS tail -n +4 ${RELEASE_FILE})
#MESSAGE("### CHANGELOG_ITEMS=${CHANGELOG_ITEMS}|")

STRING(COMPARE GREATER "${PRJ_VER_RELEASE}" "1" NEED_RPM_RELEASE_INFO)
IF(${NEED_RPM_RELEASE_INFO})
    SETTING_FILE_GET_ATTRIBUTE(RPM_RELEASE_SUMMARY "RPM_RELEASE_SUMMARY" ${RPM_RELEASE_FILE})
    COMMAND_OUTPUT_TO_VARIABLE(RPM_CHANGELOG_ITEMS tail -n +5 ${RPM_RELEASE_FILE})
ELSE(${NEED_RPM_RELEASE_INFO})
    SET(RPM_RELEASE_SUMMARY ${CHANGE_SUMMARY})
    SET(RPM_CHANGELOG_ITEMS ${CHANGELOG_ITEMS})
ENDIF(${NEED_RPM_RELEASE_INFO})
#MESSAGE("### RPM_RELEASE_SUMMARY=${RPM_RELEASE_SUMMARY}")
#MESSAGE("### RPM_CHANGELOG_ITEMS=|${RPM_CHANGELOG_ITEMS}|")


#==================================================================
# Developer setting.
#
SET(HOSTING_SERVICE_PROVIDER "GitHub")
SET(DEVELOPER_SETTING_FILE "DEVELOPER_SETTING_NO_PACK")
SET(DEVELOPER_DEPENDS UploadPkg.batch.in)
ADD_CUSTOM_TARGET(commit
    COMMAND git commit -a -m "${CHANGE_SUMMARY}"
    COMMAND git tag -a "${PRJ_VER}" -m "Ver ${PRJ_VER}" HEAD
    COMMENT "Commit and tag the changes"
    VERBATIM
    )


####################################################################
# Includes
####################################################################
INCLUDE(Gettext)

#==================================================================
# Include common.CMakeLists.txt
#
INCLUDE(common.CMakeLists.txt)

####################################################################
# Post Include setting
####################################################################

####################################################################
# RPM configuration
####################################################################
FILE(READ ${CMAKE_SOURCE_DIR}/SPECS/RPM-ChangeLog RPM_CHANGELOG)
GENERATE_SPEC(SPECS/${PROJECT_NAME}.spec.in)

####################################################################
# Definitions
####################################################################
ADD_DEFINITIONS(-DPRJ_VERSION='"${PRJ_VER_FULL}"')
ADD_DEFINITIONS(-DPKGDATADIR='"${PROJECT_DATADIR}"')
ADD_DEFINITIONS(-DSYSTEMDIR='"${PROJECT_DATADIR}"')
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/data/chewing.xml.in ${CMAKE_BINARY_DIR}/data/chewing.xml)

####################################################################
# Required
####################################################################
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GTK2 REQUIRED gtk+-2.0)
PKG_CHECK_MODULES(IBUS REQUIRED ibus-1.0)
STRING(REGEX MATCH "[1-9][0-9]*\\.[1-9][0-9]*\\.[0-9]*" IBUS_COMPAT_VERSION "${IBUS_VERSION}")
MESSAGE("IBUS_VERSION=${IBUS_VERSION} IBUS_COMPAT_VERSION=${IBUS_COMPAT_VERSION}")
IF( IBUS_COMPAT_VERSION STREQUAL 1.1.0)
    MESSAGE("IBus 1.1 detected!")
    ADD_DEFINITIONS(-DIBUS_1_1)
ELSEIF ( IBUS_COMPAT_VERSION STREQUAL 1.2.0)
    MESSAGE("IBus 1.2 detected!")
    ADD_DEFINITIONS(-DIBUS_1_2)
ELSE()
    MESSAGE("IBus 1.3 detected!")
    ADD_DEFINITIONS(-DIBUS_1_3)
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
INCLUDE(GConf)

#====================================================================
# CPack configuration
#

SET(CPACK_RESOURCE_FILE_LICENSE
    ${CMAKE_CURRENT_SOURCE_DIR}/COPYING)

SET(PRJ_COMMON_IGNORE_FILES
    "/docs/"
    "\\\\.cache$"  "\\\\.spec$"
    "messages.po$"
)

# GOB_GENERATED source codes are now excluded,
# As Bug 519108 (https://bugzilla.redhat.com/show_bug.cgi?id=519108)
# Is fixed from Fedora 11
SET(PRJ_COMMON_IGNORE_FILES
    ${PRJ_COMMON_IGNORE_FILES} "src/maker-dialog.*\\\\.[ch]" "src/ibus-chewing-engine.*\\\\.[ch]"
)

SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES} ${PRJ_COMMON_IGNORE_FILES} "/bin/"
    "\\\\.xml$" "\\\\.schemas")
SET(CPACK_PACKAGE_IGNORE_FILES ${CPACK_BINARY_IGNORE_FILES} ${PRJ_COMMON_IGNORE_FILES} "/src/"
    "\\\\.spec$" "\\\\.in$" "\\\\.pot$")

INCLUDE(CPack)

# Avoid [Bug 519108]: [gob2] class and enum names convert incorrectly in mock / koji.
ADD_DEPENDENCIES(pack_src pot_file gob2)

