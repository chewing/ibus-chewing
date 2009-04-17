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
#
#-------------------------------------------------------------------
# SETTING_FILE_GET_ATTRIBUTE(var attr_name setting_file [setting_sign]):
#     var: Variable to store the attribute value.
#     attr_name: Name of the attribute.
#     setting_file: Setting filename.
#     setting_sign: (Optional) The symbol that separate attribute name and its value.
#         Default value: "="
#
# Get attribute value from a setting file. 
# New line characters will be stripped.
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

IF(NOT DEFINED _BASIC_MACROS_CMAKE_)
    SET(_BASIC_MACROS_CMAKE_ "DEFINED")
    IF(NOT DEFINED READ_TXT_CMD)
	SET(READ_TXT_CMD cat)
    ENDIF(NOT DEFINED READ_TXT_CMD)

    MACRO(STRING_TRIM var str)
	SET(_var_1 "\r${str}\r")
	STRING(REPLACE  "\r[ \t]*" "" _var_2 "${_var_1}" )
	STRING(REGEX REPLACE  "[ \t\r\n]*\r" "" ${var} "${_var_2}" )
    ENDMACRO(STRING_TRIM var str)

    MACRO(COMMAND_OUTPUT_TO_VARIABLE var cmd)
	EXECUTE_PROCESS(
	    COMMAND ${cmd} ${ARGN}
	    OUTPUT_VARIABLE _cmd_output
	    )
	STRING_TRIM(${var} ${_cmd_output})
    ENDMACRO(COMMAND_OUTPUT_TO_VARIABLE var cmd)

    MACRO(SETTING_FILE_GET_ATTRIBUTE var attr_name setting_file)
	IF (${ARGV3})
	    SET(setting_sign ${ARGV3})
	ELSE(${ARGV3})
	    SET(setting_sign "=")
	ENDIF(${ARGV3})
	SET(_find_pattern "${attr_name}[ \\t]*${setting_sign}.*")
	SET(_ignore_pattern "[ \\t]*${attr_name}[ \\t]*${setting_sign}")

	FILE(STRINGS ${setting_file} _txt_content)
	FOREACH(_line ${_txt_content})
	    STRING(REGEX MATCH "${_find_pattern}" _matched_line "${_line}")
	    IF (_matched_line)
		#	MESSAGE("### _matched_line=${_matched_line}")
		STRING(REGEX REPLACE "${_ignore_pattern}" "" _result_line "${_matched_line}")
		BREAK()
	    ENDIF(_matched_line)
	ENDFOREACH(_line)
	# MESSAGE("### _result_line=${_result_line}")
	IF(_result_line)
	    STRING_TRIM(${var} "${_result_line}")
	ELSE(_result_line)
	    SET(${var} "")
	ENDIF(_result_line)
    ENDMACRO(SETTING_FILE_GET_ATTRIBUTE var attr_name setting_file)

    MACRO(DATE_FORMAT date_var format)
	SET(_locale ${ARGV2})
	IF(_locale)
	    SET(ENV{LC_ALL} ${_locale})
	ENDIF(_locale)
	COMMAND_OUTPUT_TO_VARIABLE(${date_var} date "${format}")
    ENDMACRO(DATE_FORMAT date_var format)    
    
ENDIF(NOT DEFINED _BASIC_MACROS_CMAKE_)

