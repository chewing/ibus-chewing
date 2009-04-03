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

INCLUDE(Version)

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
#MESSAGE("SOURCE_TARBALL_ORIG=${SOURCE_TARBALL_ORIG}")
#MESSAGE("SOURCE_TARBALL_OUTPUT=${SOURCE_TARBALL_OUTPUT}")

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

