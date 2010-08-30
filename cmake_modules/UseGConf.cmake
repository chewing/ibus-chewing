# - GConf relative targets such as install/unstall schemas.
# This module finds gconftool-2 or gconftool for GConf manipulation.
#
# Reads following variables:
# GCONF_SCHEMAS_FILE: Schema file.
#         Default: "${PROJECT_NAME}.schemas"
#
# GCONF_SCHEMAS_INSTALLED_DIR: Direct of installed schemas files.
#         Default: "${SYSCONF_INSTALL_DIR}/gconf/schemas"
#
# GCONF_CONFIG_SOURCE: configuration source.
#         Default: "" (Use the system default)
#
# Defines following targets:
#   install_schemas: install schemas
#
#   uninstall_schemas: uninstall schemas
#

IF(NOT DEFINED _USE_GCONF_CMAKE_)
    FIND_PROGRAM(GCONFTOOL NAMES gconftool-2 gconftool)

    IF(${GCONFTOOL} STREQUAL "GCONFTOOL-NOTFOUND")
	MESSAGE(FATAL_ERROR "gconftool and gconftool-2 are not found, please install GConf or GConf2.")
    ENDIF()

    IF (NOT DEFINED GCONF_SCHEMAS_FILE)
	SET(GCONF_SCHEMAS_FILE  "${PROJECT_NAME}.schemas")
    ENDIF(NOT DEFINED GCONF_SCHEMAS_FILE)

    GET_FILENAME_COMPONENT(_gconf_schemas_basename ${GCONF_SCHEMAS_FILE} NAME)

    IF (NOT DEFINED GCONF_SCHEMAS_INSTALLED_DIR)
	SET(GCONF_SCHEMAS_INSTALLED_DIR  "${SYSCONF_INSTALL_DIR}/gconf/schemas")
    ENDIF(NOT DEFINED GCONF_SCHEMAS_INSTALLED_DIR)

    IF (NOT DEFINED GCONF_CONFIG_SOURCE)
	SET(GCONF_CONFIG_SOURCE "")
    ENDIF(NOT DEFINED GCONF_CONFIG_SOURCE)
    SET(ENV{GCONF_CONFIG_SOURCE} ${GCONF_CONFIG_SOURCE})


    ADD_CUSTOM_TARGET(uninstall_schemas
	COMMAND GCONF_CONFIG_SOURCE=${GCONF_CONFIG_SOURCE}
	${GCONFTOOL} --makefile-uninstall-rule
	${GCONF_SCHEMAS_INSTALLED_DIR}/${_gconf_schemas_basename}
	COMMENT "Uninstalling schemas"
	)

    ADD_CUSTOM_TARGET(install_schemas
	COMMAND cmake -E copy ${GCONF_SCHEMAS_FILE} ${GCONF_SCHEMAS_INSTALLED_DIR}/${_gconf_schemas_basename}
	COMMAND GCONF_CONFIG_SOURCE=${GCONF_CONFIG_SOURCE}
	${GCONFTOOL} --makefile-install-rule
	${GCONF_SCHEMAS_INSTALLED_DIR}/${_gconf_schemas_basename}
	DEPENDS ${GCONF_SCHEMAS_FILE}
	COMMENT "Installing schemas"
	)

    INSTALL(FILES ${GCONF_SCHEMAS_FILE}
	DESTINATION "${GCONF_SCHEMAS_INSTALLED_DIR}")
ENDIF(NOT DEFINED _USE_GCONF_CMAKE_)


