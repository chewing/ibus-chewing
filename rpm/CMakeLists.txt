SET(RPM_BUILD_TOPDIR ${CMAKE_BINARY_DIR}/rpm CACHE FILEPATH "RPM top dir")
SET(RPM_BUILD_SOURCES ${CMAKE_BINARY_DIR} CACHE FILEPATH "RPM Source dir")
INCLUDE(ManageRPM)
INCLUDE(ManageReleaseFedora)

RPM_SPEC_STRING_ADD_TAG(RPM_SPEC_REQUIRES_OUTPUT
    Requires pre "GConf2"
    )

RPM_SPEC_STRING_ADD_TAG(RPM_SPEC_REQUIRES_OUTPUT
    Requires post "GConf2"
    )

RPM_SPEC_STRING_ADD_TAG(RPM_SPEC_REQUIRES_OUTPUT
    Requires preun "GConf2"
    )

RPM_SPEC_STRING_ADD_DIRECTIVE(RPM_SPEC_SCRIPT_OUTPUT pre ""
"if [ \"$1\" -gt 1 ] \; then
    export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
    [ -r %{_sysconfdir}/gconf/schemas/%{name}.schemas ] &&
    gconftool-2 --makefile-uninstall-rule %{_sysconfdir}/gconf/schemas/%{name}.schemas\\
    >/dev/null || :

    # Upgrading 1.0.2.20090302-1.fc11 or older?
    [ -r %{_sysconfdir}/gconf/schemas/%{name}.schema ] &&
    gconftool-2 --makefile-uninstall-rule %{_sysconfdir}/gconf/schemas/%{name}.schema\\
    >/dev/null || :
fi
"
)

RPM_SPEC_STRING_ADD_DIRECTIVE(RPM_SPEC_SCRIPT_OUTPUT preun ""
"if [ \"$1\" -eq 0 ] ; then
    export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
    gconftool-2 --makefile-uninstall-rule %{_sysconfdir}/gconf/schemas/%{name}.schemas > /dev/null || :
fi
"
    )

RPM_SPEC_STRING_ADD_DIRECTIVE(RPM_SPEC_SCRIPT_OUTPUT post ""
"export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
gconftool-2 --makefile-install-rule %{_sysconfdir}/gconf/schemas/%{name}.schemas > /dev/null || :
[ -x %{_bindir}/ibus ] && \\
  %{_bindir}/ibus write-cache --system &>/dev/null || :
"
)

RPM_SPEC_STRING_ADD_DIRECTIVE(RPM_SPEC_SCRIPT_OUTPUT postun ""
"[ -x %{_bindir}/ibus ] && \\
  %{_bindir}/ibus write-cache --system &>/dev/null || :
"
)

ADD_CUSTOM_COMMAND(OUTPUT ${SOURCE_ARCHIVE_FILE}
    COMMAND make pack_src
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

PACK_RPM()
RELEASE_FEDORA(fedora)
# "Off" Warning is displayed if mock is not installed.
RPM_MOCK_BUILD()
MANAGE_RELEASE(release_fedora)


