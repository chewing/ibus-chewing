# - Generate Doxyfile
# This module is for doxygen generation.
# Defines following macros:
#   USE_DOXYGEN(doxygen_in doc_dir)
#   - This macro generate documents according to doxygen template.
#     Arguments:
#     + doxygen_in: Doxygen template file.
#     + doc_dir: Document source directory to be copied from.
#     Reads following variable:
#     + PROJECT_DOC_DIR: Directory for document
#
#
IF(NOT DEFINED _USE_DOXYGEN_CMAKE_)
    SET(_USE_DOXYGEN_CMAKE_ "DEFINED")
    SET(PACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES} "/Doxyfile$")

    MACRO(USE_DOXYGEN doxygen_in doc_dir)
	IF(PROJECT_DOC_DIR STREQUAL "")
	    MESSAGE(FATAL_ERROR "PROJECT_DOC_DIR undefined. Use macro SET_USUAL_COMPILE_ENV() before USE_DOXYGEN")
	ENDIF(PROJECT_DOC_DIR STREQUAL "")
	FIND_PACKAGE(Doxygen)
	CONFIGURE_FILE(${doxygen_in} Doxyfile)

	ADD_CUSTOM_TARGET(doxygen
	    COMMAND "${DOXYGEN}" "Doxyfile"
	    )

	INSTALL(DIRECTORY ${doc_dir}
	    DESTINATION "${PROJECT_DOC_DIR}"
	    )
    ENDMACRO(USE_DOXYGEN doxygen_template)
ENDIF(NOT DEFINED _USE_DOXYGEN_CMAKE_)

