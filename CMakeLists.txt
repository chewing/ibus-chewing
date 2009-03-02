cmake_minimum_required(VERSION 2.4)
####################################################################
# Init Definition 
####################################################################
INCLUDE(init.CMakeLists.txt)

####################################################################
# Project basic information
####################################################################
PROJECT(ibus-chewing)
SET(CMAKE_C_FLAGS "-Wall")
SET(PRJ_VER_MAJOR "1")
SET(PRJ_VER_MINOR "0")
SET(PRJ_VER_PATCH "2")
SET(PRJ_VER_RELEASE "1")
SET(SO_VER_MAJOR "1")
SET(SO_VER_MINOR "0")
SET(AUTHORS "Peng Huang, Ding-Yi Chen")
SET(MAINTAINER "Ding-Yi Chen <dchen at redhat.com>")

# For CVS changelog
SET(CHANGE_SUMMARY "Fix rpm install issues.")

SET(SUPPORTED_DIST_TAGS F-9 F-10) 
SET(BODHI_DIST_TAGS fc9 fc10)



####################################################################
# Includes
####################################################################
SET(PRJ_VER ${PRJ_VER_MAJOR}.${PRJ_VER_MINOR}.${PRJ_VER_PATCH}.${DATE_SHORT})
INCLUDE(Version)
SET(GENERATE_SPEC SPECS/${PROJECT_NAME}.spec.in)
INCLUDE(Gettext)

#==================================================================
# Include common.CMakeLists.txt
#
INCLUDE(common.CMakeLists.txt)


####################################################################
# Definitions
####################################################################
ADD_DEFINITIONS(-DPRJ_VERSION='"${PRJ_VER_FULL}"')
ADD_DEFINITIONS(-DPKGDATADIR='"${PROJECT_DATADIR}"')
ADD_DEFINITIONS(-DSYSTEMDIR='"${PROJECT_DATADIR}"')
CONFIGURE_FILE(src/chewing.xml.in src/chewing.xml)

#==================================================================
# Required packages
#
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GTK2 gtk+-2.0)
PKG_CHECK_MODULES(IBUS ibus-1.0)
PKG_CHECK_MODULES(CHEWING chewing)
PKG_CHECK_MODULES(GCONF2 gconf-2.0)
FIND_PROGRAM(GOB2 gob2)

IF(${GOB2} STREQUAL "GOB2-NOTFOUND")
    MESSAGE(FATAL_ERROR "gob2 not found, install gob2 please.")
ENDIF()

#==================================================================
# Source code subdirectories
#
ADD_SUBDIRECTORY(src bin)
ADD_SUBDIRECTORY(po)

#====================================================================
# Files to be install.
#
SET(MAIN_DOCS AUTHORS README ChangeLog NEWS COPYING)

#INSTALL(FILES ${MAIN_DOCS} 
#    DESTINATION "${PROJECT_DOCDIR}")

INSTALL(DIRECTORY icons
    DESTINATION  ${PROJECT_DATADIR})

#====================================================================
# CPack configuration
#

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ibus-chewing: chewing Chinese IME for iBus")
SET(CPACK_PACKAGE_VENDOR "Red Hat, APAC, Inc.")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE
    ${CMAKE_CURRENT_SOURCE_DIR}/COPYING)
SET(PRJ_COMMON_IGNORE_FILES 
    "/docs/"  
    "\\\\.cache$"  "\\\\.gmo$" "\\\\.spec$" "ibus-chewing-engine.*\\\\.[ch]$"
    "messages.po$"
)

SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES} ${PRJ_COMMON_IGNORE_FILES} "/bin/" 
    "\\\\.xml$")
SET(CPACK_PACKAGE_IGNORE_FILES ${CPACK_BINARY_IGNORE_FILES} ${PRJ_COMMON_IGNORE_FILES} "/src/" 
    "\\\\.spec$" "\\\\.in$" "\\\\.pot$")

INCLUDE(CPack)

ADD_CUSTOM_TARGET(ChangeLog
    COMMAND echo "* ${DATE} - ${MAINTAINER} - ${PRJ_VER_FULL}" > ${CMAKE_SOURCE_DIR}/ChangeLog
    COMMAND cat ${CMAKE_SOURCE_DIR}/ChangeLog.in >> ${CMAKE_SOURCE_DIR}/ChangeLog
    DEPENDS ${CMAKE_SOURCE_DIR}/ChangeLog.in
    COMMENT "Making ChangeLog"
    VERBATIM
    )

ADD_DEPENDENCIES(pack_src ChangeLog)
ADD_DEPENDENCIES(pack_src pot_file)

