# Koji and Bodhi operation for Fedora developers.
#
# To use: INCLUDE(FedoraDeveloper), and DEVELOPER_SETTING file exists
# Include: RPM
#
#===================================================================
# Variables:
# KOJI_DEPENDS: Files that koji build targets depend on.
# KOJI_CVS_PATH: The path for KOJI build.
#         Default: ./
#
# RPM_RELEASE_SUMMARY: Summary of latest change, used in CVS message and Bodhi
#                 comment.
#
# CVS_DIST_TAGS: Distribution tags such as F-10, EL-5 to be committed to CVS
#                for koji or plague build. By default, devel is always built,
#                so no need to explicitly declare it.
#         Default: built devel.
#
# BODHI_DEPENDS: Files that bodhi targets depends on.
# BODHI_DIST_TAGS: Distribution tags such as fc9, fc10 to be committed to
#                  Bodhi.
#         Default: Derived from CVS_DIST_TAGS.
#
#===================================================================
# Targets:
# koji_build_scratch: koji scratch build.
# koji_submit: Submit to koji
# koji_build: Start koji build
# bodhi_new: Submit to bodhi
#

INCLUDE(RPM)
SET(RAWHIDE_VER 12)

IF(NOT DEFINED KOJI_CVS_PATH)
    SET(KOJI_CVS_PATH "./")
ELSEIF (KOJI_CVS_PATH STREQUAL "")
    SET(KOJI_CVS_PATH "./")
ENDIF(NOT DEFINED KOJI_CVS_PATH)

#====================================================================
# Koji (Fedora build system)
#

# Koji submit
SET (KOJI_SUBMISSION_CMD "")
IF (DEFINED RPM_RELEASE_SUMMARY)
    SET (KOJI_SUBMISSION_CMD
	"${KOJI_CVS_PATH}/${PROJECT_NAME}/common/cvs-import.sh -m \"${RPM_RELEASE_SUMMARY}\" ${SRPM_FILE}"
    )

    FOREACH(_dist_tag ${CVS_DIST_TAGS})
	SET (KOJI_SUBMISSION_CMD
	    "${KOJI_SUBMISSION_CMD}\;"
	    "${KOJI_CVS_PATH}/${PROJECT_NAME}/common/cvs-import.sh -b ${_dist_tag} -m \"${RPM_RELEASE_SUMMARY}\" ${SRPM_FILE}"
	)
    ENDFOREACH(_dist_tag)
ELSE(DEFINED RPM_RELEASE_SUMMARY)
    SET (KOJI_SUBMISSION_CMD
	"${KOJI_CVS_PATH}/${PROJECT_NAME}/common/cvs-import.sh  ${SRPM_FILE}"
    )
    FOREACH(_dist_tag ${CVS_DIST_TAGS})
	SET (KOJI_SUBMISSION_CMD
	    "${KOJI_SUBMISSION_CMD}\;"
	    "${KOJI_CVS_PATH}/${PROJECT_NAME}/common/cvs-import.sh -b ${_dist_tag}  ${SRPM_FILE}"
	)
    ENDFOREACH(_dist_tag)
ENDIF(DEFINED RPM_RELEASE_SUMMARY)

SET (KOJI_BUILD_CMD
    "cd ${KOJI_CVS_PATH}/${PROJECT_NAME} && cvs up && cd devel && make build && cd .."
    )

SET (KOJI_SCRATCH_BUILD_CMD
    "koji build --scratch dist-f${RAWHIDE_VER} ${SRPM_FILE}"
    )


FOREACH(_dist_tag ${CVS_DIST_TAGS})
    SET (KOJI_BUILD_CMD
	"${KOJI_BUILD_CMD} && cd ${_dist_tag} && make build && cd .."
	)
    STRING(REGEX REPLACE "F-([0-9]*)" "\\1"
	_curr_tag ${_dist_tag})
    IF (_curr_tag)
	SET (KOJI_SCRATCH_BUILD_CMD
	    "${KOJI_SCRATCH_BUILD_CMD} && koji build --scratch dist-f${_curr_tag} ${SRPM_FILE}"
	    )
	IF(NOT DEFINED BODHI_DIST_TAGS)
	    SET(_bodhi_dist_tags "fc${_curr_tag}")
	ENDIF(NOT DEFINED BODHI_DIST_TAGS)
    ENDIF(_curr_tag)
ENDFOREACH(_dist_tag)

#MESSAGE(KOJI_SCRATCH_BUILD_CMD=${KOJI_SCRATCH_BUILD_CMD})
ADD_CUSTOM_TARGET(koji_scratch_build
    COMMAND eval ${KOJI_SCRATCH_BUILD_CMD}
    DEPENDS ${KOJI_DEPENDS} ${SRPM_FILE} ${KOJI_CVS_PATH}
    COMMENT "Start Koji scratch build"
    VERBATIM
    )

#MESSAGE(KOJI_SUBMISSION_CMD=${KOJI_SUBMISSION_CMD})
ADD_CUSTOM_TARGET(koji_submit
    COMMAND eval ${KOJI_SUBMISSION_CMD}
    COMMAND cd ${KOJI_CVS_PATH}/${PROJECT_NAME} &&  cvs up
    DEPENDS ${KOJI_DEPENDS}  ${SRPM_FILE} ${KOJI_CVS_PATH}
    COMMENT "Submitting to Koji"
    VERBATIM
    )

#MESSAGE("KOJI_BUILD_CMD=${KOJI_BUILD_CMD}")
ADD_CUSTOM_TARGET(koji_build
    COMMAND eval "${KOJI_BUILD_CMD}"
    COMMENT "Building on Koji"
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
	SET (BODHI_NEW_CMD "${BODHI_NEW_CMD} && ")
    ELSE ()
	SET (BODHI_NEW_CMD "")
    ENDIF ()
    IF(DEFINED RPM_RELEASE_SUMMARY)
	SET (BODHI_NEW_CMD
	    "${BODHI_NEW_CMD}"
	    "bodhi --new --type=bugfix --comment=\"${RPM_RELEASE_SUMMARY}\" ${PROJECT_NAME}-${PRJ_VER_FULL}.${_bodhi_tag}"
	    )
    ELSE(DEFINED RPM_RELEASE_SUMMARY)
	SET (BODHI_NEW_CMD
	    "${BODHI_NEW_CMD}"
	    "bodhi --new --type=bugfix  ${PROJECT_NAME}-${PRJ_VER_FULL}.${_bodhi_tag}"
	    )
    ENDIF(DEFINED RPM_RELEASE_SUMMARY)
ENDFOREACH(_bodhi_tag)

#MESSAGE(BODHI_NEW_CMD=${BODHI_NEW_CMD})
IF(DEFINED BODHI_NEW_CMD)
    ADD_CUSTOM_TARGET(bodhi_new
	COMMAND eval "${BODHI_NEW_CMD}"
	DEPENDS ${BODHI_DEPENDS}
	COMMENT "Send the new package to bodhi"
	VERBATIM
	)
ENDIF(DEFINED BODHI_NEW_CMD)

ADD_DEPENDENCIES(koji_scratch_build srpm)
ADD_DEPENDENCIES(koji_submit srpm)
ADD_DEPENDENCIES(koji_build srpm)

