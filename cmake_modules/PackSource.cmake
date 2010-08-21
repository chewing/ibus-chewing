# - Pack source helper module
# By default CPack pack everything under the source directory, this is usually
# undesirable. We avoid this by using the sane default ignore list.
#
# Defines following macro:
#   PACK_SOURCE(packedSourceFile
#     [IGNORE excludeFilePatternList]
#   )
#   - Pack the source file as packedSourceFile
#
# Source Tarball handling. Required by RPM.cmake
# To use: INCLUDE(SourceTarball)
#
# Includes: Version
#
#===================================================================
# Variables:
#   Predefined:
#     CMAKE_GENERATED_FILES: Pattern of CMake generated files.
#          They are excluded from packing.
#     COMMON_IGNORED_FILES: Pattern of backup files and version control system
#          files such as CVS, SVN and git.
#          They are excluded from packing.
#   Output:
#     SOURCE_TARBALL_ORIG_PREFIX: The prefix of CPack packed file.
#     SOURCE_TARBALL_ORIG_POSTFIX: The postfix of CPack packed file.
#     SOURCE_TARBALL_ORIG: The filename of CPack packed file.
#     SOURCE_TARBALL_OUTPUT: output filename of target pack_src, including
#     output_dir.
#
#   Optional:
#     SOURCE_TARBALL_OUTPUT_DIR: Set the output directory for package files.
#     SOURCE_TARBALL_OUTPUT_PREFIX: Set the prefix of package files.
#
#===================================================================
# Targets:
# pack_src: Make source tarball for rpm packaging.
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

	SET(PACK_SOURCE_FILE_NAME "${outputDir}/${PROJECT_NAME}-${PRJ_VER}-Source"
	SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${outputDir}/${PROJECT_NAME}-${PRJ_VER}-Source")
	MESSAGE("CPACK_SOURCE_PACKAGE_FILE_NAME=${CPACK_SOURCE_PACKAGE_FILE_NAME}")
	INCLUDE(CPack)
    ENDMACRO(PACK_SOURCE packedSourceFile)
ENDIF(NOT DEFINED _PACK_SOURCE_CMAKE_)

#SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES} ${CMAKE_GENERATED_FILES} ${COMMON_IGNORED_FILES})

#ADD_DEPENDENCIES(pack_src version_check)

