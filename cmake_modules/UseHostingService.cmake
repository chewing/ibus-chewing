# - Read setting files and provides developer only targets.
# This module have macros that generate variables such as
# upload to a hosting services, which are valid to only the developers.
# This is normally done by checking the existence of a developer
# setting file.
#
# Defines following Macros:
#   USE_HOSTING_SERVICE_READ_SETTING_FILE(filename)
#   - It checks the existence of setting file.
#     If it does not exist, this macro acts as a no-op;
#     if it exists, then it reads variables defined in the setting file,
#     and set relevant targets.
#     See the "Setting File Format" section for description of file format.
#     Arguments:
#     + filename: Filename of the setting file.
#     Reads and defines following variables:
#     + PACK_SOURCE_IGNORE_FILES: It appends list of generated file to
#       ignore file list.
#     Reads following variables:
#     + PRJ_VER: Project version.
#     + CHANGE_SUMMARY: Change summary.
#     Defines following targets:
#     + changelog_update: Update changelog by copying ChangeLog to ChangeLog.prev
#       and RPM-ChangeLog to RPM-ChangeLog. This target should be execute before
#       starting a new version.
#     + tag: tag the latest commit with the ${PRJ_VER} and ${CHANGE_SUMMARY} as comment.
#     + upload: upload the source packages to hosting services.
#
# Setting File Format
#
# It is basically the "variable=value" format.
# For variables which accept list, use ';' to separate each element.
# A line start with '#' is deemed as comment.
#
# Recognized Variable:
# Although it does no harm to define other variables in the setting file,
# but this module only recognizes following variables:
#
#   HOSTING_SERVICES
# A list of hosting services that packages are hosted. It allows multiple elements.
#
#   SOURCE_VERSION_CONTROL
# Version control system for the source code. Accepted values: git, hg, svn, cvs.
#
# The services can be defined by following format:
#   <ServiceName>_<protocol>_<PropertyName>=<value>
#
# ServiceName is the name of the hosting service.
# If using a known service name, you may be able to omit some definition,
# as they have build in value. Do not worry that your hosting service is
# not in the known list, you can still benefit from this module, providing
# your hosting service use supported protocols.
#
# Known service name is: SOURCEFORGE.
#
# Protocol is the protocol used to upload file.
#  Supported protocol is: SFTP.
#
# PropertyName is a property that is needed to preform the upload.
# It is usually associate to the Protocol.
#
#  For protocol SFTP:
#    USER: the user name for sftp.
#    SITE: the host name of the sftp site.
#    BATCH: File that stores the batch commands.
#    BATCH_TEMPATE: File that provides template to for generating
#                   batch commands.
#                   If BATCH is also given: Generated batch file is named
#                   as defined with BATCH;
#                   if BATCH is not given: Generated batch file is named
#                   as ${CMAKE_BINARY_DIR}/BatchUpload-${ServiceName}
#   OPTIONS: List of options for sftp.
#
# Example
# For a hosting service "Host1" with git,
# while uploading the source package to "Host2" with sftp.
# The setting file might looks as follows:
#
# SOURCE_VERSION_CONTROL=git
# # No, Host1 is not needed here.
# HOSTING_SERVICES=Host2
#
# Host2_SFTP_USER=host2account
# Host2_SFTP_SITE=host2hostname
# Host2_SFTP_BATCH_TEMPLATE=BatchUpload-Host2.in
#

IF(NOT DEFINED _USE_HOSTING_SERVICE_CMAKE_)
    SET(_USE_HOSTING_SERVICE_CMAKE_ "DEFINED")
    MACRO(USE_HOSTING_SERVICE_SFTP alias user site)
	SET(_stage "NONE")
	FOREACH(_arg ${ARGN})
	    IF(_arg STREQUAL "BATCH")
		SET(_stage "BATCH")
	    ELSEIF(_arg STREQUAL "BATCH_TEMPLATE")
		SET(_stage "BATCH_TEMPLATE")
	    ELSEIF(_arg STREQUAL "OPTIONS")
		SET(_stage "OPTIONS")
	    ELSE(_arg STREQUAL "BATCH")
		IF(_stage STREQUAL "BATCH")
		    SET(_batch ${_arg})
		    SET(_stage "NONE")
		ELSEIF(_stage STREQUAL "BATCH_TEMPLATE")
		    SET(_batch_template ${_arg})
		    SET(_stage "NONE")
		ELSEIF(_stage STREQUAL "OPTIONS")
		    SET(_optionStr "${_optionStr} ${_arg}")
		ELSE(_stage STREQUAL "BATCH")
		    MESSAGE(FATAL_ERROR "Invalid option (${_arg}) for USE_HOSTING_SERVICE_SFTP!")
		ENDIF(_stage STREQUAL "SERVER_TYPE")
	    ENDIF(_arg STREQUAL "SERVER_TYPE")
	ENDFOREACH(_arg ${ARGN})
	SET(_developer_upload_cmd "sftp")
	IF(_batch_template)
	    IF(NOT _batch)
		SET(_batch ${CMAKE_BINARY_DIR}/BatchUpload-${alias})
	    ENDIF(NOT _batch)
	    CONFIGURE_FILE(${_batch_template} ${_batch})
	    SET(PACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES} ${_batch})
	ENDIF(_batch_template)

	IF(_batch STREQUAL "")
	    SET(_developer_upload_cmd "${_developer_upload_cmd} -b ${_batch}" )
	ENDIF(_batch STREQUAL "")

	IF(_optionStr)
	    SET(_developer_upload_cmd "${_developer_upload_cmd} -F ${_optionStr}" )
	ENDIF(_optionStr)
	SET(_developer_upload_cmd "${_developer_upload_cmd} ${user}@${site}")

	ADD_CUSTOM_TARGET(upload_${alias}
	    COMMAND ${_developer_upload_cmd}
	    DEPENDS ${DEVELOPER_DEPENDS}
	    COMMENT "Uploading the package releases to ${alias}..."
	    VERBATIM
	    )
    ENDMACRO(USE_HOSTING_SERVICE_SFTP user site)

    MACRO(USE_HOSTING_SERVICE_GOOGLE_UPLOAD)
	FIND_PROGRAM(CURL_CMD curl)
	IF(CURL_CMD STREQUAL "CURL_CMD-NOTFOUND")
	    MESSAGE(FATAL_ERROR "Need curl to perform google upload")
	ENDIF(CURL_CMD STREQUAL "CURL_CMD-NOTFOUND")
    ENDMACRO(USE_HOSTING_SERVICE_GOOGLE_UPLOAD)

    MACRO(USE_HOSTING_SERVICE_READ_SETTING_FILE filename)
	IF(EXISTS "${filename}")
	    INCLUDE(ManageVariable)
	    INCLUDE(ManageVersion)
	    SETTING_FILE_GET_ALL_ATTRIBUTES("${filename}" UNQUOTED)

	    #===================================================================
	    # Targets:
	    ADD_CUSTOM_TARGET(upload
		COMMENT "Uploading source to hosting services"
		)

	    ADD_CUSTOM_TARGET(changelog_update
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/ChangeLog ${CMAKE_SOURCE_DIR}/ChangeLog.prev
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/${RPM_BUILD_SPECS}/RPM-ChangeLog
		${CMAKE_SOURCE_DIR}/${RPM_BUILD_SPECS}/RPM-ChangeLog.prev
		COMMENT "Changelogs are updated for next version."
		)

	    IF(SOURCE_VERSION_CONTROL STREQUAL "git")
		ADD_CUSTOM_TARGET(tag
		    COMMAND git tag -a -m "${CHANGE_SUMMARY}" "${PRJ_VER}" HEAD
		    COMMENT "Tagging the source as ver ${PRJ_VER}"
		    )
		ADD_DEPENDENCIES(tag version_check)
	    ELSEIF(SOURCE_VERSION_CONTROL STREQUAL "hg")
		ADD_CUSTOM_TARGET(tag
		    COMMAND hg tag -m "${CHANGE_SUMMARY}" "${PRJ_VER}"
		    COMMENT "Tagging the source as ver ${PRJ_VER}"
		    )
		ADD_DEPENDENCIES(tag version_check)
	    ELSEIF(SOURCE_VERSION_CONTROL STREQUAL "svn")
		IF(DEFINED SOURCE_BASE_URL)
		    ADD_CUSTOM_TARGET(tag
			COMMAND svn copy "${SOURCE_BASE_URL}/trunk" "${SOURCE_BASE_URL}/tags/${PRJ_VER}" -m "${CHANGE_SUMMARY}"
			COMMENT "Tagging the source as ver ${PRJ_VER}"
			)
		ENDIF(DEFINED SOURCE_BASE_URL)
		ADD_DEPENDENCIES(tag version_check)
	    ELSEIF(SOURCE_VERSION_CONTROL STREQUAL "cvs")
		ADD_CUSTOM_TARGET(tag
		    COMMAND cvs tag "${PRJ_VER}"
		    COMMENT "Tagging the source as ver ${PRJ_VER}"
		    )
		ADD_DEPENDENCIES(tag version_check)
	    ENDIF(SOURCE_VERSION_CONTROL STREQUAL "git")

	    # Setting for each hosting service
	    FOREACH(_service ${HOSTING_SERVICES})
		IF(_service STREQUAL "SOURCEFORGE")
		    USE_HOSTING_SERVICE_SFTP("${_service}" ${SOURCEFORGE_SFTP_USER}
			frs.sourceforge.net
			BATCH ${SOURCEFORGE_SFTP_BATCH}
			BATCH_TEMPLATE ${SOURCEFORGE_SFTP_BATCH_TEMPLATE}
			OPTIONS ${SOURCEFORGE_SFTP_OPTIONS})
		ELSEIF(_service STREQUAL "GOOGLECODE")
		    USE_HOSTING_SERVICE_GOOGLE_UPLOAD()
		ELSEIF(_service STREQUAL "GITHUB")
		ELSE(_service STREQUAL "SOURCEFORGE")
		    # Generic hosting service
		    IF(NOT "${${_service}_SFTP_USER}" STREQUAL "")
			USE_HOSTING_SERVICE_SFTP("${_service}" ${${_service}_SFTP_USER}
			    ${${_service}_SFTP_SITE}
			    BATCH ${${_service}_SFTP_BATCH}
			    BATCH_TEMPLATE ${${_service}_SFTP_BATCH_TEMPLATE}
			    OPTIONS ${${_service}_SFTP_OPTIONS})
			# SFTP hosting service
		    ENDIF(NOT "${${_service}_SFTP_USER}" STREQUAL "")
		ENDIF(_service STREQUAL "SOURCEFORGE")
		ADD_DEPENDENCIES(upload "upload_${_service}")

	    ENDFOREACH(_service ${HOSTING_SERVICES})

	ENDIF(EXISTS "${filename}")

    ENDMACRO(USE_HOSTING_SERVICE_READ_SETTING_FILE filename)

ENDIF(NOT DEFINED _USE_HOSTING_SERVICE_CMAKE_)

