# - Module that manage version
# Defines following macros:
#   LOAD_RELEASE_FILE(releaseFile)
#   - Load release file information.
#     Arguments:
#     + releaseFile: release file to be read.
#       This file should contain following definition:
#       PRJ_VER: Release version.
#       SUMMARY: Summary of the release. Will be output as CHANGE_SUMMARY.
#       and a [Changes] section tag, below which listed the change in the
#       release.
#     This macro also set following variables:
#       PRJ_VER: Release version.
#       CHANGE_SUMMARY: Summary of changes.
#       CHANGELOG_ITEMS: Lines below the [Changes] tag.
#

IF(NOT DEFINED _MANAGE_VERSION_CMAKE_)
    SET(_MANAGE_VERSION_CMAKE_ "DEFINED")
    INCLUDE(ManageVariable)

    MACRO(LOAD_RELEASE_FILE releaseFile)
	#	SETTING_FILE_GET_ATTRIBUTE(PRJ_VER "PRJ_VER" "${releaseFile}")
	#SETTING_FILE_GET_ATTRIBUTE(CHANGE_SUMMARY "SUMMARY" "${releaseFile}")
	#SETTING_FILE_GET_ATTRIBUTE(UPDATE_TYPE "UPDATE_TYPE" "${releaseFile}")
	#SETTING_FILE_GET_ATTRIBUTE(REDHAT_BUGZILLA "UPDATE_TYPE" "${releaseFile}")
	COMMAND_OUTPUT_TO_VARIABLE(_grep_line grep -F "[Changes]" -n -m 1 ${releaseFile})
	#MESSAGE("_grep_line=|${_grep_line}|")
	IF("${_grep_line}" STREQUAL "")
	    MESSAGE(FATAL_ERROR "${releaseFile} does not have a [Changes] tag!")
	ENDIF("${_grep_line}" STREQUAL "")
	STRING(REGEX REPLACE ":.*" "" _line_num "${_grep_line}")
	MATH(EXPR _setting_line_num ${_line_num}-1)
	COMMAND_OUTPUT_TO_VARIABLE(_releaseFile_head head -n ${_setting_line_num} ${releaseFile})
	FILE(WRITE "${releaseFile}_NO_PACK" "${_releaseFile_head}")
	SETTING_FILE_GET_ALL_ATTRIBUTES("${releaseFile}_NO_PACK")

	MATH(EXPR _line_num ${_line_num}+1)

	COMMAND_OUTPUT_TO_VARIABLE(CHANGELOG_ITEMS tail -n +${_line_num} ${releaseFile})

	INCLUDE(DateTimeFormat)
	FILE(READ "ChangeLog.prev" CHANGELOG_PREV)
	CONFIGURE_FILE(ChangeLog.in ChangeLog)

	FILE(WRITE "ChangeLog" "* ${TODAY_CHANGELOG} - ${MAINTAINER} - ${PRJ_VER}\n")
	FILE(APPEND "ChangeLog" "${CHANGELOG_ITEMS}\n")
	FILE(READ "ChangeLog.prev" _changelog_prev)
	FILE(APPEND "ChangeLog" "${_changelog_prev}")

	# PRJ_VER won't be updated until the execution of cmake .
	SET(_version_check_cmd grep -e 'PRJ_VER=' ${RELEASE_FILE} |  tr -d '\\r\\n' | sed -e s/PRJ_VER=//)
	ADD_CUSTOM_TARGET(version_check
	    COMMAND ${CMAKE_COMMAND} -E echo "PRJ_VER=${PRJ_VER}"
	    COMMAND ${CMAKE_COMMAND} -E echo "Release file="`eval \"${_version_check_cmd}\"`
	    COMMAND test \"`${_version_check_cmd}`\" = \"\" -o \"`${_version_check_cmd}`\" = "${PRJ_VER}"
	    || echo Inconsistent version detected. Fixing.. && ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
	    )
    ENDMACRO(LOAD_RELEASE_FILE releaseFile)

ENDIF(NOT DEFINED _MANAGE_VERSION_CMAKE_)

