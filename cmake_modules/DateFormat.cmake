# Date format: Get date in specified format and locale.
#
# To use: INCLUDE(DateFormat)
#
#===================================================================
# Macros:
# DATE_FORMAT(date_var format [locale])
#     date_var: Result date string
#     format: date format for date(1)
#     locale: locale of the string. 
#             Use current locale setting if locale is not given.

#  Note: CMake 2.4 does not support STRING(STRIP ..)

MACRO(DATE_FORMAT date_var format)
    SET(_locale ${ARGV2})
    IF(_locale)
	SET(ENV{LC_ALL} ${_locale})
    ENDIF(_locale)
    EXECUTE_PROCESS(
	COMMAND date "${format}"
	COMMAND tr \\n \\t
	COMMAND sed  -e s/\\t//
	OUTPUT_VARIABLE ${date_var}
    )
ENDMACRO(DATE_FORMAT date_var format locale)


