# - Add Koji and Bodhi targets for Fedora and EPEL developers.
# This module provides convenient targets for scratch build, submit,
# and build on koji, using the GIT infrastructure.
# Since this module is mainly for Fedora developers/maintainers,
# This module checks ~/.fedora-upload-ca.cert
#
# Defines following macros:
#   UseKoji(srpm [NORAWHIDE] [tag1 [tag2 ...])
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
#     Defines following targets:
#     + koji_scratch_build: Sent srpm for scratch build
#     + koji_submit: Submit package to koji for each tag.
#     + koji_build: Build package with koji for each tag.
#     + bodhi_new: Submit to bodhi
# Defines following variables:
#   FEDORA_CURRENT_RELEASE_TAGS: Current tags of released fedora.
#   FEDORA_RAWHIDE_TAG: Koji tags for rawhide
#
#

IF(NOT DEFINED _USE_KOJI_FEDORA_CMAKE_)
    SET(_USE_KOJI_RPM_ "DEFINED")
    SET(FEDORA_CURRENT_RELEASE_TAGS f12 f13 f14)
    SET(FEDORA_RAWHIDE_TAG rawhide)

    MACRO(UseKoji srpm)
	IF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
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

	    #commit
	    IF (DEFINED RPM_RELEASE_SUMMARY)
		SET (COMMIT_MSG  "-m \"${RPM_RELEASE_SUMMARY}\"")
	    ELSEIF (DEFINED CHANGE_SUMMARY)
		SET (COMMIT_MSG  "-m \"${CHANGE_SUMMARY}\"")
	    ELSE(DEFINED RPM_RELEASE_SUMMARY)
		SET (COMMIT_MSG  "")
	    ENDIF(DEFINED RPM_RELEASE_SUMMARY)

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
		SET (KOJI_SCRATCH_BUILD_CMD
		    "koji build --scratch dist-${FEDORA_RAWHIDE_TAG} ${srpm}"
		    )

		SET (KOJI_SUBMISSION_CMD
		    "git checkout master\; "
		    "fedpkg import  ${srpm}"
		    )

		SET (KOJI_BUILD_CMD
		    "git checkout master\; "
		    "fedpkg build"
		    )
	    ENDIF(_rawhide EQUAL 1)

	    ## Make target commands for the released dist
	    FOREACH(_dist_tag ${_koji_dist_tags})
		STRING(REGEX MATCH "f([0-9]*)"
		    _is_fedora_tag ${_dist_tag})
		STRING(REGEX MATCH "el([0-9]*)"
		    _is_epel_tag ${_dist_tag})
		IF (_is_fedora_tag)
		    STRING(REGEX REPLACE "f([0-9]*)" "\\1"
			_curr_tag ${_dist_tag})
		    SET(_dist_target dist-${_dist_tag})
		    IF(NOT DEFINED BODHI_DIST_TAGS)
			SET(_bodhi_dist_tags ${_bodhi_dist_tags} "fc${_curr_tag}")
		    ENDIF(NOT DEFINED BODHI_DIST_TAGS)
		ELSEIF (_is_epel_tag)
		    STRING(REGEX REPLACE "el([0-9]*)" "\\1"
			_curr_tag ${_dist_tag})
		    SET(_dist_target dist-${_curr_tag}E-epel-testing-candidate )
		    IF(NOT DEFINED BODHI_DIST_TAGS)
			SET(_bodhi_dist_tags ${_bodhi_dist_tags} "el${_curr_tag}")
		    ENDIF(NOT DEFINED BODHI_DIST_TAGS)
		ENDIF(_is_fedora_tag)

		# Make target commands for the released dist
		IF (NOT "${KOJI_SCRATCH_BUILD_CMD}" STREQUAL "")
		    SET (KOJI_SCRATCH_BUILD_CMD "${KOJI_SCRATCH_BUILD_CMD}\; ")
		    SET (KOJI_SUBMISSION_CMD "${KOJI_SUBMISSION_CMD}\; ")
		    SET (KOJI_BUILD_CMD "${KOJI_BUILD_CMD}\; ")
		ENDIF (NOT "${KOJI_SCRATCH_BUILD_CMD}" STREQUAL "")

		SET (KOJI_SCRATCH_BUILD_CMD "${KOJI_SCRATCH_BUILD_CMD}"
		    "koji build --scratch ${_dist_target} ${srpm}")
		SET (KOJI_SUBMISSION_CMD "${KOJI_SUBMISSION_CMD}"
		    "fedpkg switch-branch ${_dist_tag}\; "
		    "fedpkg import -b ${_dist_tag} ${srpm}"
		    "fedpkg commit -p ${COMMIT_MSG}")
		SET (KOJI_BUILD_CMD
		    "${KOJI_BUILD_CMD}"
		    "fedpkg switch-branch ${_dist_tag}\; "
		    "fedpkg build")
	    ENDFOREACH(_dist_tag)

	    #MESSAGE(KOJI_SCRATCH_BUILD_CMD=${KOJI_SCRATCH_BUILD_CMD})
	    ADD_CUSTOM_TARGET(koji_scratch_build
		COMMAND eval ${KOJI_SCRATCH_BUILD_CMD}
		DEPENDS ${KOJI_DEPENDS} ${srpm}
		COMMENT "Start Koji scratch build"
		VERBATIM
		)

	    #MESSAGE(KOJI_SUBMISSION_CMD=${KOJI_SUBMISSION_CMD})
	    ADD_CUSTOM_TARGET(koji_submit
		COMMAND echo ${KOJI_SUBMISSION_CMD}
		DEPENDS ${KOJI_DEPENDS}  ${srpm} ${FEDPKG_DIR} ${FEDPKG_DIR}/${PROJECT_NAME}
		COMMENT "Submitting to Koji"
		WORKING_DIRECTORY ${FEDPKG_DIR}/${PROJECT_NAME}
		VERBATIM
		)

	    #MESSAGE("KOJI_BUILD_CMD=${KOJI_BUILD_CMD}")
	    ADD_CUSTOM_TARGET(koji_build
		COMMAND echo "${KOJI_BUILD_CMD}"
		COMMENT "Building on Koji"
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
    ENDMACRO(UseKoji srpm)

ENDIF(NOT DEFINED _USE_KOJI_FEDORA_CMAKE_)

