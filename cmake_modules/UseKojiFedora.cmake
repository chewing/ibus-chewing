# - Add Koji and Bodhi targets for Fedora and EPEL developers.
# This module provides convenient targets for scratch build, submit,
# and build on koji, using the GIT infrastructure.
# This modules need the variables from PackRPM, so remember to define
# PackRPM in front of this module.
#
# Read following variables
# This moudles
#
# To use: INCLUDE(FedoraDeveloper), and DEVELOPER_SETTING file exists
# Include: RPM
#
#===================================================================
# Variables:
# KOJI_DEPENDS: Files that koji build targets depend on.
# FEDPKG_DIR: The path for KOJI build.
#         Default: ./
#
# RPM_RELEASE_SUMMARY: Summary of latest change, used in CVS message and Bodhi
#                 comment.
#
# FEDORA_DIST_TAGS: Distribution tags such as f13, el5 to be committed to GIT
#                for koji.
#         By default, devel is always built, so no need to explicitly declare it.
#         Default: built devel and current two recent fedora release.
#
# BODHI_DEPENDS: Files that bodhi targets depends on.
# BODHI_DIST_TAGS: Distribution tags such as fc9, fc10 to be committed to
#                  Bodhi.
#         Default: Derived from FEDORA_DIST_TAGS.
#
#===================================================================
# Targets:
# koji_build_scratch: koji scratch build.
# koji_submit: Submit to koji
# koji_build: Start koji build
# bodhi_new: Submit to bodhi
#

IF(NOT DEFINED _USE_KOJI_FEDORA_CMAKE_)
    SET(_USE_KOJI_RPM_ "DEFINED")
    IF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)
	IF(NOT DEFINED FEDORA_RELEASE_TAGS)
	ENDIF(NOT DEFINED FEDORA_RELEASE_TAGS )
	    SET(FEDORA_RAWHIDE_TAG f15)
	IF(NOT DEFINED FEDORA_RELEASE_TAGS)
	    SET(FEDORA_RELEASE_TAGS f12 f13 f14)
	ENDIF(NOT DEFINED FEDORA_RELEASE_TAGS )


	#====================================================================
	# Koji (Fedora build system)
	#

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

	## For rawhide
	SET (KOJI_SCRATCH_BUILD_CMD
	    "koji build --scratch dist-${FEDORA_RAWHIDE_VER} ${PRj_SRPM_FILE_NAME}"
	    )

	SET (KOJI_SUBMISSION_CMD
	    "git checkout master"
	    "\; fedpkg import  ${PRj_SRPM_FILE_NAME}"
	    )

	#commit
	IF (DEFINED RPM_RELEASE_SUMMARY)
	    SET (COMMIT_MSG  "-m \"${RPM_RELEASE_SUMMARY}\"")
	ELSE(DEFINED RPM_RELEASE_SUMMARY)
	    SET (COMMIT_MSG  "")
	ENDIF(DEFINED RPM_RELEASE_SUMMARY)

	#submit
	SET (KOJI_SUBMISSION_CMD
	    "git checkout master"
	    "\; fedpkg commit -p ${COMMIT_MSG}"
	    )
	#build
	SET (KOJI_BUILD_CMD
	    "git checkout master"
	    "\; fedpkg build"
	    )

	## Make target commands for the released dist
	FOREACH(_dist_tag ${FEDORA_RELEASE_TAGS})
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
	    SET (KOJI_SCRATCH_BUILD_CMD
		"${KOJI_SCRATCH_BUILD_CMD}"
		"\; koji build --scratch ${_dist_target} ${PRj_SRPM_FILE_NAME}"
		)

	    SET (KOJI_SUBMISSION_CMD
		"${KOJI_SUBMISSION_CMD}"
		"\; fedpkg switch-branch ${_dist_tag}"
		"\; fedpkg commit -p ${COMMIT_MSG}"
		)

	    SET (KOJI_BUILD_CMD
		"${KOJI_BUILD_CMD}"
		"\; fedpkg switch-branch ${_dist_tag}"
		"\; fedpkg build"
		)
	ENDFOREACH(_dist_tag)


	#MESSAGE(KOJI_SCRATCH_BUILD_CMD=${KOJI_SCRATCH_BUILD_CMD})
	ADD_CUSTOM_TARGET(koji_scratch_build
	    COMMAND eval ${KOJI_SCRATCH_BUILD_CMD}
	    DEPENDS ${KOJI_DEPENDS} ${PRj_SRPM_FILE_NAME}
	    COMMENT "Start Koji scratch build"
	    VERBATIM
	    )

	#MESSAGE(KOJI_SUBMISSION_CMD=${KOJI_SUBMISSION_CMD})
	ADD_CUSTOM_TARGET(koji_submit
	    COMMAND echo ${KOJI_SUBMISSION_CMD}
	    DEPENDS ${KOJI_DEPENDS}  ${PRj_SRPM_FILE_NAME} ${FEDPKG_DIR} ${FEDPKG_DIR}/${PROJECT_NAME}
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

	ADD_DEPENDENCIES(koji_scratch_build srpm)
	ADD_DEPENDENCIES(koji_submit srpm)
	ADD_DEPENDENCIES(koji_build srpm)

    ENDIF(EXISTS $ENV{HOME}/.fedora-upload-ca.cert)

ENDIF(NOT DEFINED _USE_KOJI_FEDORA_CMAKE_)

