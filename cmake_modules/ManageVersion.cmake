# - Module that manage version
# Defines following macros:
#   LOAD_RELEASE_FILE(filename)
#   - Load release file information, this file should have at least one line
#     that shows:
#     PRJ_VER=<version>
#     This macro also set a variable:
#       PRJ_VER: Project version.
#

IF(NOT DEFINED _MANAGE_VERSION_CMAKE_)
    SET(_MANAGE_VERSION_CMAKE_ "DEFINED")
    INCLUDE(ManageVariable)
    MACRO(LOAD_RELEASE_FILE filename)
	SETTING_FILE_GET_ATTRIBUTE(${_prj_ver} "PRJ_VER" ${fliename})
	SET(PRJ_VER ${_prj_ver})
	#IF(_prj_ver)
	#    SET(PRJ_VER_PATTERN ${_prj_ver} CACHE INTERNAL "PRJ_VER")
	#ENDIF(_prj_ver)
	# MESSAGE("### -1 PRJ_VER=${PRJ_VER}")
    ENDMACRO(LOAD_RELEASE_FILE filename)

    #    SET(_version_check_ver grep -e 'PRJ_VER=' ${RELEASE_FILE} |  tr -d '\\r\\n' | sed -e s/PRJ_VER=//)
    #ADD_CUSTOM_TARGET(version_check
    #	COMMAND ${CMAKE_COMMAND} -E echo "PRJ_VER=${PRJ_VER}"
#   	COMMAND ${CMAKE_COMMAND} -E echo "Release file="`eval \"${_version_check_ver}\"`
#	COMMAND test \"`${_version_check_ver}`\" = \"\" -o \"`${_version_check_ver}`\" = "${PRJ_VER}"
#	|| echo Inconsistent version detected. Fixing.. && ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
#	)

ENDIF(NOT DEFINED _MANAGE_VERSION_CMAKE_)

