# RPM generation, maintaining (remove old rpm) and verification (rpmlint).
#
# Include: INCLUDE(RPM)
# 
#===================================================================
# Variables: 
# DIST_TAG: Distribution tag such as el5, fc10.
#         Default: Distribution tag from rpm --showrc
#
# RPM_BUILD_TOPDIR: Directory of  the rpm topdir.
#         Default: ${CMAKE_BINARY_DIR}
# RPM_BUILD_SPECS: Directory of  spec files.
#         Default: ${RPM_BUILD_TOPDIR}/SPECS
# RPM_BUILD_SOURCES: Directory of source (tar.gz or zip) files.
#         Default: ${RPM_BUILD_TOPDIR}/SOURCES
# RPM_BUILD_SRPMS: Directory of source rpm files.
#         Default: ${RPM_BUILD_TOPDIR}/SRPMS
# RPM_BUILD_RPMS: Directory of generated rpm files.
#         Default: ${RPM_BUILD_TOPDIR}/RPMS
# RPM_BUILD_BUILD:  Directory for RPM build.
#         Default: ${RPM_BUILD_TOPDIR}/BUILD
#
# GENERATE_SPEC: Set this to the spec.in file.
#
# RPM_SOURCE_FILES: Source and patch file for RPM build.
#         Default: ${RPM_BUILD_SOURCES}/${PROJECT_NAME}-${PRJ_VER}-Source 
#                   with suffix of either tar.gz, tar.bz2, tgz, tbz, zip
# SRPM_FILE: Generated srpm file.
#         Default: ${RPM_BUILD_SRPMS}/${PROJECT_NAME}-${PRJ_VER_FULL}.${DIST_TAG}.src.rpm
# RPM_IS_NO_ARCH: Set it if this rpm is noarch, it also
#         hide rpm_mock_i386 and rpm_mock_x86_64 for noarch package
#
#===================================================================
# 
# 
#

SET (SPEC_FILE_WARNING "This file is generated, please modified the .spec.in file instead!")

IF(NOT DEFINED DIST_TAG)
    EXECUTE_PROCESS(COMMAND rpm --showrc
	COMMAND grep -E "dist[[:space:]]*\\."
	COMMAND sed -e "s/^.*dist\\s*\\.//"
	COMMAND tr \\n \\t
	COMMAND sed  -e s/\\t//
	OUTPUT_VARIABLE DIST_TAG)
ENDIF(NOT DEFINED DIST_TAG)

IF(NOT DEFINED RPM_BUILD_TOPDIR)
    SET(RPM_BUILD_TOPDIR ${CMAKE_BINARY_DIR})
ENDIF(NOT DEFINED RPM_BUILD_TOPDIR)

IF(NOT DEFINED RPM_BUILD_SPECS)
    SET(RPM_BUILD_SPECS "${RPM_BUILD_TOPDIR}/SPECS")
ENDIF(NOT DEFINED RPM_BUILD_SPECS)

IF(NOT DEFINED RPM_BUILD_SOURCES)
    SET(RPM_BUILD_SOURCES "${RPM_BUILD_TOPDIR}/SOURCES")
ENDIF(NOT DEFINED RPM_BUILD_SOURCES)

IF(NOT DEFINED RPM_BUILD_SRPMS)
    SET(RPM_BUILD_SRPMS "${RPM_BUILD_TOPDIR}/SRPMS")
ENDIF(NOT DEFINED RPM_BUILD_SRPMS)

IF(NOT DEFINED RPM_BUILD_RPMS)
    SET(RPM_BUILD_RPMS "${RPM_BUILD_TOPDIR}/RPMS")
ENDIF(NOT DEFINED RPM_BUILD_RPMS)

IF(NOT DEFINED RPM_BUILD_BUILD)
    SET(RPM_BUILD_BUILD "${RPM_BUILD_TOPDIR}/BUILD")
ENDIF(NOT DEFINED RPM_BUILD_BUILD)


IF (DEFINED GENERATE_SPEC)
    CONFIGURE_FILE(${GENERATE_SPEC} ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec)

    SET_SOURCE_FILES_PROPERTIES(${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec
	PROPERTIES GENERATED TRUE
	)
ENDIF(DEFINED GENERATE_SPEC)

IF(NOT DEFINED RPM_SOURCE_FILES)
    SET(RPM_SOURCE_FILES)
    SET(RPM_SOURCE_FILES_PREFIX "${RPM_BUILD_SOURCES}/${PROJECT_NAME}-${PRJ_VER}-Source")
    IF(CPACK_SOURCE_GENERATOR STREQUAL "TGZ")
	SET(RPM_SOURCE_FILES ${RPM_SOURCE_FILES_PREFIX}.tar.gz)
    ELSEIF(CPACK_SOURCE_GENERATOR STREQUAL "TBZ2")
	SET(RPM_SOURCE_FILES ${RPM_SOURCE_FILES_PREFIX}.tar.bz2)
    ELSEIF(CPACK_SOURCE_GENERATOR STREQUAL "TZ")
	SET(RPM_SOURCE_FILES ${RPM_SOURCE_FILES_PREFIX}.tar.Z)
    ELSEIF(CPACK_SOURCE_GENERATOR STREQUAL "ZIP")
	SET(RPM_SOURCE_FILES ${RPM_SOURCE_FILES_PREFIX}.zip)
    ENDIF(CPACK_SOURCE_GENERATOR STREQUAL "TGZ")
    #MESSAGE("RPM_SOURCE_FILES=${RPM_SOURCE_FILES}")
ENDIF(NOT DEFINED RPM_SOURCE_FILES)

IF(NOT DEFINED SRPM_FILE)
    SET (SRPM_FILE ${RPM_BUILD_SRPMS}/${PROJECT_NAME}-${PRJ_VER_FULL}.${DIST_TAG}.src.rpm)
ENDIF(NOT DEFINED SRPM_FILE)

GET_FILENAME_COMPONENT(rpm_build_sources_basename ${RPM_BUILD_SOURCES} NAME)
GET_FILENAME_COMPONENT(rpm_build_srpms_basename ${RPM_BUILD_SRPMS} NAME)
GET_FILENAME_COMPONENT(rpm_build_rpms_basename ${RPM_BUILD_RPMS} NAME)
GET_FILENAME_COMPONENT(rpm_build_build_basename ${RPM_BUILD_BUILD} NAME)
SET(RPM_IGNORE_FILES "\\\\.rpm$"
    "/${rpm_build_sources_basename}/" "/${rpm_build_srpms_basename}/" "/${rpm_build_rpms_basename}/" "/${rpm_build_build_basename}/")
#MESSAGE("RPM_IGNORE_FILES=${RPM_IGNORE_FILES}")
SET(SOURCE_TARBALL ${PROJECT_NAME}-${PRJ_VER}-Source.tar.gz)

#-------------------------------------------------------------------
# Move source tarball
#
# Target pack_src is here because it is for and basically only for rpm
#
GET_FILENAME_COMPONENT(_rpm_build_source ${RPM_BUILD_SOURCES} ABSOLUTE)
IF (${_rpm_build_source} STREQUAL ${CMAKE_BINARY_DIR})
    ADD_CUSTOM_TARGET(pack_src}
	COMMAND make package_source
	COMMENT "Packaging Source files"
	VERBATIM
	)
ELSE(${_rpm_build_source} STREQUAL ${CMAKE_BINARY_DIR})
    ADD_CUSTOM_TARGET(pack_src
	COMMAND make package_source
	COMMENT "Packaging Source files"
	COMMAND mkdir -p ${RPM_BUILD_SOURCES}
	COMMAND mv ${SOURCE_TARBALL} ${RPM_BUILD_SOURCES}/${SOURCE_TARBALL}
	VERBATIM
	)
ENDIF(${_rpm_build_source} STREQUAL ${CMAKE_BINARY_DIR})


#-------------------------------------------------------------------
# RPM build targets
ADD_CUSTOM_TARGET(srpm
    COMMAND mkdir -p SRPMS
    COMMAND rpmbuild -bs ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec 
    --define '_sourcedir ${RPM_BUILD_SOURCES}'
    --define '_builddir ${RPM_BUILD_BUILD}' 
    --define '_srcrpmdir ${RPM_BUILD_SRPMS}'
    --define '_rpmdir ${RPM_BUILD_RPMS}'
    --define '_specdir ${RPM_BUILD_SPECS}' 
    DEPENDS ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec ${RPM_SOURCE_FILES}
    )

ADD_CUSTOM_TARGET(rpm
    COMMAND mkdir -p ${RPM_BUILD_SRPMS}
    COMMAND mkdir -p ${RPM_BUILD_RPMS}/i386
    COMMAND mkdir -p ${RPM_BUILD_RPMS}/x86_64
    COMMAND mkdir -p ${RPM_BUILD_RPMS}/noarch
    COMMAND mkdir -p BUILD
    COMMAND rpmbuild -ba ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec
    --define '_sourcedir ${RPM_BUILD_SOURCES}'
    --define '_builddir ${RPM_BUILD_BUILD}'
    --define '_srcrpmdir ${RPM_BUILD_SRPMS}'
    --define '_rpmdir ${RPM_BUILD_RPMS}'
    --define '_specdir ${RPM_BUILD_SPECS}'
    DEPENDS ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec ${RPM_SOURCE_FILES}
    )

ADD_DEPENDENCIES(srpm pack_src)
ADD_DEPENDENCIES(rpm pack_src)

IF(NOT RPM_IS_NOARCH)
    ADD_CUSTOM_TARGET(rpm_mock_i386 
	COMMAND mkdir -p RPMS/i386
	COMMAND mock -r  fedora-10-i386 --resultdir="${RPM_BUILD_RPMS}/i386" ${SRPM_FILE}
	)

    ADD_CUSTOM_TARGET(rpm_mock_x86_64
	COMMAND mkdir -p RPMS/x86_64
	COMMAND mock -r  fedora-10-x86_64 --resultdir="${RPM_BUILD_RPMS}/x86_64" ${SRPM_FILE}
	)

    ADD_DEPENDENCIES(rpm_mock_i386 srpm)
    ADD_DEPENDENCIES(rpm_mock_x86_64 srpm)
ENDIF(NOT RPM_IS_NOARCH)

ADD_CUSTOM_TARGET(rpmlint find . 
    -name '${PROJECT_NAME}*-${PRJ_VER_FULL}.*.rpm' 
    -print -exec rpmlint '{}' '\\;'
    )


ADD_CUSTOM_TARGET(rpm_remove_old
    COMMAND find . 
    -name '${PROJECT_NAME}*.rpm' ! -name '${PROJECT_NAME}*-${PRJ_VER_FULL}.*.rpm'
    -print -delete
    COMMENT "Removing the old rpms.."
    )


ADD_CUSTOM_TARGET(pkg_remove_old 
    COMMAND find . 
    -name '${PROJECT_NAME}*.tar.[bg]z*' ! -name '${PROJECT_NAME}-${PRJ_VER}-*.tar.[bg]z*'
    -print -delete
    COMMENT "Removing the old tarballs .."
    )
ADD_DEPENDENCIES(pkg_remove_old rpm_remove_old)

