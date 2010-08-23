# - Pack source helper module
# By default CPack pack everything under the source directory, this is usually
# undesirable. We avoid this by using the sane default ignore list.
#
# Defines following macro:
#   PACK_SOURCE(outputDir [generator])
#   - Pack source files as <projectName>-<PRJ_VER>-Source.<packFormat>,
#     Arguments:
#     + outputDir: Directory to write source tarball.
#     + generator: (Optional) Method to make tarball. Basically this argument
#       is passed as CPACK_GENERATOR. Default to TGZ.
#     Variable to be read:
#     + PROJECT_NAME: Project name
#     + PRJ_VER: Project version
#     + PACK_SOURCE_IGNORE_FILES: A list of regex filename pattern to indicate
#       the files to be exclude.
#     Output Variable:
#     + PACK_SOURCE_FILE_NAME: The final output file, including the output
#       directory.
#     Target:
#     + pack_src: Pack source files like package_source, but also check
#       version.
# Defines following target:
#     pack_remove_old: Remove old source tarballs.
#
#
IF(NOT DEFINED _PACK_SOURCE_CMAKE_)
    SET (_PACK_SOURCE_CMAKE_ "DEFINED")
    SET(PACK_SOURCE_IGNORE_FILES_COMMON
	"/\\\\.svn/"  "/CVS/" "/\\\\.git/"  "\\\\.gitignore$" "/\\\\.hg/"
	"/\\\\.hgignore$"
	"~$" "\\\\.swp$" "\\\\.log$" "\\\\.bak$" "\\\\.old$"
	"\\\\.gmo$" "\\\\.cache$"
	"\\\\.tar.gz$" "\\\\.tar.bz2$" "/src/config\\\\.h$" "NO_PACK")

    SET(PACK_SOURCE_IGNORE_FILES_CMAKE "/CMakeFiles/" "_CPack_Packages/" "/Testing/"
	"\\\\.directory$" "CMakeCache\\\\.txt$"
	"/install_manifest.txt$"
	"/cmake_.*install\\\\.cmake$" "/CPack.*\\\\.cmake$" "/CTestTestfile\\\\.cmake$"
	"Makefile$"
	)

    IF(NOT DEFINED PACK_SOURCE_IGNORE_FILES)
	SET(PACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES_COMMON} ${PACK_SOURCE_IGNORE_FILES_CMAKE})
    ENDIF(NOT DEFINED PACK_SOURCE_IGNORE_FILES)

    INCLUDE(ManageVersion)
    MACRO(PACK_SOURCE outputDir)
	IF(${PRJ_VER} STREQUAL "")
	    MESSAGE(FATAL_ERROR "PRJ_VER not defined")
	ENDIF(${PRJ_VER} STREQUAL "")
	IF(${ARGV2})
	    SET(CPACK_GENERATOR "${ARGV2}")
	ELSE(${ARGV2})
	    SET(CPACK_GENERATOR "TGZ")
	ENDIF(${ARGV2})
	SET(CPACK_SOURCE_GENERATOR ${CPACK_GENERATOR})
	IF(${CPACK_GENERATOR} STREQUAL "TGZ")
	    SET(_pack_source_ext "tar.gz")
	ELSEIF(${CPACK_GENERATOR} STREQUAL "TBZ2")
	    SET(_pack_source_ext "tar.bz2")
	ELSEIF(${CPACK_GENERATOR} STREQUAL "ZIP")
	    SET(_pack_source_ext "zip")
	ENDIF(${CPACK_GENERATOR} STREQUAL "TGZ")

	SET(CPACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES})
	SET(CPACK_PACKAGE_VERSION ${PRJ_VER})

	IF(EXISTS ${CMAKE_SOURCE_DIR}/COPYING)
	    SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/README)
	ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/COPYING)

	IF(EXISTS ${CMAKE_SOURCE_DIR}/README)
	    SET(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README)
	ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/README)

	IF(DEFINED PROJECT_DESCRIPTION)
	    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
	ENDIF(DEFINED PROJECT_DESCRIPTION)

	SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${outputDir}/${PROJECT_NAME}-${PRJ_VER}-Source")
	SET(PACK_SOURCE_FILE_NAME
	    "${CPACK_SOURCE_PACKAGE_FILE_NAME}.${_pack_source_ext}")
	MESSAGE("PACK_SOURCE_FILE_NAME=${PACK_SOURCE_FILE_NAME}")
	INCLUDE(CPack)
	UNSET(_pack_source_ext)

	ADD_CUSTOM_COMMAND(OUTPUT ${PACK_SOURCE_FILE_NAME}
	    COMMAND make pack_src
	    COMMENT "Packing the source"
	    )

	ADD_CUSTOM_TARGET(pack_src
	    COMMAND make package_source
	    )

	ADD_DEPENDENCIES(pack_src version_check)
    ENDMACRO(PACK_SOURCE packedSourceFile)

    ADD_CUSTOM_TARGET(pack_remove_old
	COMMAND find .
	-name '${PROJECT_NAME}*.tar.[bg]z*' ! -name '${PROJECT_NAME}-${PRJ_VER}-*.${SOURCE_TARBALL_POSTFIX}'
	-print -delete
	COMMENT "Removing the old tarballs .."
	)

    ADD_DEPENDENCIES(pack_remove_old version_check)
ENDIF(NOT DEFINED _PACK_SOURCE_CMAKE_)

