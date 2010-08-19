# - Pack source helper module
# By default CPack pack everything under the source directory, this is usually
# undesirable. We avoid this by using the sane default ignore list.
#
# Defines following macro:
#   PACK_SOURCE(packedSourceFile
#     [OUTPUT_DIR outdir]
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
    SET(PACK_SOURCE_IGNORED_FILES_COMMON
	"/\\\\.svn/"  "/CVS/" "/\\\\.git/"  "\\\\.gitignore$" "/\\\\.hg/"
	"/\\\\.hgignore$"
	"~$" "\\\\.swp$" "\\\\.log$" "\\\\.bak$" "\\\\.old$"
	"\\\\.gmo$"
	"\\\\.tar.gz$" "\\\\.tar.bz2$" "/src/config\\\\.h$" "NO_PACK")

    SET(PACK_SOURCE_IGNORED_FILES_CMAKE "/CMakeFiles/" "_CPack_Packages/" "/Testing/"
	"\\\\.directory$" "CMakeCache\\\\.txt$"
	"/install_manifest.txt$"
	"/cmake_.*install\\\\.cmake$" "/CPack.*\\\\.cmake$" "/CTestTestfile\\\\.cmake$"
	"Makefile$"
	)

    SET(PACK_SOURCE_IGNORED_FILES ${PACK_SOURCE_IGNORED_FILES_COMMON} ${PACK_SOURCE_IGNORED_FILES_CMAKE})

    INCLUDE(ManageVersion)
    MACRO(PACK_SOURCE packedSourceFile)
	SET(_stage PACKED_SOURCE)
	SET(_output_dir)
	SET(_ignore)
	FOREACH(_arg_0 ${ARGN})
	    IF(_arg_0 STREQUAL "PACKED_SOURCE")
		SET(_stage "PACKED_SOURCE")
	    ELSEIF(_arg_0 STREQUAL "OUTPUT_DIR")
		SET(_stage "OUTPUT_DIR")
	    ELSEIF(_arg_0 STREQUAL "IGNORE")
		SET(_stage "IGNORE")
	    ELSE(_arg_0 STREQUAL "PACKED_SOURCE")
		IF(_stage STREQUAL "PACKED_SOURCE")
		    SET(packedSourceFile ${_arg_0})
		ELSEIF(_stage STREQUAL "OUTPUT_DIR")
		    SET(_output_dir ${_arg_0})
		ELSEIF(_stage STREQUAL "IGNORE")
		    SET(_ignore ${_ignore} ${_arg_0})
		ELSE(_stage STREQUAL "PACKED_SOURCE")
		ENDIF(_stage STREQUAL "PACKED_SOURCE")
	    ENDIF(_arg_0 STREQUAL "PACKED_SOURCE")
	ENDFOREACH(_arg_0)

	# Determine the generator via the extension.
	GET_FILENAME_COMPONENT(_fileExt packedSourceFile EXT)
	IF(_fileExt STREQUAL "gz")
	    SET(CPACK_GENERATOR "TGZ")
	ELSEIF(_fileExt STREQUAL "tgz")
	    SET(CPACK_GENERATOR "TGZ")
	ELSEIF(_fileExt STREQUAL "bz2")
	    SET(CPACK_GENERATOR "TBZ2")
	ELSEIF(_fileExt STREQUAL "tbz2")
	    SET(CPACK_GENERATOR "TBZ2")
	ELSEIF(_fileExt STREQUAL "Z")
	    SET(CPACK_GENERATOR "TZ")
	ELSEIF(_fileExt STREQUAL "zip")
	    SET(CPACK_GENERATOR "ZIP")
	ENDIF(_fileExt)
	SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES} ${CMAKE_GENERATED_FILES} ${COMMON_IGNORED_FILES})

    ENDMACRO(PACK_SOURCE packedSourceFile)

ENDIF(NOT DEFINED _PACK_SOURCE_CMAKE_)
IF(NOT DEFINED CPACK_GENERATOR)
    SET(CPACK_GENERATOR "TGZ")
ENDIF(NOT DEFINED CPACK_GENERATOR)

IF(NOT DEFINED CPACK_SOURCE_GENERATOR)
    SET(CPACK_SOURCE_GENERATOR ${CPACK_GENERATOR})
ENDIF(NOT DEFINED CPACK_SOURCE_GENERATOR)

IF(CPACK_SOURCE_GENERATOR STREQUAL "TGZ")
    SET(SOURCE_TARBALL_POSTFIX "tar.gz")
ELSEIF(CPACK_SOURCE_GENERATOR STREQUAL "TBZ2")
    SET(SOURCE_TARBALL_POSTFIX "tar.bz2")
ELSEIF(CPACK_SOURCE_GENERATOR STREQUAL "TZ")
    SET(SOURCE_TARBALL_POSTFIX "tar.Z")
ELSEIF(CPACK_SOURCE_GENERATOR STREQUAL "ZIP")
    SET(SOURCE_TARBALL_POSTFIX "zip")
ENDIF(CPACK_SOURCE_GENERATOR STREQUAL "TGZ")
SET(SOURCE_TARBALL_ORIG_PREFIX ${PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-Source)
SET(SOURCE_TARBALL_ORIG ${SOURCE_TARBALL_ORIG_PREFIX}.${SOURCE_TARBALL_POSTFIX})

SET(SOURCE_TARBALL_OUTPUT)

IF(DEFINED SOURCE_TARBALL_OUTPUT_DIR)
    IF(DEFINED SOURCE_TARBALL_OUTPUT_PREFIX)
	SET(SOURCE_TARBALL_OUTPUT
	    ${SOURCE_TARBALL_OUTPUT_DIR}/${SOURCE_TARBALL_OUTPUT_PREFIX}.${SOURCE_TARBALL_POSTFIX})
    ELSE(DEFINED SOURCE_TARBALL_OUTPUT_PREFIX)
	SET(SOURCE_TARBALL_OUTPUT
	    ${SOURCE_TARBALL_OUTPUT_DIR}/${SOURCE_TARBALL_ORIG})
    ENDIF(DEFINED SOURCE_TARBALL_OUTPUT_PREFIX)
ELSE(DEFINED SOURCE_TARBALL_OUTPUT_DIR)
    IF(DEFINED SOURCE_TARBALL_OUTPUT_PREFIX)
	SET(SOURCE_TARBALL_OUTPUT
	    ${SOURCE_TARBALL_OUTPUT_PREFIX}.${SOURCE_TARBALL_POSTFIX})
    ELSE(DEFINED SOURCE_TARBALL_OUTPUT_PREFIX)
	SET(SOURCE_TARBALL_OUTPUT
	    ${SOURCE_TARBALL_ORIG})
    ENDIF(DEFINED SOURCE_TARBALL_OUTPUT_PREFIX)
ENDIF(DEFINED SOURCE_TARBALL_OUTPUT_DIR)


IF(DEFINED SOURCE_TARBALL_OUTPUT_DIR)
    ADD_CUSTOM_TARGET(pack_src
	COMMAND make package_source
	COMMENT "Packaging Source files"
	COMMAND mkdir -p ${SOURCE_TARBALL_OUTPUT_DIR}
	COMMAND mv ${SOURCE_TARBALL_ORIG} ${SOURCE_TARBALL_OUTPUT}
	VERBATIM
	)
ELSE(DEFINED SOURCE_TARBALL_OUTPUT_DIR)
    IF(DEFINED SOURCE_TARBALL_OUTPUT_PREFIX)
	ADD_CUSTOM_TARGET(pack_src
	    COMMAND make package_source
	    COMMENT "Packaging Source files"
	    COMMAND mv ${SOURCE_TARBALL_ORIG} ${SOURCE_TARBALL_OUTPUT}
	    VERBATIM
	    )
    ELSE(DEFINED SOURCE_TARBALL_OUTPUT_PREFIX)
	ADD_CUSTOM_TARGET(pack_src
	    COMMAND make package_source
	    COMMENT "Packaging Source files"
	    VERBATIM
	    )
    ENDIF(DEFINED SOURCE_TARBALL_OUTPUT_PREFIX)
ENDIF(DEFINED SOURCE_TARBALL_OUTPUT_DIR)

SET(CMAKE_GENERATED_FILES "/CMakeFiles/" "_CPack_Packages/" "/Testing/"
    "\\\\.directory$" "CMakeCache\\\\.txt$"
    "/install_manifest.txt$"
    "/cmake_.*install\\\\.cmake$" "/CPack.*\\\\.cmake$" "/CTestTestfile\\\\.cmake$"
    "Makefile$"
    )

SET(COMMON_IGNORED_FILES
    "/\\\\.svn/"  "/CVS/" "/\\\\.git/"  "\\\\.gitignore$"
    "~$" "\\\\.swp$" "\\\\.log$" "\\\\.bak$" "\\\\.old$"
    "\\\\.gmo$"
    "\\\\.tar.gz$" "\\\\.tar.bz2$" "/src/config\\\\.h$" "NO_PACK")

SET(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES} ${CMAKE_GENERATED_FILES} ${COMMON_IGNORED_FILES})

ADD_DEPENDENCIES(pack_src version_check)

