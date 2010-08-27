# - Pack source helper module
# By default CPack pack everything under the source directory, this is usually
# undesirable. We avoid this by using the sane default ignore list.
#
# Defines following variable:
#   PACK_SOURCE_IGNORE_FILES_DEFAULT: Default list of file that should not be packed.
# Defines following macro:
#   PACK_SOURCE(var outputDir [generator])
#   - Pack source files as <projectName>-<PRJ_VER>-Source.<packFormat>,
#     Arguments:
#     + var: The filename of source package is outputted to this var.
#            Path is excluded.
#     + outputDir: Directory to write source tarball.
#     + generator: (Optional) Method to make tarball. Basically this argument
#       is passed as CPACK_GENERATOR. Default to TGZ.
#     Variable to be read:
#     + PROJECT_NAME: Project name
#     + PRJ_VER: Project version
#     + PACK_SOURCE_IGNORE_FILES: A list of regex filename pattern to indicate
#       the files to be excluded.
#     Target:
#     + pack_src: Pack source files like package_source, but also check
#       version.
# Defines following target:
#     pack_remove_old: Remove old source package files.
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

    SET(PACK_SOURCE_IGNORE_FILES_DEFAULT ${PACK_SOURCE_IGNORE_FILES_COMMON} ${PACK_SOURCE_IGNORE_FILES_CMAKE})

    INCLUDE(ManageVersion)
    MACRO(GET_RELATIVE_PATH var pwd target)
	GET_FILENAME_COMPONENT(_pwdAbs ${pwd} ABSOLUTE)
	GET_FILENAME_COMPONENT(_targetAbs ${target} ABSOLUTE)
	SET(_res)
	SET(_dotdot)
	WHILE("${_res}" STREQUAL "")
	    #MESSAGE("# _pwdAbs=${_pwdAbs} _dotdot=${_dotdot}")
	    STRING(REGEX MATCH "^${_pwdAbs}" _res "${_targetAbs}")
	    IF("${_res}" STREQUAL "")
		IF(_pwdAbs STREQUAL "/")
		    MESSAGE(FATAL_ERROR "Reach the root '/' but still not match, something wrong here!")
		ENDIF(_pwdAbs STREQUAL "/")
		GET_FILENAME_COMPONENT(_pwdAbs ${_pwdAbs} PATH)
		SET(_dotdot "../${_dotdot}")
	    ENDIF("${_res}" STREQUAL "")
	ENDWHILE("${_res}" STREQUAL "")
	#MESSAGE("# _res=${_res} _pwdAbs=${_pwdAbs} _dotdot=${_dotdot}")
	IF("${_res}" STREQUAL "${_targetAbs}")
	    IF("${_dotdot}" STREQUAL "")
		SET(${var} ".")
	    ELSE("${_dotdot}" STREQUAL "")
		STRING(REPLACE "${_res}" "${_dotdot}" ${var} "${_targetAbs}")
	    ENDIF("${_dotdot}" STREQUAL "")
	ELSE("${_res}" STREQUAL "${_targetAbs}")
	    # Replace the trailing "/" to avoid the path start with "/"
	    STRING(REPLACE "${_res}/" "${_dotdot}" ${var} "${_targetAbs}")
	ENDIF("${_res}" STREQUAL "${_targetAbs}")
    ENDMACRO(GET_RELATIVE_PATH var pwd target)

    MACRO(PACK_SOURCE var outputDir)
	IF(NOT DEFINED PACK_SOURCE_IGNORE_FILES)
	    SET(PACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES_DEFAULT})
	ENDIF(NOT DEFINED PACK_SOURCE_IGNORE_FILES)

	IF(${PRJ_VER} STREQUAL "")
	    MESSAGE(FATAL_ERROR "PRJ_VER not defined")
	ENDIF(${PRJ_VER} STREQUAL "")
	IF(${ARGV3})
	    SET(CPACK_GENERATOR "${ARGV3}")
	ELSE(${ARGV3})
	    SET(CPACK_GENERATOR "TGZ")
	ENDIF(${ARGV3})
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

	# Get relative path of outputDir
	GET_RELATIVE_PATH(_outputDir_rel ${CMAKE_BINARY_DIR} ${outputDir})
	#MESSAGE("#_outputDir_rel=${_outputDir_rel}")

	IF(EXISTS ${CMAKE_SOURCE_DIR}/COPYING)
	    SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/README)
	ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/COPYING)

	IF(EXISTS ${CMAKE_SOURCE_DIR}/README)
	    SET(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README)
	ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/README)

	IF(DEFINED PROJECT_DESCRIPTION)
	    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_DESCRIPTION}")
	ENDIF(DEFINED PROJECT_DESCRIPTION)

	SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${_outputDir_rel}/${PROJECT_NAME}-${PRJ_VER}-Source")
	SET(_path_source_path "${CPACK_SOURCE_PACKAGE_FILE_NAME}.${_pack_source_ext}")
	SET(${var} "${PROJECT_NAME}-${PRJ_VER}-Source.${_pack_source_ext}")
	#MESSAGE("_pach_source_path=${CPACK_SOURCE_PACKAGE_FILE_NAME}.${_pack_source_ext}")

	INCLUDE(CPack)

	ADD_CUSTOM_COMMAND(OUTPUT ${_path_source_path}
	   COMMAND make pack_src
	   COMMENT "Packing the source"
	    )

	ADD_CUSTOM_TARGET(pack_src
	    COMMAND make package_source
	    DEPENDS ChangeLog
	    )

	ADD_DEPENDENCIES(pack_src version_check)
    ENDMACRO(PACK_SOURCE var outputDir)

    ADD_CUSTOM_TARGET(pack_remove_old
	COMMAND find .
	-name '${PROJECT_NAME}*.tar.[bg]z*' ! -name '${PROJECT_NAME}-${PRJ_VER}-*.${SOURCE_TARBALL_POSTFIX}'
	-print -delete
	COMMENT "Removing the old tarballs .."
	)

    ADD_DEPENDENCIES(pack_remove_old version_check)
ENDIF(NOT DEFINED _PACK_SOURCE_CMAKE_)

