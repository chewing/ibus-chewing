# - Module that manage version
# Defines following macros:
#   LOAD_RELEASE_FILE(filename)
#   - Load release file information, this file should have at least one line
#     that shows:
#     PRJ_VER=<version>
#     This macro also set following variables:
#       PRJ_VER: Project version.
#       CHANGE_SUMMARY: Summary of changes.
#       CHANGELOG_ITEMS: Items for change logs
#

IF(NOT DEFINED _MANAGE_VERSION_CMAKE_)
    SET(_MANAGE_VERSION_CMAKE_ "DEFINED")
    INCLUDE(ManageVariable)
    MACRO(LOAD_RELEASE_FILE filename)
	SETTING_FILE_GET_ATTRIBUTE(PRJ_VER "PRJ_VER" "${filename}")
	SETTING_FILE_GET_ATTRIBUTE(CHANGE_SUMMARY "SUMMARY" "${filename}")
	COMMAND_OUTPUT_TO_VARIABLE(_grep_line grep -F "[Changes]" -n -m 1 ${filename})
	#MESSAGE("_grep_line=|${_grep_line}")
	STRING(REGEX REPLACE ":.*" "" _line_num "${_grep_line}")
	MATH(EXPR _line_num ${_line_num}+1)

	COMMAND_OUTPUT_TO_VARIABLE(CHANGELOG_ITEMS tail -n +${_line_num} ${filename})
	INCLUDE(DateTimeFormat)
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
    ENDMACRO(LOAD_RELEASE_FILE filename)

ENDIF(NOT DEFINED _MANAGE_VERSION_CMAKE_)

