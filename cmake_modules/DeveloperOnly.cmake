# Generic developers helper target such as commit and upload.
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
#  FEDPKG_DIR=<directory for Fedora git clone>
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

INCLUDE(ManageVariable)
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
# next_version:
#   Starting a new version.
#   This updates ChangeLog.prev and RPM-ChangeLog.prev
#
ADD_CUSTOM_TARGET(next_version
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/ChangeLog ${CMAKE_SOURCE_DIR}/ChangeLog.prev
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/SPECS/RPM-ChangeLog
    ${CMAKE_SOURCE_DIR}/SPECS/RPM-ChangeLog.prev
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/SPECS/RPM-RELEASE-NOTES.template
    ${CMAKE_SOURCE_DIR}/SPECS/RPM-RELEASE-NOTES.txt
    )

IF (HOSTING_SERVICE STREQUAL git)
    ADD_CUSTOM_TARGET(push
	COMMAND git commit -a -m "${CHANGE_SUMMARY}"
	COMMAND git tag -a "${PRJ_VER}" -m "Ver ${PRJ_VER}" HEAD
	COMMAND git push
	COMMENT "Commit and tag the changes"
	VERBATIM
	)
ENDIF(HOSTING_SERVICE STREQUAL git)


