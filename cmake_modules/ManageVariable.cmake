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
#   SETTING_FILE_GET_ATTRIBUTE(var attr_name setting_file [UNQUOTED]
#     [NOESCAPE_SEMICOLON] [setting_sign])
#     - Get an attribute value from a setting file.
#       * Parameters:
#         var: Variable to store the attribute value.
#         attr_name: Name of the attribute.
#         setting_file: Setting filename.
#         UNQUOTED: (Optional) remove the double quote mark around the string.
#         NOESCAPE_SEMICOLON: Escape semicolons.
#         setting_sign: (Optional) The symbol that separate attribute name and its value.
#           Default value: "="
#
#   SETTING_FILE_GET_ALL_ATTRIBUTES(setting_file [UNQUOTED] [NOREPLACE]
#     [NOESCAPE_SEMICOLON] [setting_sign])
#     - Get all attribute values from a setting file.
#       '#' is used to comment out setting.
#       * Parameters:
#         setting_file: Setting filename.
#         UNQUOTED: (Optional) remove the double quote mark around the string.
#         NOREPLACE (Optional) Without this parameter, this macro replaces
#           previous defined variables, use NOREPLACE to prevent this.
#         NOESCAPE_SEMICOLON: Escape semicolons.
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
#   SET_VAR(var untrimmed_value)
#     - Trim an set the value to a variable.
#       * Parameters:
#         var: Variable to be set
#         untrimmed_value: Untrimmed values that may have space, \t, \n, \r in the front or back of the string.
#
IF(NOT DEFINED _MANAGE_VARIABLE_CMAKE_)
    SET(_MANAGE_VARIABLE_CMAKE_ "DEFINED")
    INCLUDE(SupportCmake-2.4)

    MACRO(COMMAND_OUTPUT_TO_VARIABLE var cmd)
	EXECUTE_PROCESS(
	    COMMAND ${cmd} ${ARGN}
	    OUTPUT_VARIABLE _cmd_output
	    OUTPUT_STRIP_TRAILING_WHITESPACE
	    )
	IF(_cmd_output)
	    SET(${var} ${_cmd_output})
	ELSE(_cmd_output)
	    SET(var "${var}-NOVALUE")
	ENDIF(_cmd_output)
	#SET(value ${${var}})
	#MESSAGE("var=${var} _cmd_output=${_cmd_output} value=|${value}|" )
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
	SET(_NOESCAPE_SEMICOLON "")
	SET(SED "sed")
	FOREACH(_arg ${ARGN})
	    IF (${_arg} STREQUAL "UNQUOTED")
		SET(_UNQUOTED "UNQUOTED")
	    ELSEIF (${_arg} STREQUAL "NOREPLACE")
		SET(_NOREPLACE "NOREPLACE")
	    ELSEIF (${_arg} STREQUAL "NOESCAPE_SEMICOLON")
		SET(_NOESCAPE_SEMICOLON "NOESCAPE_SEMICOLON")
	    ELSE(${_arg} STREQUAL "UNQUOTED")
		SET(setting_sign ${_arg})
	    ENDIF(${_arg} STREQUAL "UNQUOTED")
	ENDFOREACH(_arg)
	SET(_find_pattern "\n[ \\t]*([A-Za-z0-9_]*\)[ \\t]*${setting_sign}\([^\n]*\)")

	IF(NOT _NOESCAPE_SEMICOLON STREQUAL "")
	    FILE(READ "${setting_file}" _txt_content)
	ELSE(NOT _NOESCAPE_SEMICOLON STREQUAL "")
	    EXECUTE_PROCESS(COMMAND sed -e s/a/+/ "${setting_file}"
	    	OUTPUT_VARIABLE _txt_content
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	    COMMAND_OUTPUT_TO_VARIABLE(_txt_content sed -e 's/;/+/' ${setting_file})
	ENDIF(NOT _NOESCAPE_SEMICOLON STREQUAL "")

	MESSAGE("1_txt_content=|${_txt_content}|")
	SET(_txt_content "\n${_txt_content}\n")

	MESSAGE("_txt_content=|${_txt_content}|")
	# Escape ';'
	IF(NOT _ESCAPE_SEMICOLON STREQUAL "")
	    STRING(REGEX REPLACE ";" "\\;" _txt_content "${_txt_content}")
	ENDIF(NOT _ESCAPE_SEMICOLON STREQUAL "")

	STRING(REGEX MATCHALL "${_find_pattern}" _matched_lines "${_txt_content}")

	MESSAGE("_matched_lines=|${_matched_lines}|")
	SET(_result_line)
	SET(_var)

	FOREACH(_line ${_matched_lines})
	    #MESSAGE("### _line=|${_line}|")
	    STRING(REGEX REPLACE "${_find_pattern}" "\\1" _var "${_line}")
	    STRING(REGEX REPLACE "${_find_pattern}" "\\2" _result_line "${_line}")
	    IF ( NOT "${_var}" STREQUAL "")
		IF ("${_NOREPLACE}" STREQUAL "" OR "${${_var}}" STREQUAL "" )
		    #MESSAGE("### _var=${_var} _result_line=|${_result_line}|")
		    SET_VAR(${_var} "${_result_line}")
		ELSE()
		    SET(val ${${_var}})
		    MESSAGE("### ${_var} is already defined as ${val}")
		ENDIF()
	    ENDIF()
	ENDFOREACH()
    ENDMACRO(SETTING_FILE_GET_ALL_ATTRIBUTES setting_file)

    MACRO(GET_ENV var default_value)
	IF(${ARGC} GREATER 2)
	    SET(_env "${ARGV2}")
	ELSE(${ARGC} GREATER 2)
	    SET(_env "${var}")
	ENDIF(${ARGC} GREATER 2)
	IF ("$ENV{${_env}}" STREQUAL "")
	    SET(${var} "${default_value}")
	ELSE()
	    SET(${var} "$ENV{${_env}}")
	ENDIF()
	# MESSAGE("Variable ${var}=${${var}}")
    ENDMACRO(GET_ENV var default_value)

    MACRO(SET_VAR var untrimmedValue)
	SET(_UNQUOTED "")
	FOREACH(_arg ${ARGN})
	    IF (${_arg} STREQUAL "UNQUOTED")
		SET(_UNQUOTED "UNQUOTED")
	    ENDIF(${_arg} STREQUAL "UNQUOTED")
	ENDFOREACH(_arg)
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

