# - koji, fedpkg and bodhi targets for Fedora and EPEL developers.
# This module provides convenient targets for scratch build, submit,
# and build on koji, using the GIT infrastructure,
# as well as bodhi update.
# Since this module is mainly for Fedora developers/maintainers,
# This module checks ~/.fedora-upload-ca.cert
#
# Defines following variable:
#   FEDORA_RAWHIDE_TAG: Koji tags for rawhide
#   FEDORA_CURRENT_RELEASE_TAGS: Current tags of fedora releases.
# Defines following macros:
#   USE_FEDPKG(srpm [NORAWHIDE] [tag1 [tag2 ...])
#   - Use fedpkg targets if ~/.fedora-upload-ca.cert exists.
#     If ~/.fedora-upload-ca.cert does not exists, this marcos run as an empty
#     macro.
#     Argument:
#     + srpm: srpm file with path.
#     + NORAWHIDE: Don't build on rawhide.
#     + tag1, tag2...: Dist tags such as f14, f13, el5.
#       if no defined, then tags in FEDORA_CURRENT_RELEASE_TAGS are used.
#     Reads following variables:
#     + FEDORA_RELEASE_TAGS: (optional) Release to be built.
#       Note this override the setting from [tag1 [tag2 ...].
#       Thus, this variable can be defined in RELEASE.txt to specify the
#       dist tags to be built.
#     + FEDPKG_DIR: Directory for fedpkg checkout.
#       Default: FedPkg.
#     Defines following targets:
#     + fedpkg_scratch_build: Perform scratch build with fedpkg.
#     + fedpkg_submit: Submit build with fedpkg.
#     + fedpkg_build: Perform build with fedpkg.
#     + fedpkg_update: Update with fedpkg.
#     + koji_scratch_build: Sent srpm for scratch build
#   USE_BODHI([TAGS [tag1 [tag2 ...]] [KARMA karmaValue] )
#   - Use bodhi targets with bodhi command line client.
#     Argument:
#     + TAGS tag1, ....: Dist Tags for submission. Accepts formats like f14,
#        fc14, el6.
#     + KARMA karmaValue: Set the karma threshold. Default is 3.
#     Reads following variables:
#     + BODHI_UPDATE_TYPE: Type of update. Default is "bugfix".
#     + BODHI_USER: Login username for bodhi (for -u).
#     + FEDORA_CURRENT_RELEASE_TAGS: If TAGS is not defined, then it will be
#       use as default tags.
#     + SUGGEST_REBOOT: Whether this update require reboot to take effect.
#       Default is "False".
#     Defines following targets:
#     + bodhi_new: Send a new release to bodhi.
#

IF(NOT DEFINED _USE_FEDPKG_CMAKE_)
    SET(_USE_FEDPKG_CMAKE_ "DEFINED")
    SET(FEDORA_CURRENT_RELEASE_TAGS f14 f13)
    SET(FEDORA_RAWHIDE_TAG rawhide)
    IF(NOT DEFINED FEDPKG_DIR)
	SET(FEDPKG_DIR "FedPkg")
    ELSEIF (FEDPKG_DIR STREQUAL "")
	SET(FEDPKG_DIR "FedPkg")
    ENDIF(NOT DEFINED FEDPKG_DIR)
    SET(_bodhi_template_file "bodhi.NO_PACK.template")
    SET(PACK_SOURCE_IGNORE_FILES ${PACK_SOURCE_IGNORE_FILES} "/${FEDPKG_DIR}/")

    MACRO(_use_koji_make_cmds srpm tags)
	#commit
	IF(DEFINED CHANGE_SUMMARY)
	    SET (COMMIT_MSG  "-m \"${CHANGE_SUMMARY}\"")
	ELSE(DEFINED CHANGE_SUMMARY)
	    SET (COMMIT_MSG  "")
	ENDIF(DEFINED CHANGE_SUMMARY)
	SET(_first_branch "")
	FOREACH(_tag ${tags})
	    IF(_tag MATCHES "^el")
		STRING(REGEX REPLACE "el\([0-9]+\)"
		    "\\1E-epel-testing-candidate"  _branch
		    "${_tag}")
	    ELSE(_tag MATCHES "^el")
		SET(_branch "${_tag}")
	    ENDIF(_tag MATCHES "^el")

	    IF(_first_branch STREQUAL "")
		SET(_first_branch ${_branch})
		SET(KOJI_SCRATCH_BUILD_CMD "${KOJI} build --scratch dist-${_branch} ${srpm}")
	    ELSE(_first_branch STREQUAL "")
		SET(KOJI_SCRATCH_BUILD_CMD "${KOJI_SCRATCH_BUILD_CMD}"
		    " ${KOJI} build --scratch dist-${_branch} ${srpm}")
	    ENDIF(_first_branch STREQUAL "")
	ENDFOREACH(_tag ${tags})
    ENDMACRO(_use_koji_make_cmds srpm tags)

    MACRO(_use_fedpkg_make_cmds srpm tags)
	#commit
	IF (DEFINED CHANGE_SUMMARY)
	    SET (COMMIT_MSG  "-m \"${CHANGE_SUMMARY}\"")
	ELSE(DEFINED CHANGE_SUMMARY)
	    SET (COMMIT_MSG  "")
	ENDIF(DEFINED CHANGE_SUMMARY)
	SET(_first_branch "")

	FOREACH(_tag ${tags})
	    IF(_tag STREQUAL "${FEDORA_RAWHIDE_TAG}")
		SET(_branch "master")
	    ELSE(_tag STREQUAL "${FEDORA_RAWHIDE_TAG}")
		SET(_branch "${_tag}")
	    ENDIF(_tag STREQUAL "${FEDORA_RAWHIDE_TAG}")

	    IF(_first_branch STREQUAL "")
		SET(_first_branch ${_branch})
		SET(FEDPKG_SCRATCH_BUILD_CMD
		    "${FEDPKG} switch-branch ${_branch}"
		    " git pull"
		    " ${FEDPKG} scratch-build --srpm ${srpm}")
		SET(FEDPKG_COMMIT_CMD
		    "${FEDPKG} switch-branch ${_branch}"
		    " git pull"
		    " ${FEDPKG} import  ${srpm}"
		    " ${FEDPKG} commit ${COMMIT_MSG} -p"
		    )
		SET(FEDPKG_BUILD_CMD
		    "${FEDPKG} switch-branch ${_branch}"
		    " git pull"
		    " ${FEDPKG} build")
		SET(FEDPKG_UPDATE_CMD
		    "${FEDPKG} switch-branch ${_branch}"
		    " git pull"
		    " ${FEDPKG} update")
	    ELSE(_first_branch STREQUAL "")
		SET(FEDPKG_SCRATCH_BUILD_CMD "${FEDPKG_SCRATCH_BUILD_CMD}"
		    " ${FEDPKG} switch-branch ${_branch}"
		    " git pull"
		    " ${FEDPKG} scratch-build --srpm ${srpm}")
		SET(FEDPKG_COMMIT_CMD "${FEDPKG_COMMIT_CMD}"
		    " ${FEDPKG} switch-branch ${_branch}"
		    " git pull"
		    " git merge ${_first_branch}"
		    " git push")
		SET(FEDPKG_BUILD_CMD "${FEDPKG_BUILD_CMD} "
		    " ${FEDPKG} switch-branch ${_branch}"
		    " git pull"
		    " ${FEDPKG} build")
		SET(FEDPKG_UPDATE_CMD "${FEDPKG_UPDATE_CMD}"
		    " ${FEDPKG} switch-branch ${_branch}"
		    " git pull"
		    " ${FEDPKG} update")
	    ENDIF(_first_branch STREQUAL "")
	ENDFOREACH(_tag ${tags})
    ENDMACRO(_use_fedpkg_make_cmds tags srpm)

    MACRO(USE_FEDPKG srpm)
	IF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
	    SET(_rawhide 1)
	    SET(_koji_dist_tags "")
	    SET(_stage "")
	    FOREACH(_arg ${ARGN})
		IF ("${_arg}" STREQUAL "NORAWHIDE")
		    SET(_rawhide 0)
		ELSEIF("${_arg}" STREQUAL "TAGS")
		    SET(_stage "TAGS")
		ELSE("${_arg}" STREQUAL "NORAWHIDE")
		    IF(_stage STREQUAL "TAGS")
			SET(_koji_dist_tags ${_koji_dist_tags} ${_arg})
		    ENDIF(_stage STREQUAL "TAGS")
		ENDIF("${_arg}" STREQUAL "NORAWHIDE")
	    ENDFOREACH(_arg)

	    IF(_koji_dist_tags STREQUAL "")
		SET(_koji_dist_tags ${FEDORA_CURRENT_RELEASE_TAGS})
	    ENDIF(_koji_dist_tags STREQUAL "")

	    IF(NOT "${FEDORA_RELEASE_TAGS}" STREQUAL "")
		SET(_koji_dist_tags ${FEDORA_RELEASE_TAGS})
	    ENDIF(NOT "${FEDORA_RELEASE_TAGS}" STREQUAL "")

	    IF(_rawhide EQUAL 1)
		SET(_koji_dist_tags ${FEDORA_RAWHIDE_TAG} ${_koji_dist_tags})
	    ENDIF(_rawhide EQUAL 1)

	    FIND_PROGRAM(FEDPKG fedpkg)
	    IF(FEDPKG STREQUAL "FEDPKG-NOTFOUND")
		MESSAGE("Program fedpkg is not found!")
	    ELSE(FEDPKG STREQUAL "FEDPKG-NOTFOUND")
		ADD_CUSTOM_COMMAND(OUTPUT ${FEDPKG_DIR}
		    COMMAND mkdir -p ${FEDPKG_DIR}
		    )

		ADD_CUSTOM_COMMAND(OUTPUT ${FEDPKG_DIR}/${PROJECT_NAME}
		    COMMAND fedpkg clone ${PROJECT_NAME}
		    DEPENDS ${FEDPKG_DIR}
		    WORKING_DIRECTORY ${FEDPKG_DIR}
		    )


		## Make target commands for the released dist
		_use_fedpkg_make_cmds("${srpm}" "${_koji_dist_tags}")

		#MESSAGE(FEDPKG_SCRATCH_BUILD_CMD=${FEDPKG_SCRATCH_BUILD_CMD})
		ADD_CUSTOM_TARGET(fedpkg_scratch_build
		    COMMAND eval "${FEDPKG_SCRATCH_BUILD_CMD}"
		    DEPENDS ${FEDPKG_DIR}/${PROJECT_NAME} ${srpm} ChangeLog
		    WORKING_DIRECTORY ${FEDPKG_DIR}/${PROJECT_NAME}
		    COMMENT "Start fedpkg scratch build"
		    VERBATIM
		    )

		#MESSAGE(FEDPKG_COMMIT_CMD=${FEDPKG_COMMIT_CMD})
		ADD_CUSTOM_TARGET(fedpkg_commit
		    COMMAND eval "${FEDPKG_COMMIT_CMD}"
		    DEPENDS ${FEDPKG_DIR}/${PROJECT_NAME} ${srpm} ChangeLog
		    WORKING_DIRECTORY ${FEDPKG_DIR}/${PROJECT_NAME}
		    COMMENT "Submitting build to fedpkg"
		    VERBATIM
		    )

		#MESSAGE("FEDPKG_BUILD_CMD=${FEDPKG_BUILD_CMD}")
		ADD_CUSTOM_TARGET(fedpkg_build
		    COMMAND eval "${FEDPKG_BUILD_CMD}"
		    DEPENDS ${FEDPKG_DIR}/${PROJECT_NAME} ${srpm} ChangeLog
		    WORKING_DIRECTORY ${FEDPKG_DIR}/${PROJECT_NAME}
		    COMMENT "Building with fedpkg"
		    VERBATIM
		    )

		#MESSAGE("FEDPKG_BUILD_CMD=${FEDPKG_BUILD_CMD}")
		ADD_CUSTOM_TARGET(fedpkg_update
		    COMMAND eval "${FEDPKG_UPDATE_CMD}"
		    DEPENDS ${FEDPKG_DIR}/${PROJECT_NAME} ${srpm} ChangeLog
		    WORKING_DIRECTORY ${FEDPKG_DIR}/${PROJECT_NAME}
		    COMMENT "Updating with fedpkg"
		    VERBATIM
		    )
	    ENDIF(FEDPKG STREQUAL "FEDPKG-NOTFOUND")
	    FIND_PROGRAM(KOJI koji)
	    IF(KOJI STREQUAL "KOJI-NOTFOUND")
		MESSAGE("Program koji is not found!")
	    ELSE(KOJI STREQUAL "KOJI-NOTFOUND")
		## Make target commands for the released dist
		_use_koji_make_cmds("${srpm}" "${_koji_dist_tags}")

		#MESSAGE("KOJI_SCRATCH_BUILD_CMD=${KOJI_SCRATCH_BUILD_CMD}")
		ADD_CUSTOM_TARGET(koji_scratch_build
		    COMMAND eval "${KOJI_SCRATCH_BUILD_CMD}"
		    DEPENDS ${srpm} ChangeLog
		    COMMENT "Start Koji scratch build"
		    VERBATIM
		    )
	    ENDIF(KOJI STREQUAL "KOJI-NOTFOUND")

	ENDIF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
    ENDMACRO(USE_FEDPKG srpm)

    MACRO(_use_bodhi_convert_tag tag_out tag_in)
	STRING(REGEX REPLACE "f([0-9]+)" "fc\\1" _tag_replace "${tag_in}")
	IF(_tag_replace STREQUAL "")
	    SET(${tag_out} ${tag_in})
	ELSE(_tag_replace STREQUAL "")
	    SET(${tag_out} ${_tag_replace})
	ENDIF(_tag_replace STREQUAL "")
    ENDMACRO(_use_bodhi_convert_tag tag_out tag_in)

    MACRO(_append_notes _file)
	STRING(REGEX REPLACE "\n" "\n " _notes "${CHANGELOG_ITEMS}")
	FILE(APPEND ${_file} "notes=${_notes}\n\n")
    ENDMACRO(_append_notes _file)

    MACRO(USE_BODHI)
	# Bodhi does not really require .fedora-upload-ca.cert
	# But since this macro is meant for package maintainers,
	# so..
	SET(_autokarma "True")
	SET(_stable_karma 3)
	SET(_unstable_karma -3)
	SET(_tags "")
	IF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
	    FIND_PROGRAM(BODHI bodhi)
	    IF(BODHI STREQUAL "BODHI-NOTFOUND")
		MESSAGE(FATAL-ERROR "Program bodhi is not found!")
	    ENDIF(BODHI STREQUAL "BODHI-NOTFOUND")

	    SET(_stage "NONE")
	    FOREACH(_arg ${ARGN})
		IF(_arg STREQUAL "TAGS")
		    SET(_stage "TAGS")
		ELSEIF(_arg STREQUAL "KARMA")
		    SET(_stage "KARMA")
		ELSE(_arg STREQUAL "TAGS")
		    # option values
		    IF(_stage STREQUAL "TAGS")
			SET(_tags ${_tags} ${_arg})
		    ELSEIF(_stage STREQUAL "KARMA")
			IF(_arg STREQUAL "0")
			    SET(_autokarma "False")
			ELSE(_arg STREQUAL "0")
			    SET(_autokarma "True")
			ENDIF(_arg STREQUAL "0")
			SET(_stable_karma "${_arg}")
			SET(_unstable_karma "-${_arg}")
			SET(_tags ${_arg})
		    ENDIF(_stage STREQUAL "TAGS")
		ENDIF(_arg STREQUAL "TAGS")
	    ENDFOREACH(_arg ${ARGN})

	    IF(NOT _tags)
		SET(_tags ${FEDORA_CURRENT_RELEASE_TAGS})
	    ENDIF(NOT _tags)

	    IF(NOT "${FEDORA_RELEASE_TAGS}" STREQUAL "")
		SET(_tags ${FEDORA_RELEASE_TAGS})
	    ENDIF(NOT "${FEDORA_RELEASE_TAGS}" STREQUAL "")

	    FILE(REMOVE ${_bodhi_template_file})
	    FOREACH(_tag ${_tags})
		_use_bodhi_convert_tag(_bodhi_tag ${_tag})

		FILE(APPEND ${_bodhi_template_file} "[${PROJECT_NAME}-${PRJ_VER}-${PRJ_RELEASE_NO}.${_bodhi_tag}]\n\n")

		IF(BODHI_UPDATE_TYPE)
		    FILE(APPEND ${_bodhi_template_file} "type=${BODHI_UPDATE_TYPE}\n\n")
		ELSE(BODHI_UPDATE_TYPE)
		    FILE(APPEND ${_bodhi_template_file} "type=bugfix\n\n")
		ENDIF(BODHI_UPDATE_TYPE)

		FILE(APPEND ${_bodhi_template_file} "request=testing\n")
		FILE(APPEND ${_bodhi_template_file} "bugs=${REDHAT_BUGZILLA}\n")

		_append_notes(${_bodhi_template_file})

		FILE(APPEND ${_bodhi_template_file} "autokarma=${_autokarma}\n")
		FILE(APPEND ${_bodhi_template_file} "stable_karma=${_stable_karma}\n")
		FILE(APPEND ${_bodhi_template_file} "unstable_karma=${_unstable_karma}\n")
		FILE(APPEND ${_bodhi_template_file} "close_bugs=True\n")

		IF(SUGGEST_REBOOT)
		    FILE(APPEND ${_bodhi_template_file} "suggest_reboot=True\n")
		ELSE(SUGGEST_REBOOT)
		    FILE(APPEND ${_bodhi_template_file} "suggest_reboot=False\n\n")
		ENDIF(SUGGEST_REBOOT)
	    ENDFOREACH(_tag ${_tags})

	    IF(BODHI_USER)
		SET(_bodhi_login "-u ${BODHI_USER}")
	    ENDIF(BODHI_USER)

	    ADD_CUSTOM_TARGET(bodhi_new
		COMMAND eval "bodhi --new ${_bodhi_login} --file ${_bodhi_template_file}"
		COMMENT "Send new package to bodhi"
		VERBATIM
		)

	ENDIF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
    ENDMACRO(USE_BODHI)

ENDIF(NOT DEFINED _USE_FEDPKG_CMAKE_)

