# Version: Set version varibles.
#
# Include: INCLUDE(DateFormat)
#
#===================================================================
# Variables: 
#  PRJ_VER: Version for source tarball, program display, etc.
#         Default: ${PRJ_VER_MAJOR}.${PRJ_VER_MINOR}.${PRJ_VER_PATCH})
#  PRJ_VER_FULL: Version for rpms.
#         Default: ${PRJ_VER}-${PRJ_VER_RELEASE})
# 

IF(NOT DEFINED PRJ_VER)
    SET(PRJ_VER ${PRJ_VER_MAJOR}.${PRJ_VER_MINOR}.${PRJ_VER_PATCH})
ENDIF()

IF(NOT DEFINED PRJ_VER_FULL)
    SET(PRJ_VER_FULL  ${PRJ_VER}-${PRJ_VER_RELEASE})
ENDIF()


