# Version: Handle version 
#
# To use: INCLUDE(Version)
# Includes: BasicMacros
#
#===================================================================
# Variables: 
#  PRJ_VER: Version for source tarball, program display, etc.
#         Default: ${PRJ_VER_MAJOR}.${PRJ_VER_MINOR}.${PRJ_VER_PATCH})
#  PRJ_VER_FULL: Version for rpms.
#         Default: ${PRJ_VER}-${PRJ_VER_RELEASE})
#

IF(NOT DEFINED _VERSION_CMAKE_)
    SET(_VERSION_CMAKE_)

    SET(_version_lock_file _version_lock)
    INCLUDE(BasicMacros)

    IF(EXISTS ${_version_lock_file})
        SETTING_FILE_GET_ATTRIBUTE(_prj_ver "PRJ_VER" ${_version_lock_file})
	IF(_prj_ver)
	    SET(PRJ_VER ${_prj_ver})
	ENDIF(_prj_ver)
	# MESSAGE("### -1 PRJ_VER=${PRJ_VER}")
    ENDIF(EXISTS ${_version_lock_file})

    IF(NOT DEFINED PRJ_VER)
        SET(PRJ_VER ${PRJ_VER_MAJOR}.${PRJ_VER_MINOR}.${PRJ_VER_PATCH})
    ENDIF(NOT DEFINED PRJ_VER)

    IF(NOT DEFINED PRJ_VER_FULL)
        SET(PRJ_VER_FULL  ${PRJ_VER}-${PRJ_VER_RELEASE})
    ENDIF(NOT DEFINED PRJ_VER_FULL)

ENDIF(NOT DEFINED _VERSION_CMAKE_)

