# Version: Handle version
#
# To use: INCLUDE(Version)
# Includes: BasicMacros
#
#===================================================================
# Variables:
#  RELEASE_FILE: File that contain version information
#  PRJ_VER_PATTERN: Version for source tarball, program display, etc.
#         Default: ${PRJ_VER_MAJOR}.${PRJ_VER_MINOR}.${PRJ_VER_PATCH})
#  PRJ_VER_FULL: Version for rpms.
#         Default: ${PRJ_VER}-${PRJ_VER_RELEASE})
#===================================================================
# Targets:
# version_check:
#     Check and fix the inconsistent version.
#

IF(NOT DEFINED _VERSION_CMAKE_)
    SET(_VERSION_CMAKE_ "DEFINED")
    IF(NOT DEFINED RELEASE_FILE)
	SET(RELEASE_FILE RELEASE-NOTES)
    ENDIF(NOT DEFINED RELEASE_FILE)
    INCLUDE(BasicMacros)

    IF(EXISTS ${RELEASE_FILE})
        SETTING_FILE_GET_ATTRIBUTE(_prj_ver "PRJ_VER" ${RELEASE_FILE})
	IF(_prj_ver)
	    SET(PRJ_VER_PATTERN ${_prj_ver} CACHE INTERNAL "PRJ_VER")
	ENDIF(_prj_ver)
	# MESSAGE("### -1 PRJ_VER=${PRJ_VER}")
    ENDIF(EXISTS ${RELEASE_FILE})

    IF(NOT DEFINED PRJ_VER_PATTERN)
        SET(PRJ_VER ${PRJ_VER_MAJOR}.${PRJ_VER_MINOR}.${PRJ_VER_PATCH} CACHE
	    INTERNAL "PRJ_VER")
    ELSE(NOT DEFINED PRJ_VER_PATTERN)
	SET(PRJ_VER ${PRJ_VER_PATTERN} CACHE INTERNAL "PRJ_VER")
    ENDIF(NOT DEFINED PRJ_VER_PATTERN)


    IF(NOT DEFINED PRJ_VER_FULL)
        SET(PRJ_VER_FULL  ${PRJ_VER}-${PRJ_VER_RELEASE})
    ENDIF(NOT DEFINED PRJ_VER_FULL)

    SET(_version_check_ver grep -e 'PRJ_VER=' ${RELEASE_FILE} |  tr -d '\\r\\n' | sed -e s/PRJ_VER=//)
    ADD_CUSTOM_TARGET(version_check
	COMMAND ${CMAKE_COMMAND} -E echo "PRJ_VER=${PRJ_VER}"
	COMMAND ${CMAKE_COMMAND} -E echo "Release file="`eval \"${_version_check_ver}\"`
	COMMAND test \"`${_version_check_ver}`\" = \"\" -o \"`${_version_check_ver}`\" = "${PRJ_VER}"
	|| echo Inconsistent version detected. Fixing.. && ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
	)

ENDIF(NOT DEFINED _VERSION_CMAKE_)

