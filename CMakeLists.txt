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
SET(PRJ_VER_PATCH "0")
SET(PRJ_VER_RELEASE "1")
SET(SO_VER_MAJOR "1")
SET(SO_VER_MINOR "0")
SET(AUTHORS "Peng Huang (shawn.p.huang@gmail.com), Ding-Yi Chen (dchen at redhat.com)")

# For CVS changelog
SET(CHANGE_SUMMARY "First working version for iBus C version.")

SET(SUPPORTED_DIST_TAGS F-9 F-10) 
SET(BODHI_DIST_TAGS fc9 fc10)



####################################################################
# Includes
####################################################################

#==================================================================
# Include common.CMakeLists.txt
#

SET(GENERATE_SPEC TRUE)
SET(PRJ_VER_TEMPLATE ${PRJ_VER_MAJOR}.${PRJ_VER_MINOR}.${PRJ_VER_PATCH}-${DATE_SHORT})
INCLUDE(common.CMakeLists.txt)


####################################################################
# Definitions
####################################################################
ADD_DEFINITIONS(-DPRJ_VERSION='"${PRJ_VER_FULL}"')
ADD_DEFINITIONS(-DPKGDATADIR='"${PROJECT_DATADIR}"')
CONFIGURE_FILE(src/chewing.xml.in src/chewing.xml)

#==================================================================
# Required packages
#
FIND_PACKAGE(Gettext)
FIND_PACKAGE(PkgConfig)
PKG_CHECK_MODULES(GTK2 gtk+-2.0)
PKG_CHECK_MODULES(IBUS ibus-1.0)
PKG_CHECK_MODULES(CHEWING chewing)


#==================================================================
# Source code subdirectories
#
ADD_SUBDIRECTORY(src bin)
ADD_SUBDIRECTORY(po)


#====================================================================
# Files to be install.
#
SET(MAIN_DOCS AUTHORS README ChangeLog NEWS COPYING)

INSTALL(FILES ${MAIN_DOCS} 
    DESTINATION "${PROJECT_DOCDIR}")

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
SET(PRJ_COMMON_IGNORE_FILES ${COMMON_IGNORE_FILES} ${RPM_IGNORE_FILES}
    "/docs/"  "/BUILD/" "/SOURCES/" "/SRPMS/" "/RPMS/"
)

SET(CPACK_SOURCE_IGNORE_FILES ${PRJ_COMMON_IGNORE_FILES} "/bin/" 
    ".cache$")
SET(CPACK_PACKAGE_IGNORE_FILES ${PRJ_COMMON_IGNORE_FILES} "/src/" 
    ".spec$" ".in$" )

INCLUDE(CPack)


#====================================================================
# Upload the package releases to hosting service provider (Source forge).
#

ADD_CUSTOM_TARGET(ChangeLog
    COMMAND echo "* ${DATE} - ${AUTHORS} <${MAINTAINER_EMAIL}> - ${PRJ_VER_FULL}" > ChangeLog
    COMMAND cat ChangeLog.in >> ChangeLog
    DEPENDS ChangeLog.in
    COMMENT "Making ChangeLog"
    VERBATIM
    )

ADD_DEPENDENCIES(pack_src ChangeLog)



