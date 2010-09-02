# - Generate Doxyfile
# This module is for doxygen generation.
# Defines following macros:
#   USE_DOXYGEN(doxygen_in doc_dir)
#   - This macro generate documents according to doxygen template.
#     Arguments:
#     + doxygen_in: Doxygen template file.
#     + doc_dir: Document source directory to be copied from.
#
#
IF(NOT DEFINED _USE_DOXYGEN_CMAKE_)
    SET(_USE_DOXYGEN_CMAKE_ "DEFINED")

    MACRO(USE_DOXYGEN doxygen_in doc_dir)
	FIND_PACKAGE(Doxygen)
	CONFIGURE_FILE(${doxygen_in} Doxyfile)
	SET(PACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES} "/Doxyfile$")
	ADD_CUSTOM_TARGET(doxygen
	    COMMAND "${DOXYGEN}" "Doxyfile"
	    )

	INSTALL(DIRECTORY ${doc_dir}
	    DESTINATION "${docdir}/${PROJECT_NAME}-${PRJ_VER}"
	    )
    ENDMACRO(USE_DOXYGEN doxygen_template)
ENDIF(NOT DEFINED _USE_DOXYGEN_CMAKE_)

