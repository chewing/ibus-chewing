# - Module that help to get/set variables.
# Defines the following macros:
#   COMMAND_OUTPUT_TO_VARIABLE(var cmd)
#     - Store command output to a variable, without new line characters (\n and \r).
#       This macro is suitable for command that output one line result.
#       Note that the var will be set to ${var_name}-NOVALUE if cmd does not have
#       any output.
#       * Parameters:
#         var: A variable that stores the result.
#         cmd: A command.
#
#   SETTING_FILE_GET_ATTRIBUTE(var attr_name setting_file [UNQUOTED] [setting_sign])
#     - Get an attribute value from a setting file.
#       * Parameters:
#         var: Variable to store the attribute value.
#         attr_name: Name of the attribute.
#         setting_file: Setting filename.
#         UNQUOTED: (Optional) remove the double quote mark around the string.
#         setting_sign: (Optional) The symbol that separate attribute name and its value.
#           Default value: "="
#
#   SETTING_FILE_GET_ALL_ATTRIBUTES(setting_file [UNQUOTED] [NOREPLACE] [setting_sign])
#     - Get all attribute values from a setting file.
#       '#' is used to comment out setting.
#       * Parameters:
#         setting_file: Setting filename.
#         UNQUOTED: (Optional) remove the double quote mark around the string.
#         NOREPLACE (Optional) Without this parameter, this macro replaces
#           previous defined variables, use NOREPLACE to prevent this.
#         setting_sign: (Optional) The symbol that separate attribute name and its value.
#           Default value: "="
#
#   GET_ENV(var default_value [env])
#     - Get the value of a environment variable, or use default
#       if the environment variable does not exist or is empty.
#       * Parameters:
#         var: Variable to be set
#         default_value: Default value of the var
#         env: (Optional) The name of environment variable. Only need if different from var.
#
#   SET_COMPILE_ENV(var default_value [env])
#     - Add compiler environment with a variable and its value.
#       If the variable is not defined yet, then a default value is assigned to it first.
#       * Parameters:
#         var: Variable to be set
#         default_value: Default value of the var
#         env: (Optional)The name of environment variable. Only need if different from var.
#
#   SET_VAR(var untrimmed_value)
#     - Trim an set the value to a variable.
#       * Parameters:
#         var: Variable to be set
#         untrimmed_value: Untrimmed values that may have space, \t, \n, \r in the front or back of the string.
#
INCLUDE(SupportCmake-2.4)
IF(NOT DEFINED _MANAGE_VARIABLE_CMAKE_)
    SET(_MANAGE_VARIABLE_CMAKE_ "DEFINED")

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
    SET(_find_pattern "\n[ \\t]*(${attr_name}\)[ \\t]*${setting_sign}\([^\n]*\)")

    FILE(READ ${setting_file} _txt_content)
    SET(_txt_content "\n${_txt_content}\n")
    # Escape ';'
    STRING(REPLACE ";" "\\;" _txt_content "${_txt_content}")
    STRING(REGEX MATCHALL "${_find_pattern}" _matched_lines "${_txt_content}")
    #MESSAGE("_matched_lines=|${_matched_lines}|")
    SET(_result_line)

    FOREACH(_line ${_matched_lines})
	#MESSAGE("### _line=|${_line}|")
	STRING(REGEX REPLACE "${_find_pattern}" "\\2" _result_line "${_line}")
	SET_VAR(${var} "${_result_line}")
    ENDFOREACH()
ENDMACRO(SETTING_FILE_GET_ATTRIBUTE var attr_name setting_file)

MACRO(SETTING_FILE_GET_ALL_ATTRIBUTES setting_file)
    SET(setting_sign "=")
    SET(_UNQUOTED "")
    SET(_NOREPLACE "")
    FOREACH(_arg ${ARGN})
	IF (${_arg} STREQUAL "UNQUOTED")
	    SET(_UNQUOTED "UNQUOTED")
	ELSEIF (${_arg} STREQUAL "NOREPLACE")
	    SET(_NOREPLACE "NOREPLACE")
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
	    IF ("${_NOREPLACE}" STREQUAL "" OR "${${_var}}" STREQUAL "" )
		MESSAGE("### _var=${_var} _result_line=|${_result_line}|")
		SET_VAR(${_var} "${_result_line}")
	    ELSE()
		SET(val ${${_var}})
		MESSAGE("### ${_var} is already defined as ${val}")
	    ENDIF()
	ENDIF()
    ENDFOREACH()
ENDMACRO(SETTING_FILE_GET_ALL_ATTRIBUTES setting_file)

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

MACRO(SET_COMPILE_ENV var default_value)
    IF(${ARGV2})
	SET(_env ${ARGV2})
    ELSE(${ARGV2})
	SET(_env ${var})
    ENDIF(${ARGV2})
    IF(NOT DEFINED ${var})
	SET(${var} "${default_value}")
    ENDIF(NOT DEFINED ${var})
    #MESSAGE("Variable Set to ${var},${_env}=${${var}}")
    ADD_DEFINITIONS(-D${_env}='"${${var}}"')
ENDMACRO(SET_COMPILE_ENV var default_value)

MACRO(SET_VAR var untrimmedValue)
    IF ("${untrimmedValue}" STREQUAL "")
	SET(${var} "")
    ELSE("${untrimmedValue}" STREQUAL "")
	STRING_TRIM(trimmedValue "${untrimmedValue}" ${_UNQUOTED})
	SET(${var} "${trimmedValue}")
    ENDIF("${untrimmedValue}" STREQUAL "")
    #SET(value "${${var}}")
    #MESSAGE("### ${var}=|${value}|")
ENDMACRO(SET_VAR var untrimmedValue)

ENDIF(NOT DEFINED _MANAGE_VARIABLE_CMAKE_)

