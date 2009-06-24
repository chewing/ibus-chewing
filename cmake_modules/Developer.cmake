# Developers helper target such as commit and upload.
#
# To use: INCLUDE(Developer)
#
# To hide the developer only targets,
# only include this modules when  DEVELOPER_SETTING_FILE defined and file exists.
#
# Example of a developer setting file:
#  SSH_USER=<user_name>
#  SSH_ARGS=<additional args>
#  KOJI_CVS_PATH=<path for koji CVS>
#
#
#===================================================================
# Variables:
# DEVELOPER_SETTING_FILE: the file that contain developer information.
#                         Required.
# DEVELOPER_DEPENDS: Files that the developer targets depends on.
# HOSTING_SERVICE_PROVIDER: Name of hosting service provider for display.
#===================================================================
# Macro:
# DEVELOPER_UPLOAD([SERVER_TYPE] server_type
#     [USER user]
#     [BATCH batchfile]
#     [SITE hosting_site]
#     [OPTIONS upload_options]
#     [COMMAND upload_command]
#  )
#  Generate an upload_pkg_release target for uploading releases to hosting
#  service.
#
#  Parameters:
#     server_type: Server type of hosting service. Current "sftp" and "custom"
#         are supported.
#         Note that type "sftp" ignores "COMMAND" option, while type
#         "custom" only recognizes "COMMAND", but other type recognize
#         everything else.
#     user: User name used to upload.
#     batchfile: File that contain batch command to upload.
#     hosting_site: Domain name of hosting service.
#     upload_options: Parameters of uploading command. No use if type is
#         "custom"
#     upload_command: Custom upload command.
# * Produced targets: upload_pkg_release
#

INCLUDE(BasicMacros)
IF(NOT DEFINED HOSTING_SERVICE_PROVIDER)
    SET(HOSTING_SERVICE_PROVIDER "hosting service provider")
ENDIF(NOT DEFINED HOSTING_SERVICE_PROVIDER)

MACRO(DEVELOPER_UPLOAD arg_0 arg_list)
    SET(_stage SERVER_TYPE)
    SET(_type)
    SET(_user)
    SET(_batch)
    SET(_site)
    SET(_options)
    SET(_command)
    FOREACH(_arg_0 ${arg_list} ${ARGN})
	IF(_arg_0 STREQUAL "SERVER_TYPE")
	    SET(_stage "SERVER_TYPE")
	ELSEIF(_arg_0 STREQUAL "USER")
	    SET(_stage "USER")
	ELSEIF(_arg_0 STREQUAL "BATCH")
	    SET(_stage "BATCH")
	ELSEIF(_arg_0 STREQUAL "SITE")
	    SET(_stage "SITE")
	ELSEIF(_arg_0 STREQUAL "OPTIONS")
	    SET(_stage "OPTIONS")
	ELSEIF(_arg_0 STREQUAL "COMMAND")
	    SET(_stage "COMMAND")
	ELSE(_arg_0 STREQUAL "SERVER_TYPE")
	    IF(_stage STREQUAL "SERVER_TYPE")
		SET(_type ${_arg_0})
	    ELSEIF(_stage STREQUAL "USER")
		SET(_user ${_arg_0})
	    ELSEIF(_stage STREQUAL "BATCH")
		SET(_batch ${_arg_0})
	    ELSEIF(_stage STREQUAL "SITE")
		SET(_site ${_arg_0})
	    ELSEIF(_stage STREQUAL "OPTIONS")
		SET(_options ${_options} ${_arg_0})
	    ELSEIF(_stage STREQUAL "COMMAND")
		SET(_command ${_arg_0})
	    ELSE(_stage STREQUAL "SERVER_TYPE")
	    ENDIF(_stage STREQUAL "SERVER_TYPE")
	ENDIF(_arg_0 STREQUAL "SERVER_TYPE")
    ENDFOREACH(_arg_0)

    SET(_developer_upload_cmd)
    IF(_type STREQUAL "sftp")
	SET(_developer_upload_cmd "sftp")
	IF(_batch)
	    SET(_developer_upload_cmd "${_developer_upload_cmd} -b ${_batch}" )
	ENDIF(_batch)
	IF(_options)
	    SET(_developer_upload_cmd "${_developer_upload_cmd} -F ${_options}" )
	ENDIF(_options)
	IF(_user)
	    SET(_developer_upload_cmd "${_developer_upload_cmd}  ${_user}@" )
	ELSE(_user)
	    SET(_developer_upload_cmd "${_developer_upload_cmd}  " )
	ENDIF(_user)
	IF(_site)
	    SET(_developer_upload_cmd "${_developer_upload_cmd}${_site}" )
	ENDIF(_site)
    ELSE(_type STREQUAL "sftp")
	SET(_developer_upload_cmd ${_command} )
    ENDIF(_type STREQUAL "sftp")

    #MESSAGE("_developer_upload_cmd=${_developer_upload_cmd}")
    IF(EXISTS ${DEVELOPER_SETTING_FILE})
	ADD_CUSTOM_TARGET(upload_pkg_release
	    COMMAND ${DEVELOPER_UPLOAD_CMD}
	    DEPENDS ${DEVELOPER_DEPENDS}
	    COMMENT "Uploading the package releases to ${HOSTING_SERVICE_PROVIDER}..."
	    VERBATIM
	    )
    ENDIF(EXISTS ${DEVELOPER_SETTING_FILE})
ENDMACRO(DEVELOPER_UPLOAD arg_0 arg_list)


#===================================================================
# Targets:
# version_lock:
#   Lock in the current version, so the version won't be changed until unlocked.
#   Useful for time-based version.
#   Note that the PRJ_VER is locked but PRJ_VER_FULL is not, thus RPM
#   maintainer can do some maintenances without affecting source version.
#
# version_unlock:
#   Unlock the version.
#
# To enable these two targets, need to:
# SET(VERSION_NEED_LOCK)
#

#IF (DEFINED VERSION_NEED_LOCK)
#    ADD_CUSTOM_TARGET(version_lock
#	COMMAND grep "PRJ_VER=" ${RELEASE_FILE}
#	|| ${CMAKE_COMMAND} -E echo "PRJ_VER=${PRJ_VER}" > ${RELEASE_FILE}
#	COMMAND cmake ${CMAKE_SOURCE_DIR}
#	COMMENT "Lock version"
#	)
#
#    ADD_CUSTOM_TARGET(version_unlock
#	COMMAND ${RM} ${RELEASE_FILE}
#	COMMAND ${CMAKE_COMMAND} -E touch  ${RELEASE_FILE}
#	COMMAND ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}
#	COMMENT "Unlock version"
#	)
#ENDIF(DEFINED VERSION_NEED_LOCK)

