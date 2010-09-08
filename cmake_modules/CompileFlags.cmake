# - Set flags for compile environment.
# This module sets flags for compile environment.
#
# Defines following macros:
#   SET_COMPILE_ENV(var default_value [env])
#     - Add compiler environment with a variable and its value.
#       If the variable is not defined yet, then a default value is assigned to it first.
#       Parameters:
#       + var: Variable to be set
#       + default_value: Default value of the var
#       + env: (Optional)The name of environment variable. Only need if different from var.
#
#  SET_USUAL_COMPILE_ENV()
#  - Set the most often used variable and compile flags.
#    It defines compile flags according to the values of corresponding variables,
#    usually under the same or similar name.
#    If a corresponding variable is not defined yet, then a default value is assigned
#    to that variable, then define the flag.
#
#    Defines following flags according to the variable with same name.
#    + CMAKE_INSTALL_PREFIX: Compile flag whose value is ${CMAKE_INSTALL_PREFIX}.
#    + DATA_DIR: Directory for architecture independent data files.
#      Default: ${CMAKE_INSTALL_PREFIX}/share
#    + DOC_DIR: Directory for documentation
#      Default: ${DATA_DIR}/doc
#    + SYSCONF_DIR: System wide configuration files.
#      Default: /etc
#    + LIB_DIR: System wide library path.
#      Default: ${CMAKE_INSTALL_PREFIX}/lib for 32 bit,
#               ${CMAKE_INSTALL_PREFIX}/lib64 for 64 bit.
#    + LIBEXEC_DIR: Executables that are not meant to be executed by user directly.
#      Default: ${CMAKE_INSTALL_PREFIX}/libexec
#    + PROJECT_NAME: Project name
#    + PRJ_VER: Project version
#    + PROJECT_DATA_DIR: Data directory for the project.
#      Default: ${DATA_DIR}/${PROJECT_NAME}
#    + PROJECT_DOC_DIR: Documentation for the project.
#      Default: ${DOC_DIR}/${PROJECT_NAME}-${PRJ_VER}
#


IF(NOT DEFINED _COMPILE_FLAGS_CMAKE_)
    SET(_COMPILE_FLAGS_CMAKE_ "DEFINED")

    MACRO(SET_COMPILE_ENV var default_value)
	IF(${ARGC} GREATER 2)
	    SET(_env ${ARGV2})
	ELSE(${ARGC} GREATER 2)
	    SET(_env ${var})
	ENDIF(${ARGC} GREATER 2)
	IF(NOT DEFINED ${var})
	    SET(${var} "${default_value}")
	ENDIF(NOT DEFINED ${var})
	ADD_DEFINITIONS(-D${_env}='"${${var}}"')
    ENDMACRO(SET_COMPILE_ENV var default_value)

    MACRO(SET_USUAL_COMPILE_ENV)
	ADD_DEFINITIONS(-DCMAKE_INSTALL_PREFIX='"${CMAKE_INSTALL_PREFIX}"')

	SET_COMPILE_ENV(DATA_DIR "${CMAKE_INSTALL_PREFIX}/share")
	SET_COMPILE_ENV(DOC_DIR "${DATA_DIR}/doc")
	SET_COMPILE_ENV(SYSCONF_DIR "/etc" )
	SET_COMPILE_ENV(LIBEXEC_DIR "${CMAKE_INSTALL_PREFIX}/libexec")

	IF(NOT DEFINED LIB_DIR)
	    IF( $ENV{MACHTYPE} MATCHES "64")
		SET(LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib64")
	    ELSE($ENV{MACHTYPE} MATCHES "64")
		SET(LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
	    ENDIF($ENV{MACHTYPE} MATCHES "64")
	ENDIF()
	ADD_DEFINITIONS(-DIS_64='"${IS64}"')

	IF(DEFINED PROJECT_NAME)
	    ADD_DEFINITIONS(-DPROJECT_NAME='"${PROJECT_NAME}"')

	    SET_COMPILE_ENV(PROJECT_DATA_DIR "${DATA_DIR}/${PROJECT_NAME}")

	    IF(DEFINED PRJ_VER)
		ADD_DEFINITIONS(-DPRJ_VER='"${PRJ_VER}"')
		SET_COMPILE_ENV(PROJECT_DOC_DIR "${DOC_DIR}/${PROJECT_NAME}-${PRJ_VER}")
	    ENDIF(DEFINED PRJ_VER)
	ENDIF(DEFINED PROJECT_NAME)
    ENDMACRO(SET_USUAL_COMPILE_ENV)

ENDIF(NOT DEFINED _COMPILE_FLAGS_CMAKE_)


