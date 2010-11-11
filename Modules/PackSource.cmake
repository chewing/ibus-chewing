# - Pack source helper module
# By default CPack pack everything under the source directory, this is usually
# undesirable. We avoid this by using the sane default ignore list.
#
# Read and Defines following variable:
#   PACK_SOURCE_IGNORE_FILES_DEFAULT: Default list of file patterns
#     that are normally exclude from the source package.
#     Override it by setting it before INCLUDE(PackSource).
# Defines following target:
#     pack_remove_old: Remove old source package files.
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
#     + VENDOR: Organization that issue this project.
#     + PRJ_VER: Project version
#     + PRJ_SUMMARY: (Optional) Project summary
#     + PACK_SOURCE_IGNORE_FILES: A list of regex filename pattern to indicate
#       the files to be excluded. Note that cmake generated file
#       (PACK_SOURCE_IGNORE_FILE_CMAKE) is already in this list.
#     Define following variables:
#     + PACK_SOURCE_FILE_EXTENSION: File extension of the source package
#       files.
#     Target:
#     + pack_src: Pack source files like package_source.
#       This target depends on version_check.
#     + clean_pack_src: Remove all source packages.
#     + clean_old_pack_src: Remove all old source package.
#       This target depends on version_check.
#
#   PACK_SOURCE_FILES(var)
#   - Return all source file to be packed.
#     Arguments:
#     + var: A list of relative filenames that will be in the tarball.
#
IF(NOT DEFINED _PACK_SOURCE_CMAKE_)
    SET (_PACK_SOURCE_CMAKE_ "DEFINED")
    SET(PACK_SOURCE_IGNORE_FILES_DEFAULT
	"/\\\\.svn/"  "/CVS/" "/\\\\.git/"  "\\\\.gitignore$" "/\\\\.hg/"
	"/\\\\.hgignore$"
	"~$" "\\\\.swp$" "\\\\.log$" "\\\\.bak$" "\\\\.old$"
	"\\\\.gmo$" "\\\\.cache$"
	"\\\\.tar.gz$" "\\\\.tar.bz2$" "/src/config\\\\.h$" "NO_PACK")

    SET(PACK_SOURCE_IGNORE_FILES_CMAKE "/CMakeFiles/" "_CPack_Packages/" "/Testing/"
	"\\\\.directory$" "CMakeCache\\\\.txt$"
	"/install_manifest.txt$"
	"/cmake_install\\\\.cmake$" "/cmake_uninstall\\\\.cmake$""/CPack.*\\\\.cmake$" "/CTestTestfile\\\\.cmake$"
	"Makefile$"
	)

    SET(PACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES}
	${PACK_SOURCE_IGNORE_FILES_CMAKE} ${PACK_SOURCE_IGNORE_FILES_DEFAULT})

    INCLUDE(ManageVersion)

    MACRO(PACK_SOURCE_FILES var)
	SET(_filelist "")
	FILE(GLOB_RECURSE _ls "*")
	STRING(REPLACE "\\\\" "\\" _ignore_files "${PACK_SOURCE_IGNORE_FILES}")
	FOREACH(_file ${_ls})
	    SET(_matched 0)
	    FOREACH(filePattern ${_ignore_files})
		#MESSAGE("filePattern=${filePattern}")
		IF(NOT _matched)
		    IF(_file MATCHES "${filePattern}")
			SET(_matched 1)
		    ENDIF(_file MATCHES "${filePattern}")
		ENDIF(NOT _matched)
	    ENDFOREACH(filePattern ${_ignore_files})
	    IF(NOT _matched)
		FILE(RELATIVE_PATH _file ${CMAKE_SOURCE_DIR} "${_file}")
		#MESSAGE("_file=${_file}")
		LIST(APPEND _filelist "${_file}")
	    ENDIF(NOT _matched)
	ENDFOREACH(_file ${_ls})
	SET(${var} ${_filelist})
	#MESSAGE("PACK_SOURCE_IGNORE_FILES=${_ignore_files}")
    ENDMACRO(PACK_SOURCE_FILES var)

    MACRO(PACK_SOURCE var outputDir)
	#MESSAGE("PACK_SOURCE_IGNORE_FILES=${PACK_SOURCE_IGNORE_FILES}")

	IF(PRJ_VER STREQUAL "")
	    MESSAGE(FATAL_ERROR "PRJ_VER not defined")
	ENDIF(PRJ_VER STREQUAL "")
	IF(${ARGV3})
	    SET(CPACK_GENERATOR "${ARGV3}")
	ELSE(${ARGV3})
	    SET(CPACK_GENERATOR "TGZ")
	ENDIF(${ARGV3})
	SET(CPACK_SOURCE_GENERATOR ${CPACK_GENERATOR})
	IF(${CPACK_GENERATOR} STREQUAL "TGZ")
	    SET(PACK_SOURCE_FILE_EXTENSION "tar.gz")
	ELSEIF(${CPACK_GENERATOR} STREQUAL "TBZ2")
	    SET(PACK_SOURCE_FILE_EXTENSION "tar.bz2")
	ELSEIF(${CPACK_GENERATOR} STREQUAL "ZIP")
	    SET(PACK_SOURCE_FILE_EXTENSION "zip")
	ENDIF(${CPACK_GENERATOR} STREQUAL "TGZ")

	SET(CPACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES})
	SET(CPACK_PACKAGE_VERSION ${PRJ_VER})

	# Get relative path of outputDir
	FILE(RELATIVE_PATH _outputDir_rel ${CMAKE_BINARY_DIR} ${outputDir})
	#MESSAGE("#_outputDir_rel=${_outputDir_rel}")

	IF(EXISTS ${CMAKE_SOURCE_DIR}/COPYING)
	    SET(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/README)
	ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/COPYING)

	IF(EXISTS ${CMAKE_SOURCE_DIR}/README)
	    SET(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_SOURCE_DIR}/README)
	ENDIF(EXISTS ${CMAKE_SOURCE_DIR}/README)

	IF(DEFINED PRJ_SUMMARY)
	    SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PRJ_SUMMARY}")
	ENDIF(DEFINED PRJ_SUMMARY)

	SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PRJ_VER}-Source")
	SET(${var} "${CPACK_SOURCE_PACKAGE_FILE_NAME}.${PACK_SOURCE_FILE_EXTENSION}")

	SET(CPACK_PACKAGE_VENDOR "${VENDOR}")
	PACK_SOURCE_FILES(PACK_SOURCE_FILE_LIST)

	INCLUDE(CPack)
	ADD_CUSTOM_COMMAND(OUTPUT ${_outputDir_rel}
	    COMMAND cmake -E make_directory ${_outputDir_rel}
	    COMMENT "Making package output directory."
	    )

	IF("${_outputDir_rel}" STREQUAL ".")
	    ADD_CUSTOM_COMMAND(OUTPUT "${_outputDir_rel}/${${var}}"
		COMMAND make package_source
		DEPENDS ChangeLog ${RELEASE_FILE} ${PACK_SOURCE_FILE_LIST}
		COMMENT "Packing the source"
		)
	ELSE("${_outputDir_rel}" STREQUAL ".")
	    ADD_CUSTOM_COMMAND(OUTPUT "${_outputDir_rel}/${${var}}"
		COMMAND make package_source
		COMMAND cmake -E copy ${${var}} "${_outputDir_rel}/"
		COMMAND cmake -E remove ${${var}}
		DEPENDS ChangeLog ${RELEASE_FILE} ${PACK_SOURCE_FILE_LIST} ${_outputDir_rel}
		COMMENT "Packing the source"
		)
	ENDIF("${_outputDir_rel}" STREQUAL ".")

	ADD_CUSTOM_TARGET(pack_src
	    DEPENDS "${_outputDir_rel}/${${var}}"
	    )

	ADD_DEPENDENCIES(pack_src version_check)

	ADD_CUSTOM_TARGET(clean_old_pack_src
	    COMMAND find .
	    -name '${PROJECT_NAME}*.${PACK_SOURCE_FILE_EXTENSION}' ! -name '${PROJECT_NAME}-${PRJ_VER}-*.${PACK_SOURCE_FILE_EXTENSION}'
	    -print -delete
	    COMMENT "Cleaning old source packages"
	    )

	ADD_DEPENDENCIES(clean_old_pack_src version_check)

	ADD_CUSTOM_TARGET(clean_pack_src
	    COMMAND find .
	    -name '${PROJECT_NAME}*.${PACK_SOURCE_FILE_EXTENSION}'
	    -print -delete
	    COMMENT "Cleaning all source packages"
	    )
    ENDMACRO(PACK_SOURCE var outputDir)


ENDIF(NOT DEFINED _PACK_SOURCE_CMAKE_)

