# BasicMacros: Macros which are needed by other modules.
#
# To use: INCLUDE(BasicMacros)
#
#===================================================================
# Macros:
# STRING_TRIM(var str)
#     var: A variable that stores the result.
#     str: A string.
#
# Trim a string. This macro is needed as CMake 2.4 does not support STRING(STRIP ..)
#-------------------------------------------------------------------
# COMMAND_OUTPUT_TO_VARIABLE(var cmd):
#     var: A variable that stores the result.
#     cmd: A command.
#
# Store command output to a variable, without new line characters (\n and \r).
# This macro is suitable for command that output one line result.
# Note that the var will be set to ${var_name}-NOVALUE if cmd does not have
# any output.
#
#-------------------------------------------------------------------
# SETTING_FILE_GET_ATTRIBUTE(var attr_name setting_file [UNQUOTED] [setting_sign]):
#     var: Variable to store the attribute value.
#     attr_name: Name of the attribute.
#     setting_file: Setting filename.
#     setting_sign: (Optional) The symbol that separate attribute name and its value.
#         Default value: "="
#
# Get attribute value from a setting file.
# New line characters will be stripped.
# Use "#" to comment out lines.
#
#-------------------------------------------------------------------
# SETTING_FILE_GET_ALL_ATTRIBUTES(setting_file [UNQUOTED] [setting_sign]):
#     setting_file: Setting filename.
#
# Get all attributes and corresponding from a setting file.
# New line characters will be stripped.
# Use "#" to comment out lines.
#
#-------------------------------------------------------------------
# DATE_FORMAT(date_var format [locale])
#     date_var: Result date string
#     format: date format for date(1)
#     locale: locale of the string.
#             Use current locale setting if locale is not given.
#
# Get date in specified format and locale.
#
#-------------------------------------------------------------------
# GET_ENV(var default_value [env])
#     var: Variable to be set
#     default_value: Default value of the var
#     env: The name of environment variable. Only need if different from var.
#
# Get the value of a environment variable, or use default
# if the environment variable does not exist or is empty.
#
#-------------------------------------------------------------------
# SET_ENV(var default_value [env])
#     var: Variable to be set
#     default_value: Default value of the var
#     env: The name of environment variable. Only need if different from var.
#
# Add compiler environment with a variable and its value.
# If the variable is not defined yet, then a default value is assigned to it first.
#

IF(NOT DEFINED _BASIC_MACROS_CMAKE_)
    SET(CMAKE_LOWEST_SUPPORTED_VERSION 2.4)
    SET(_BASIC_MACROS_CMAKE_ "DEFINED")
    IF(NOT DEFINED READ_TXT_CMD)
	SET(READ_TXT_CMD cat)
    ENDIF(NOT DEFINED READ_TXT_CMD)

    IF(CMAKE_VERSION)
	IF(CMAKE_VERSION VERSION_LESS 2.6)
	    MESSAGE("SET CMAKE_BACKWARDS_COMPATIBILITY ${CMAKE_VERSION}")
	    SET(CMAKE_BACKWARDS_COMPATIBILITY ${CMAKE_VERSION})
	ENDIF()
    ELSE()
	# CMAKE_VERSION may not available in 2.4
	MESSAGE("SET CMAKE_BACKWARDS_COMPATIBILITY ${CMAKE_LOWEST_SUPPORTED_VERSION}")
	SET(CMAKE_BACKWARDS_COMPATIBILITY ${CMAKE_LOWEST_SUPPORTED_VERSION})
	SET(CMAKE_VERSION ${CMAKE_LOWEST_SUPPORTED_VERSION})
    ENDIF()

    MACRO(STRING_TRIM var str)
	IF ("${ARGN}" STREQUAL "UNQUOTED")
	    SET(_unquoted 1)
	ELSE("${ARGN}" STREQUAL "UNQUOTED")
	    SET(_unquoted 0)
	ENDIF("${ARGN}" STREQUAL "UNQUOTED")
	SET(_var_1 "\r${str}\r")
	STRING(REPLACE  "\r[ \t]*" "" _var_2 "${_var_1}" )
	STRING(REGEX REPLACE  "[ \t\r\n]*\r" "" _var_3 "${_var_2}" )
	IF (${_unquoted})
	    STRING(REGEX REPLACE "^\"" "" _var_4 "${_var_3}" )
	    STRING(REGEX REPLACE "\"$" "" ${var} "${_var_4}" )
	ELSE(${_unquoted})
	    SET(${var} "${_var_3}")
        ENDIF(${_unquoted})
    ENDMACRO(STRING_TRIM var str)

    MACRO(COMMAND_OUTPUT_TO_VARIABLE var cmd)
	EXECUTE_PROCESS(
	    COMMAND ${cmd} ${ARGN}
	    OUTPUT_VARIABLE _cmd_output
	    )
	IF(_cmd_output)
	    STRING_TRIM(${var} ${_cmd_output})
	ELSE(_cmd_output)
	    SET(var "${var}-NOVALUE")
	ENDIF(_cmd_output)
	# MESSAGE("var=${var} _cmd_output=${_cmd_output}")
    ENDMACRO(COMMAND_OUTPUT_TO_VARIABLE var cmd)

    MACRO(SETTING_FILE_GET_ATTRIBUTE var attr_name setting_file)
	SET(setting_sign "=")
	SET(_UNQUOTED "")
	FOREACH(_arg ${ARGN})
	    IF (${_arg} STREQUAL "UNQUOTED")
		SET(_UNQUOTED "UNQUOTED")
	    ELSE(${_arg} STREQUAL "UNQUOTED")
	        SET(setting_sign ${_arg})
	    ENDIF(${_arg} STREQUAL "UNQUOTED")
	ENDFOREACH(_arg)
	SET(_find_pattern "\n[^#][ \\t]*${attr_name}[ \\t]*${setting_sign}[^\n]*")
	SET(_strip_pattern "\n[ \\t]*${attr_name}[ \\t]*${setting_sign}")

	FILE(READ ${setting_file} _txt_content)
	SET(_txt_content "\n${_txt_content}\n")
	STRING(REGEX MATCHALL "${_find_pattern}" _matched_lines "${_txt_content}")
	#MESSAGE("attr_name=${attr_name} _matched_lines=|${_matched_lines}|")
	SET(_result_line)

	# Last line should get priority.
	FOREACH(_line ${_matched_lines})
	    STRING(REGEX REPLACE "${_strip_pattern}" "" _result_line "${_line}")
	ENDFOREACH()
	#MESSAGE("### _result_line=|${_result_line}|")
	IF ("${_result_line}" STREQUAL "")
	    SET(${var} "")
	ELSE("${_result_line}" STREQUAL "")
	    STRING_TRIM(${var} "${_result_line}" ${_UNQUOTED})
	ENDIF("${_result_line}" STREQUAL "")
	#SET(value "${${var}}")
	#MESSAGE("### ${var}=|${value}|")
    ENDMACRO(SETTING_FILE_GET_ATTRIBUTE var attr_name setting_file)

    MACRO(SETTING_FILE_GET_ALL_ATTRIBUTES setting_file)
	SET(setting_sign "=")
	SET(_UNQUOTED "")
	FOREACH(_arg ${ARGN})
	    IF (${_arg} STREQUAL "UNQUOTED")
		SET(_UNQUOTED "UNQUOTED")
	    ELSE(${_arg} STREQUAL "UNQUOTED")
		SET(setting_sign ${_arg})
	    ENDIF(${_arg} STREQUAL "UNQUOTED")
	ENDFOREACH(_arg)
	SET(_find_pattern "\n[ \\t]*([A-Za-z0-9_]*\)[ \\t]*${setting_sign}\([^\n]*\)")

	FILE(READ ${setting_file} _txt_content)
	SET(_txt_content "\n${_txt_content}\n")

	# Escape ';'
	STRING(REPLACE ";" "\\;" _txt_content "${_txt_content}")

	STRING(REGEX MATCHALL "${_find_pattern}" _matched_lines "${_txt_content}")

	#MESSAGE("_matched_lines=|${_matched_lines}|")
	SET(_result_line)
	SET(_var)

	FOREACH(_line ${_matched_lines})
	    #MESSAGE("### _line=|${_line}|")
	    STRING(REGEX REPLACE "${_find_pattern}" "\\1" _var "${_line}")
	    STRING(REGEX REPLACE "${_find_pattern}" "\\2" _result_line "${_line}")
	    IF ( NOT "${_var}" STREQUAL "")
		#MESSAGE("### _var=${_var} _result_line=|${_result_line}|")
		IF ("${_result_line}" STREQUAL "")
		    IF (${CMAKE_VERSION} EQUAL ${CMAKE_LOWEST_SUPPORTED_VERSION})
			SET(${_var} "")
		    ELSE()
			SET(${_var} "" PARENT_SCOPE)
		    ENDIF()
		ELSE("${_result_line}" STREQUAL "")
		    STRING_TRIM(_result_line_striped "${_result_line}" ${_UNQUOTED})
		    IF (${CMAKE_VERSION} EQUAL ${CMAKE_LOWEST_SUPPORTED_VERSION})
			SET(${_var} "${_result_line_striped}")
		    ELSE()
			SET(${_var} "${_result_line_striped}")
		    ENDIF()
		ENDIF("${_result_line}" STREQUAL "")
		SET(value "${${_var}}")
		MESSAGE("### ${_var}=|${value}|")
	    ENDIF()

	ENDFOREACH()
    ENDMACRO(SETTING_FILE_GET_ALL_ATTRIBUTES setting_file)

    MACRO(DATE_FORMAT date_var format)
	SET(_locale ${ARGV2})
	IF(_locale)
	    SET(ENV{LC_ALL} ${_locale})
	ENDIF(_locale)
	COMMAND_OUTPUT_TO_VARIABLE(${date_var} date "${format}")
    ENDMACRO(DATE_FORMAT date_var format)

    MACRO(GET_ENV var default_value)
	IF(${ARGV2})
	    SET(_env "${ARGV2}")
	ELSE()
	    SET(_env "${var}")
	ENDIF()
	IF ("$ENV{${_env}}" STREQUAL "")
	    SET(${var} "${default_value}")
	ELSE()
	    SET(${var} "$ENV{${_env}}")
	ENDIF()
	# MESSAGE("Variable ${var}=${${var}}")
    ENDMACRO(GET_ENV var default_value)

    MACRO(SET_ENV var default_value)
	IF(${ARGV2})
	    SET(_env ${ARGV2})
	ELSE()
	    SET(_env ${var})
	ENDIF()
	IF(NOT DEFINED ${var})
	    SET(${var} "${default_value}")
	ENDIF(NOT DEFINED ${var})
	#MESSAGE("Variable Set to ${var},${_env}=${${var}}")
	ADD_DEFINITIONS(-D${_env}='"${${var}}"')
    ENDMACRO(SET_ENV var default_value)

ENDIF(NOT DEFINED _BASIC_MACROS_CMAKE_)

