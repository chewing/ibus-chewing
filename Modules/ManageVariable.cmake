# - Get or set variables from various sources.
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
#   SETTING_FILE_GET_VARIABLES_PATTERN(var attr_pattern setting_file [NOUNQUOTE] [NOREPLACE]
#     [NOESCAPE_SEMICOLON] [setting_sign])
#     - Get variable values from a setting file if their names matches given
#       pattern. '#' is used for comment.
#       * Parameters:
#         + var: Variable to store the attribute value.
#           Set to "" to set attribute under matched variable name.
#         + attr_pattern: Regex pattern of variable name.
#         + setting_file: Setting filename.
#         + NOUNQUOTE: (Optional) do not remove the double quote mark around the string.
#         + NOREPLACE (Optional) Without this parameter, this macro replaces
#           previous defined variables, use NOREPLACE to prevent this.
#         + NOESCAPE_SEMICOLON: (Optional) do not escape semicolons.
#         + setting_sign: (Optional) The symbol that separate attribute name and its value.
#           Default value: "="
#
#   SETTING_FILE_GET_ALL_VARIABLES(setting_file [NOUNQUOTE] [NOREPLACE]
#     [NOESCAPE_SEMICOLON] [setting_sign])
#     - Get all variable values from a setting file.
#       It is equivalent to:
#       SETTING_FILE_GET_VARIABLES_PATTERN("" "[A-Za-z_][A-Za-z0-9_]*"
#        "${setting_file}" ${ARGN})
#      '#' is used to comment out setting.
#       * Parameters:
#         + setting_file: Setting filename.
#         + NOUNQUOTE: (Optional) do not remove the double quote mark around the string.
#         + NOREPLACE (Optional) Without this parameter, this macro replaces
#           previous defined variables, use NOREPLACE to prevent this.
#         + NOESCAPE_SEMICOLON: (Optional) Do not escape semicolons.
#         + setting_sign: (Optional) The symbol that separate attribute name and its value.
#           Default value: "="
#
#   SETTING_FILE_GET_VARIABLE(var attr_name setting_file [NOUNQUOTE] [NOREPLACE]
#     [NOESCAPE_SEMICOLON] [setting_sign])
#     - Get a variable value from a setting file.
#       It is equivalent to:
#	SETTING_FILE_GET_VARIABLES_PATTERN(${var} "${attr_name}"
#	    "${setting_file}" ${ARGN})
#      '#' is used to comment out setting.
#       * Parameters:
#         + var: Variable to store the attribute value.
#         + attr_name: Name of the variable.
#         + setting_file: Setting filename.
#         + NOUNQUOTE: (Optional) do not remove the double quote mark around the string.
#         + NOREPLACE (Optional) Without this parameter, this macro replaces
#           previous defined variables, use NOREPLACE to prevent this.
#         + NOESCAPE_SEMICOLON: (Optional) do not escape semicolons.
#         + setting_sign: (Optional) The symbol that separate attribute name and its value.
#           Default value: "="
#
#   SETTING_FILE_GET_ALL_VARIABLES(setting_file [NOUNQUOTE] [NOREPLACE]
#     [NOESCAPE_SEMICOLON] [setting_sign])
#     - Get all attribute values from a setting file.
#       '#' is used to comment out setting.
#       * Parameters:
#         + setting_file: Setting filename.
#         + NOUNQUOTE: (Optional) do not remove the double quote mark around the string.
#         + NOREPLACE (Optional) Without this parameter, this macro replaces
#           previous defined variables, use NOREPLACE to prevent this.
#         + NOESCAPE_SEMICOLON: (Optional) Do not escape semicolons.
#         + setting_sign: (Optional) The symbol that separate attribute name and its value.
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
    INCLUDE(ManageString)

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

    # This macro is meant to be internal.
    MACRO(_MANAGE_VARIABLE_SET var value)
	SET(${var} "${value}")
    ENDMACRO(_MANAGE_VARIABLE_SET var value)

    # This macro is meant to be internal.
    # it deals the "encoded" line.
    MACRO(SETTING_FILE_LINE_PARSE attr value setting_sign str  _noUnQuoted )
	STRING_SPLIT(_tokens "${setting_sign}" "${str}" 2)
	SET(_index 0)
	FOREACH(_token ${_tokens})
	    IF(_index EQUAL 0)
		SET(${attr} "${_token}")
	    ELSE(_index EQUAL 0)
		SET_VAR(${value} "${_token}")
	    ENDIF(_index EQUAL 0)
	    MATH(EXPR _index ${_index}+1)
	ENDFOREACH(_token ${_tokens})
	#SET(val ${${value}})
	#MESSAGE("SETTING_FILE_LINE_PARSE: val=${val}")
    ENDMACRO(SETTING_FILE_LINE_PARSE attr value setting_sign str  _noUnQuoted)

    # Internal macro
    # Similar to STRING_ESCAPE, but read directly from file,
    # This avoid the variable substitution
    # Variable escape is enforced.
    MACRO(FILE_READ_ESCAPE var filename)
	# '$' is very tricky.
	# '$' => '#D'
	GET_FILENAME_COMPONENT(_filename_abs "${filename}" ABSOLUTE)
	FILE(READ "${_filename_abs}" _ret)
	STRING(REGEX REPLACE "[$]" "#D" _ret "${_ret}")
	STRING_ESCAPE(${var} "${_ret}" ${ARGN})
    ENDMACRO(FILE_READ_ESCAPE var filename)

    MACRO(SETTING_FILE_GET_VARIABLES_PATTERN var attr_pattern setting_file)
	SET(setting_sign "=")
	SET(_noUnQuoted "")
	SET(_noEscapeSemicolon "")
	SET(_noReplace "")
	SET(_escapeVariable "")
	FOREACH(_arg ${ARGN})
	    IF (${_arg} STREQUAL "UNQUOTED")
		SET(_noUnQuoted "UNQUOTED")
	    ELSEIF (${_arg} STREQUAL "NOREPLACE")
		SET(_noReplace "NOREPLACE")
	    ELSEIF (${_arg} STREQUAL "NOESCAPE_SEMICOLON")
		SET(_noEscapeSemicolon "NOESCAPE_SEMICOLON")
	    ELSEIF (${_arg} STREQUAL "ESCAPE_VARIABLE")
		SET(_escapeVariable "ESCAPE_VARIABLE")
	    ELSE(${_arg} STREQUAL "UNQUOTED")
		SET(setting_sign ${_arg})
	    ENDIF(${_arg} STREQUAL "UNQUOTED")
	ENDFOREACH(_arg)

	# Escape everything to be safe.
	FILE_READ_ESCAPE(_txt_content "${setting_file}")

	STRING_SPLIT(_lines "\n" "${_txt_content}")
	#MESSAGE("_lines=|${_lines}|")
	FOREACH(_line ${_lines})
	    #MESSAGE("_line=|${_line}|")
	    IF(_line MATCHES "[ \\t]*${attr_pattern}[ \\t]*${setting_sign}")
		#MESSAGE("*** matched_line=|${_line}|")
		SETTING_FILE_LINE_PARSE(_attr _value ${setting_sign}
		    "${_line}" "${_noUnQuoted}" )
		IF(_noReplace STREQUAL "" OR NOT DEFINED ${_attr})
		    # Unencoding
		    # Note content is escaped twice.
		    STRING_UNESCAPE(_value "${_value}" ${_noEscapeSemicolon} ESCAPE_VARIABLE)
		    STRING_UNESCAPE(_value "${_value}" ${_noEscapeSemicolon} ESCAPE_VARIABLE)
		    IF(_escapeVariable STREQUAL "")
			# Variable should not be escaped
			# i.e. need substitution
			_MANAGE_VARIABLE_SET(_value "${_value}")
		    ENDIF(_escapeVariable STREQUAL "")
		    IF("${var}" STREQUAL "")
			SET(${_attr} "${_value}")
		    ELSE("${var}" STREQUAL "")
			SET(${var} "${_value}")
		    ENDIF("${var}" STREQUAL "")
		ENDIF(_noReplace STREQUAL "" OR NOT DEFINED ${_attr})
	    ENDIF(_line MATCHES "[ \\t]*${attr_pattern}[ \\t]*${setting_sign}")
	ENDFOREACH(_line ${_lines})
	#SET(${var} "${_value}")

    ENDMACRO(SETTING_FILE_GET_VARIABLES_PATTERN var attr_pattern setting_file)

    MACRO(SETTING_FILE_GET_VARIABLE var attr_name setting_file)
	SETTING_FILE_GET_VARIABLES_PATTERN(${var} "${attr_name}"
	    "${setting_file}" ${ARGN})
    ENDMACRO(SETTING_FILE_GET_VARIABLE var attr_name setting_file)

    MACRO(SETTING_FILE_GET_ALL_VARIABLES setting_file)
	SETTING_FILE_GET_VARIABLES_PATTERN("" "[A-Za-z_][A-Za-z0-9_]*"
	    "${setting_file}" ${ARGN})
    ENDMACRO(SETTING_FILE_GET_ALL_VARIABLES setting_file)

    MACRO(GET_ENV var default_value)
	IF(${ARGC} GREATER 2)
	    SET(_env "${ARGV2}")
	ELSE(${ARGC} GREATER 2)
	    SET(_env "${var}")
	ENDIF(${ARGC} GREATER 2)

	IF ("$ENV{${_env}}" STREQUAL "")
	    SET(${var} "${default_value}")
	ELSE("$ENV{${_env}}" STREQUAL "")
	    SET(${var} "$ENV{${_env}}")
	ENDIF("$ENV{${_env}}" STREQUAL "")
	# MESSAGE("Variable ${var}=${${var}}")
    ENDMACRO(GET_ENV var default_value)

    MACRO(SET_VAR var untrimmedValue)
	SET(_noUnQuoted "")
	FOREACH(_arg ${ARGN})
	    IF (${_arg} STREQUAL "UNQUOTED")
		SET(_noUnQuoted "UNQUOTED")
	    ENDIF(${_arg} STREQUAL "UNQUOTED")
	ENDFOREACH(_arg)
	IF ("${untrimmedValue}" STREQUAL "")
	    SET(${var} "")
	ELSE("${untrimmedValue}" STREQUAL "")
	    STRING_TRIM(trimmedValue "${untrimmedValue}" ${_noUnQuoted})
	    SET(${var} "${trimmedValue}")
	ENDIF("${untrimmedValue}" STREQUAL "")
	#SET(value "${${var}}")
	#MESSAGE("### ${var}=|${value}|")
    ENDMACRO(SET_VAR var untrimmedValue)

ENDIF(NOT DEFINED _MANAGE_VARIABLE_CMAKE_)

