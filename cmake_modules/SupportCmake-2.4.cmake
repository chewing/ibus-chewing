# - SupportCmake-2.4: Supporter module for cmake 2.4.
# Defines the following macros:
#   STRING_TRIM(var str [UNQUOTED])
#     - Trim a string. This macro is needed as CMake 2.4 does not support
#       STRING(STRIP ..)
#       * Parameters:
#          var: A variable that stores the result.
#          str: A string.
#          UNQUOTED: (Optional) remove the double quote mark around the string.
# This module is mainly for provide an unified environment for CMake 2.4
# and up. You can use this module even you are normally with higher version
# such like 2.6.
#
IF(NOT DEFINED _SUPPORT_CMAKE_2.4_CMAKE_)
    SET(_SUPPORT_CMAKE_2.4_CMAKE_ "DEFINED")
    SET(CMAKE_LOWEST_SUPPORTED_VERSION 2.4)

    IF(CMAKE_VERSION)
	IF(CMAKE_VERSION VERSION_LESS 2.6)
	    MESSAGE("SET CMAKE_BACKWARDS_COMPATIBILITY ${CMAKE_VERSION}")
	    SET(CMAKE_BACKWARDS_COMPATIBILITY ${CMAKE_VERSION})
	ENDIF(CMAKE_VERSION VERSION_LESS 2.6)
    ELSE(CMAKE_VERSION)
	# CMAKE_VERSION may not available in 2.4
	# MESSAGE("SET CMAKE_BACKWARDS_COMPATIBILITY ${CMAKE_LOWEST_SUPPORTED_VERSION}")
	SET(CMAKE_BACKWARDS_COMPATIBILITY ${CMAKE_LOWEST_SUPPORTED_VERSION})
	SET(CMAKE_VERSION ${CMAKE_LOWEST_SUPPORTED_VERSION})
    ENDIF(CMAKE_VERSION)

    MACRO(STRING_TRIM var str)
	IF ("${ARGN}" STREQUAL "UNQUOTED")
	    SET(_unquoted 1)
	ELSE("${ARGN}" STREQUAL "UNQUOTED")
	    SET(_unquoted 0)
	ENDIF("${ARGN}" STREQUAL "UNQUOTED")
	SET(_var_1 "+${str}+")
	STRING(REGEX REPLACE  "^[+][ \t\r\n]*" "" _var_2 "${_var_1}" )
	STRING(REGEX REPLACE  "[ \t\r\n]*[+]$" "" _var_3 "${_var_2}" )
	IF (${_unquoted})
	    STRING(REGEX REPLACE "^\"" "" _var_4 "${_var_3}" )
	    STRING(REGEX REPLACE "\"$" "" ${var} "${_var_4}" )
	ELSE(${_unquoted})
	    SET(${var} "${_var_3}")
	ENDIF(${_unquoted})
    ENDMACRO(STRING_TRIM var str)
ENDIF(NOT DEFINED _SUPPORT_CMAKE_2.4_CMAKE_)

