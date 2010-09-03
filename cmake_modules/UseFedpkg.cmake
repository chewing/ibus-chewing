# - fedpkg targets for Fedora and EPEL developers.
# This module provides convenient targets for scratch build, submit,
# and build on koji, using the GIT infrastructure,
# as well as bodhi update.
# Since this module is mainly for Fedora developers/maintainers,
# This module checks ~/.fedora-upload-ca.cert
#
# Defines following macros:
#   USE_FEDPKG(srpm [NORAWHIDE] [tag1 [tag2 ...])
#   - Use koji and bodhi targets if ~/.fedora-upload-ca.cert exists.
#     If ~/.fedora-upload-ca.cert does not exists, this marcos run as an empty
#     macro.
#     Argument:
#     + srpm: srpm file with path.
#     + NORAWHIDE: Don't build on rawhide.
#     + tag1, tag2...: Dist tags such as f14, f13, el5.
#       if no defined, then tags in FEDORA_CURRENT_RELEASE_TAGS are used.
#     Reads following variables:
#     + FEDPKG_DIR: Directory for fedpkg checkout.
#       Default: FedPkg.
#     + BODHI_USER: (Optional) If defined, the specified user
#       is then use for bodhi update instead of your current login name.
#     Defines following targets:
#     + koji_scratch_build: Sent srpm for scratch build
#     + koji_submit: Submit package to koji for each tag.
#     + koji_build: Build package with koji for each tag.
#     + bodhi_new: Submit to bodhi
# Reads and defines following variables:
#   FEDORA_CURRENT_RELEASE_TAGS: Current tags of fedora releases.
# Defines following variable:
#   FEDORA_RAWHIDE_TAG: Koji tags for rawhide
#
#

IF(NOT DEFINED _USE_FEDPKG_CMAKE_)
    SET(_USE_FEDPKG_CMAKE_ "DEFINED")
    IF(NOT DEFINED FEDORA_CURRENT_RELEASE_TAGS)
	SET(FEDORA_CURRENT_RELEASE_TAGS f14 f13 f12)
    ENDIF(NOT DEFINED FEDORA_CURRENT_RELEASE_TAGS)
    SET(FEDORA_RAWHIDE_TAG rawhide)
    MESSAGE("CMAKE_HOST_SYSTEM=${CMAKE_HOST_SYSTEM} "
	"CMAKE_HOST_SYSTEM_NAME=${CMAKE_HOST_SYSTEM_NAME}")

    MACRO(_use_fedpkg_make_cmds srpm tags)
	#commit
	IF (DEFINED CHANGE_SUMMARY)
	    SET (COMMIT_MSG  "-m \"${CHANGE_SUMMARY}\"")
	ELSE(DEFINED CHANGE_SUMMARY)
	    SET (COMMIT_MSG  "")
	ENDIF(DEFINED CHANGE_SUMMARY)

	FOREACH(_tag ${tags})
	    IF(_tag STREQUAL "${FEDORA_RAWHIDE_TAG}")
		SET(_branch "master")
	    ELSE(_tag STREQUAL "${FEDORA_RAWHIDE_TAG}")
		SET(_branch "${_tag}")
	    ENDIF(_tag STREQUAL "${FEDORA_RAWHIDE_TAG}")

	    IF(DEFINED _first_branch)
		SET(FEDPKG_SCRATCH_BUILD_CMD "${FEDPKG_SCRATCH_BUILD_CMD}\; "
		    "${FEDPKG} switch-branch ${_branch}\; "
		    "${FEDPKG} scratch-build --srpm ${srpm}")
		SET(FEDPKG_COMMIT_CMD "${FEDPKG_COMMIT_CMD}\; "
		    "${FEDPKG} switch-branch ${_branch}\; "
		    "git merge ${_first_branch}\; "
		    "git push\; ")
		SET(FEDPKG_BUILD_CMD "${FEDPKG_BUILD_CMD}\; "
		    "${FEDPKG} switch-branch ${_branch}\; "
		    "${FEDPKG} build")
		SET(FEDPKG_UPDATE_CMD "${FEDPKG_UPDATE_CMD}\; "
		    "${FEDPKG} switch-branch ${_branch}\; "
		    "${FEDPKG} update")
	    ELSE(DEFINED _first_branch)
		SET(_first_branch ${_branch})
		SET(FEDPKG_SCRATCH_BUILD_CMD
		    "${FEDPKG} switch-branch ${_branch}\; "
		    "${FEDPKG} scratch-build --srpm ${srpm}")
		SET(FEDPKG_COMMIT_CMD
		    "${FEDPKG} switch-branch ${_branch}\; "
		    "${FEDPKG} import  ${srpm}"
		    "${FEDPKG} commit ${COMMIT_MSG} -p"
		    )
		SET(FEDPKG_BUILD_CMD
		    "${FEDPKG} switch-branch ${_branch}\; "
		    "${FEDPKG} build")
		SET(FEDPKG_UPDATE_CMD
		    "${FEDPKG} switch-branch ${_branch}\; "
		    "${FEDPKG} update")
	    ENDIF(DEFINED _first_branch)
	ENDFOREACH(_tag ${tags})
    ENDMACRO(_use_fedpkg_make_cmds tags srpm)

    MACRO(USE_FEDPKG srpm)
	IF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
	    FIND_PROGRAM(FEDPKG fedpkg)
	    IF(FEDPKG STREQUAL "FEDPKG-NOTFOUND")
		MESSAGE(FATAL-ERROR "Program fedpkg is not found!")
	    ENDIF(FEDPKG STREQUAL "FEDPKG-NOTFOUND")
	    IF(NOT DEFINED FEDPKG_DIR)
		SET(FEDPKG_DIR "FedPkg")
	    ELSEIF (FEDPKG_DIR STREQUAL "")
		SET(FEDPKG_DIR "FedPkg")
	    ENDIF(NOT DEFINED FEDPKG_DIR)
	    ADD_CUSTOM_COMMAND(OUTPUT ${FEDPKG_DIR}
		COMMAND mkdir -p ${FEDPKG_DIR}
		)

	    ADD_CUSTOM_COMMAND(OUTPUT ${FEDPKG_DIR}/${PROJECT_NAME}
		COMMAND fedpkg clone ${PROJECT_NAME}
		DEPENDS ${FEDPKG_DIR}
		WORKING_DIRECTORY ${FEDPKG_DIR}
		)


	    SET(_rawhide 1)
	    FOREACH(_arg ${ARGN})
		IF ("${_arg}" STREQUAL "NORAWHIDE")
		    SET(_rawhide 0)
		ELSE("${_arg}" STREQUAL "NORAWHIDE")
		    SET(_koji_dist_tags ${_koji_dist_tags} ${_arg})
		ENDIF("${_arg}" STREQUAL "NORAWHIDE")
	    ENDFOREACH(_arg)

	    IF(NOT DEFINED _koji_dist_tags)
		SET(_koji_dist_tags ${FEDORA_CURRENT_RELEASE_TAGS})
	    ENDIF(NOT DEFINED _koji_dist_tags)

	    IF(_rawhide EQUAL 1)
		SET(_koji_dist_tags ${FEDORA_RAWHIDE_TAG} ${_koji_dist_tags})
	    ENDIF(_rawhide EQUAL 1)

	    ## Make target commands for the released dist
	    _use_fedpkg_make_cmds(${_koji_dist_tags} srpm)

	    #MESSAGE(FEDPKG_SCRATCH_BUILD_CMD=${FEDPKG_SCRATCH_BUILD_CMD})
	    ADD_CUSTOM_TARGET(fedpkg_scratch_build
		COMMAND eval ${FEDPKG_SCRATCH_BUILD_CMD}
		DEPENDS ${FEDPKG_DIR}/${PROJECT_NAME} ${srpm}
		COMMENT "Start Koji scratch build"
		VERBATIM
		)

	    #MESSAGE(FEDPKG_COMMIT_CMD=${FEDPKG_COMMIT_CMD})
	    ADD_CUSTOM_TARGET(fedpkg_commit
		COMMAND echo "${FEDPKG_COMMIT_CMD}"
		DEPENDS ${FEDPKG_DIR}/${PROJECT_NAME} ${srpm}
		COMMENT "Submitting to Koji"
		WORKING_DIRECTORY ${FEDPKG_DIR}/${PROJECT_NAME}
		VERBATIM
		)

	    #MESSAGE("FEDPKG_BUILD_CMD=${FEDPKG_BUILD_CMD}")
	    ADD_CUSTOM_TARGET(fedpkg_build
		COMMAND echo "${FEDPKG_BUILD_CMD}"
		COMMENT "Building on Koji"
		WORKING_DIRECTORY ${FEDPKG_DIR}/${PROJECT_NAME}
		VERBATIM
		)

	    #MESSAGE("FEDPKG_BUILD_CMD=${FEDPKG_BUILD_CMD}")
	    ADD_CUSTOM_TARGET(fedpkg_update
		COMMAND echo "${FEDPKG_UPDATE_CMD}"
		COMMENT "Updating on Bodhi"
		WORKING_DIRECTORY ${FEDPKG_DIR}/${PROJECT_NAME}
		VERBATIM
		)

	    #====================================================================
	    # Bodhi (Fedora update system)
	    #

	    IF(DEFINED BODHI_DIST_TAGS)
		SET(${_bodhi_dist_tags} ${BODHI_DIST_TAGS})
	    ENDIF(DEFINED BODHI_DIST_TAGS)

	    FOREACH(_bodhi_tag ${_bodhi_dist_tags})
		IF (DEFINED BODHI_NEW_CMD)
		    SET (BODHI_NEW_CMD "${BODHI_NEW_CMD} ; ")
		ELSE ()
		    SET (BODHI_NEW_CMD "")
		ENDIF ()
		IF(DEFINED RPM_RELEASE_SUMMARY)
		    SET(commentArg "--comment=\"${RPM_RELEASE_SUMMARY}\"")
		ELSEIF(DEFINED CHANGE_SUMMARY)
		    SET(commentArg "--comment=\"${CHANGE_SUMMARY}\"")
		ENDIF()
		SET (BODHI_NEW_CMD
		    "${BODHI_NEW_CMD} bodhi --new --type=bugfix ${commentArg} ${PROJECT_NAME}-${PRJ_VER_FULL}.${_bodhi_tag}")
	    ENDFOREACH(_bodhi_tag)

	    #MESSAGE(BODHI_NEW_CMD=${BODHI_NEW_CMD})
	    IF(DEFINED BODHI_NEW_CMD)
		ADD_CUSTOM_TARGET(bodhi_new
		    COMMAND eval "${BODHI_NEW_CMD}"
		    DEPENDS ${BODHI_DEPENDS}
		    COMMENT "Send new package to bodhi"
		    VERBATIM
		    )
	    ENDIF(DEFINED BODHI_NEW_CMD)

	ENDIF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
    ENDMACRO(USE_FEDPKG srpm)

    MACRO(_use_bodhi_convert_tag tag_out tag_in)
	STRING(REGEX REPLACE "f([0-9]+)" "fc\\1" _tag_replace "${tag_in}")
	IF(_tag_replace STREQUAL "")
	    SET(${tag_out} ${tag_in})
	ELSE(_tag_replace STREQUAL "")
	    SET(${tag_out} ${tag_replace})
	ENDIF(_tag_replace STREQUAL "")
    ENDMACRO(_use_bodhi_convert_tag tag_out tag_in)

    MACRO(USE_BODHI)
	# Bodhi does not really require .fedora-upload-ca.cert
	# But since this macro is meant for package maintainers,
	# so..
	IF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
	    FIND_PROGRAM(BODHI bodhi)
	    IF(BODHI STREQUAL "BODHI-NOTFOUND")
		MESSAGE(FATAL-ERROR "Program bodhi is not found!")
	    ENDIF(BODHI STREQUAL "BODHI-NOTFOUND")

	    FOREACH(_arg ${ARGN})
		SET(_stage "NONE")
		IF(_arg STREQUAL "TAGS")
		    SET(_stage "TAGS")
		ELSEIF(_arg STREQUAL "KARMA")
		    SET(_stage "KARMA")
		ELSE(_arg STREQUAL "TAGS")
		    # option values
		    IF(_stage STREQUAL "TAGS")
			SET(_tags ${_arg})
		    ELSEIF(_stage STREQUAL "KARMA")
			IF(_arg STREQUAL "0")
			    SET(_autokarma "False")
			    SET(_stable_karma "${_arg}")
			ELSE(_arg STREQUAL "0")
			    SET(_autokarma "True")
			    SET(_stable_karma "${_arg}")
			ENDIF(_arg STREQUAL "0")
			SET(_tags ${_arg})
		    ENDIF(_stage STREQUAL "TAGS")
		ENDIF(_arg STREQUAL "TAGS")
	    ENDFOREACH(_arg ${ARGN})

	    IF(_tags)
		SET(_tags ${FEDORA_CURRENT_RELEASE_TAGS})
	    ENDIF(_tags)
	    SET(_bodhi_template_file "bodhi.template")
	    FILE(REMOVE ${_bodhi_template_file})

	    FOREACH(_tag ${_tags})
		_use_bodhi_convert_add_tag(_bodhi_tag ${_tag})

		FILE(APPEND ${_bodhi_template_file} "[${PROJECT_NAME}-${PRJ_VER}-${PRJ_RELEASE_VER}.${_bodhi_tag}]\n\n")

		IF(BODHI_UPDATE_TYPE)
		    FILE(APPEND ${_bodhi_template_file} "type=${BODHI_UPDATE_TYPE}\n\n")
		ELSE(BODHI_UPDATE_TYPE)
		    FILE(APPEND ${_bodhi_template_file} "type=bugfix\n\n")
		ENDIF(BODHI_UPDATE_TYPE)

		FILE(APPEND ${_bodhi_template_file} "request=testing\n")
		FILE(APPEND ${_bodhi_template_file} "bugs=${REDHAT_BUGZILLA}\n")
		FILE(APPEND ${_bodhi_template_file} "notes=${CHANGELOG_ITEMS}\n\n")

		FILE(APPEND ${_bodhi_template_file} "autokarma=${_autokarma}\n")
		FILE(APPEND ${_bodhi_template_file} "stable_karma=${_stable_karma}\n")
		FILE(APPEND ${_bodhi_template_file} "close_bugs=True\n")

		IF(SUGGEST_REBOOT)
		    FILE(APPEND ${_bodhi_template_file} "suggest_reboot=True\n")
		ELSE(SUGGEST_REBOOT)
		    FILE(APPEND ${_bodhi_template_file} "suggest_reboot=False\n")
		ENDIF(SUGGEST_REBOOT)

	    ENDFOREACH(_tag ${_tags})
	    ADD_CUSTOM_TARGET(bodhi_new
		COMMAND bodhi --new --file ${_bodhi_template_file}
		COMMENT "Send new package to bodhi"
		VERBATIM
		)

	ENDIF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
    ENDMACRO(USE_BODHI)

ENDIF(NOT DEFINED _USE_FEDPKG_CMAKE_)

