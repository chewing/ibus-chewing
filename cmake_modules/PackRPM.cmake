# - RPM generation, maintaining (remove old rpm) and verification (rpmlint).
# This module provides macros that provides various rpm building and
# verification targets.
# Reads following variables:
#   RPM_DIST_TAG: (optional) Current distribution tag such as el5, fc10.
#     Default: Distribution tag from rpm --showrc
#
#   RPM_BUILD_TOPDIR: (optional) Directory of  the rpm topdir.
#     Default: ${CMAKE_BINARY_DIR}
#
#   RPM_BUILD_SPECS: (optional) Directory of  spec files.
#     Default: ${RPM_BUILD_TOPDIR}/SPECS
#
#   RPM_BUILD_SOURCES: (optional) Directory of source (tar.gz or zip) files.
#     Default: ${RPM_BUILD_TOPDIR}/SOURCES
#
#   RPM_BUILD_SRPMS: (optional) Directory of source rpm files.
#     Default: ${RPM_BUILD_TOPDIR}/SRPMS
#
#   RPM_BUILD_RPMS: (optional) Directory of generated rpm files.
#     Default: ${RPM_BUILD_TOPDIR}/RPMS
#
#   RPM_BUILD_BUILD: (optional) Directory for RPM build.
#     Default: ${RPM_BUILD_TOPDIR}/BUILD
#
#   RPM_BUILD_BUILDROOT: (optional) Directory for RPM build.
#     Default: ${RPM_BUILD_TOPDIR}/BUILDROOT
#
# Defines following variables after include:
#   RPM_IGNORE_FILES: A list of exclude file patterns for PackSource.
#     Include it in PACK_SOURCE_IGNORE_FILES before calling PackSource()
#
# Defines following Macros:
#   PACK_RPM(var spec_in source0 )
#   - Generate spec and pack rpm  according to the spec_in file.
#     It calls PackSource, so no need to call it manually,
#     note that environment variables for PackSource should be defined
#     before calling this macro.
#     Arguments:
#     + var: The filename of srpm is outputted to this var.
#            Path is excluded.
#     + spec_in: RPM spec input template.
#     + source0: Source tarball for source0 in spec.
#                Note that its better to put the URL prefix in spec_in, instead of here.
#     Targets:
#     + srpm: Build srpm (rpmbuild -bs).
#     + rpm: Build rpm and srpm (rpmbuild -ba)
#     + rpmlint: Run rpmlint to generated rpms.
#     + rpm_remove_old: Remove old rpms.
#     + pkg_remove_old: Remove old source tarballs and rpms.
#     This macro defines following variables:
#     + PRJ_SRPM_PATH: Filename of generated SRPM file, including relative path.
#
#   USE_MOCK(spec_in)
#   - Add mock related targets.
#     Arguments:
#     + spec_in: RPM spec input template.
#     Targets:
#     + rpm_mock_i386: Make i386 rpm
#     + rpm_mock_x86_64: Make x86_64 rpm
#     This macor reads following variables?:
#     + MOCK_RPM_DIST_TAG: Prefix of mock configure file, such as "fedora-11", "fedora-rawhide", "epel-5".
#         Default: Convert from RPM_DIST_TAG
#

IF(NOT DEFINED _PACK_RPM_CMAKE_)
    SET (_PACK_RPM_CMAKE_ "DEFINED")

    INCLUDE(PackSource)
    SET (SPEC_FILE_WARNING "This file is generated, please modified the .spec.in file instead!")

    IF(NOT DEFINED RPM_DIST_TAG)
	EXECUTE_PROCESS(COMMAND rpm --showrc
	    COMMAND grep -E "dist[[:space:]]*\\."
	    COMMAND sed -e "s/^.*dist\\s*\\.//"
	    COMMAND tr \\n \\t
	    COMMAND sed  -e s/\\t//
	    OUTPUT_VARIABLE RPM_DIST_TAG)
    ENDIF(NOT DEFINED RPM_DIST_TAG)

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

    IF(NOT DEFINED RPM_BUILD_BUILDROOT)
	SET(RPM_BUILD_BUILDROOT "${RPM_BUILD_TOPDIR}/BUILDROOT")
    ENDIF(NOT DEFINED RPM_BUILD_BUILDROOT)

    # Add RPM build directories in ignore file list.
    GET_FILENAME_COMPONENT(_rpm_build_sources_basename ${RPM_BUILD_SOURCES} NAME)
    GET_FILENAME_COMPONENT(_rpm_build_srpms_basename ${RPM_BUILD_SRPMS} NAME)
    GET_FILENAME_COMPONENT(_rpm_build_rpms_basename ${RPM_BUILD_RPMS} NAME)
    GET_FILENAME_COMPONENT(_rpm_build_build_basename ${RPM_BUILD_BUILD} NAME)
    GET_FILENAME_COMPONENT(_rpm_build_buildroot_basename ${RPM_BUILD_BUILDROOT} NAME)
    SET(RPM_IGNORE_FILES
	"/${_rpm_build_sources_basename}/" "/${_rpm_build_srpms_basename}/" "/${_rpm_build_rpms_basename}/"
	"/${_rpm_build_build_basename}/" "/${_rpm_build_buildroot_basename}/" "debug.*s.list")

    MACRO(PACK_RPM var spec_in source0)
	IF(NOT EXISTS ${spec_in})
	    MESSAGE(FATAL_ERROR "File ${spec_in} not found!")
	ENDIF(NOT EXISTS ${spec_in})
	FIND_PROGRAM(RPMBUILD NAMES "rpmbuild-md5" "rpmbuild")
	IF(${RPMBUILD} STREQUAL "RPMBUILD-NOTFOUND")
	    MESSAGE(FATAL_ERROR "rpmbuild-md5 and rpmbuild are not found in PATH!")
	ENDIF(${RPMBUILD} STREQUAL "RPMBUILD-NOTFOUND")

	# Get release number from spec_in
	INCLUDE(ManageVariable)
	SETTING_FILE_GET_ATTRIBUTE(_releaseStr Release ${spec_in} ":")
	STRING(REPLACE "%{?dist}" ".${RPM_DIST_TAG}" PRJ_RELEASE ${_releaseStr})
	STRING(REPLACE "%{?dist}" "" PRJ_RELEASE_NUMBER ${_releaseStr})
	#MESSAGE("_releaseTag=${_releaseTag} _releaseStr=${_releaseStr}")

        # Update RPM_ChangeLog
	FILE(READ "${RPM_BUILD_SPECS}/RPM-ChangeLog.prev" RPM_CHANGELOG_PREV)
	CONFIGURE_FILE(${RPM_BUILD_SPECS}/RPM-ChangeLog.in ${RPM_BUILD_SPECS}/RPM-ChangeLog)

	# Generate spec
	CONFIGURE_FILE(${spec_in} ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec)
	SET_SOURCE_FILES_PROPERTIES(${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec
	    PROPERTIES GENERATED TRUE
	    )

	SET(${var} "${PROJECT_NAME}-${PRJ_VER}-${PRJ_RELEASE}.src.rpm")
	SET(_prj_srpm_path "${RPM_BUILD_SRPMS}/${${var}}")

	#-------------------------------------------------------------------
	# RPM build commands and targets

	ADD_CUSTOM_COMMAND(OUTPUT ${RPM_BUILD_SRPMS}
	    COMMAND ${CMAKE_COMMAND} -E make_directory ${RPM_BUILD_SRPMS}
	    )

	ADD_CUSTOM_COMMAND(OUTPUT ${RPM_BUILD_RPMS}
	    COMMAND ${CMAKE_COMMAND} -E make_directory ${RPM_BUILD_RPMS}
	    )

	ADD_CUSTOM_COMMAND(OUTPUT ${RPM_BUILD_BUILD}
	    COMMAND ${CMAKE_COMMAND} -E make_directory ${RPM_BUILD_BUILD}
	    )

	ADD_CUSTOM_COMMAND(OUTPUT ${RPM_BUILD_BUILDROOT}
	    COMMAND ${CMAKE_COMMAND} -E make_directory ${RPM_BUILD_BUILDROOT}
	    )

	ADD_CUSTOM_COMMAND(OUTPUT ${_prj_srpm_path}
	    COMMAND ${RPMBUILD} -bs ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec
	    --define '_sourcedir ${RPM_BUILD_SOURCES}'
	    --define '_builddir ${RPM_BUILD_BUILD}'
	    --define '_srcrpmdir ${RPM_BUILD_SRPMS}'
	    --define '_rpmdir ${RPM_BUILD_RPMS}'
	    --define '_specdir ${RPM_BUILD_SPECS}'
	    DEPENDS ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec ${RPM_BUILD_SOURCES}/${source0}
	    ${RPM_BUILD_SRPMS} ${RPM_BUILD_BUILD}
	    COMMENT "Building srpm"
	    )

	ADD_CUSTOM_TARGET(srpm
	    DEPENDS ${RPM_BUILD_SOURCES}/${source0}
	    )

	ADD_CUSTOM_TARGET(rpm
	    COMMAND ${CMAKE_COMMAND} -E make_directory ${RPM_BUILD_SRPMS}
	    COMMAND ${RPMBUILD} -ba --buildroot ${RPM_BUILD_BUILDROOT} ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec
	    --define '_sourcedir ${RPM_BUILD_SOURCES}'
	    --define '_builddir ${RPM_BUILD_BUILD}'
	    --define '_srcrpmdir ${RPM_BUILD_SRPMS}'
	    --define '_rpmdir ${RPM_BUILD_RPMS}'
	    --define '_specdir ${RPM_BUILD_SPECS}'
	    DEPENDS ${RPM_BUILD_SPECS}/${PROJECT_NAME}.spec ${RPM_BUILD_SOURCES}/${source0}
	    ${RPM_BUILD_SRPMS} ${RPM_BUILD_RPMS} ${RPM_BUILD_BUILD} ${RPM_BUILD_BUILDROOT}
	    )

	ADD_CUSTOM_TARGET(rpmlint find .
	    -name '${PROJECT_NAME}*-${PRJ_VER}-${PRJ_RELEASE}*.rpm'
	    -print -exec rpmlint '{}' '\\;'
	    )

	ADD_CUSTOM_TARGET(rpm_remove_old
	    COMMAND find .
	    -name '${PROJECT_NAME}*.rpm' ! -name '${PROJECT_NAME}*-${PRJ_VER}-${PRJ_RELEASE}.*.rpm'
	    -print -delete
	    COMMENT "Removing the old rpms.."
	    )

	ADD_CUSTOM_TARGET(pkg_remove_old
	)

	ADD_DEPENDENCIES(pkg_remove_old rpm_remove_old pack_remove_old)
    ENDMACRO(PACK_RPM var spec_in source0)

    MACRO(USE_MOCK ${srpm} ${spec_in})
	FIND_PROGRAM(MOCK NAME "mock")
	IF("${MOCK}" STREQUAL "MOCK-NOTFOUND")
	    MESSAGE(FATAL_ERROR "mock is not found in PATH!")
	ENDIF("${MOCK}" STREQUAL "MOCK-NOTFOUND")

	IF(NOT DEFINED MOCK_RPM_DIST_TAG)
	    STRING(REGEX MATCH "^fc([1-9][0-9]*)"  _fedora_mock_dist "${RPM_DIST_TAG}")
	    STRING(REGEX MATCH "^el([1-9][0-9]*)"  _el_mock_dist "${RPM_DIST_TAG}")

	    IF (_fedora_mock_dist)
		STRING(REGEX REPLACE "^fc([1-9][0-9]*)" "fedora-\\1" MOCK_RPM_DIST_TAG "${RPM_DIST_TAG}")
	    ELSEIF (_el_mock_dist)
		STRING(REGEX REPLACE "^el([1-9][0-9]*)" "epel-\\1" MOCK_RPM_DIST_TAG "${RPM_DIST_TAG}")
	    ELSE (_fedora_mock_dist)
		SET(MOCK_RPM_DIST_TAG "fedora-devel")
	    ENDIF(_fedora_mock_dist)
	    #MESSAGE ("MOCK_RPM_DIST_TAG=${MOCK_RPM_DIST_TAG}")
	ENDIF(NOT DEFINED MOCK_RPM_DIST_TAG)

	SETTING_FILE_GET_ATTRIBUTE(_archStr BuildArch ${spec_in} ":")
	IF("${_archStr}" STREQUAL noarch)
	    ADD_CUSTOM_TARGET(rpm_mock_i386
		COMMAND ${CMAKE_COMMAND} -E make_directory ${RPM_BUILD_RPMS}/i386
		COMMAND mock -r  "${MOCK_RPM_DIST_TAG}-i386" --resultdir="${RPM_BUILD_RPMS}/i386" ${RPM_BUILD_SRPMS}/${srpm}
		DEPENDS ${RPM_BUILD_SRPMS}/${srpm}
		)

	    ADD_CUSTOM_TARGET(rpm_mock_x86_64
		COMMAND ${CMAKE_COMMAND} -E make_directory ${RPM_BUILD_RPMS}/x86_64
		COMMAND mock -r  "${MOCK_RPM_DIST_TAG}-x86_64" --resultdir="${RPM_BUILD_RPMS}/x86_64" ${RPM_BUILD_SRPMS}/${srpm}
		DEPENDS ${RPM_BUILD_SRPMS}/${srpm}
		)

	ENDIF("${_archStr}" STREQUAL noarch)
    ENDMACRO(USE_MOCK)

ENDIF(NOT DEFINED _PACK_RPM_CMAKE_)

