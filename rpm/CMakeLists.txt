SET(RPM_BUILD_TOPDIR ${CMAKE_BINARY_DIR}/rpm CACHE FILEPATH "RPM top dir")
SET(RPM_BUILD_SOURCES ${CMAKE_BINARY_DIR} CACHE FILEPATH "RPM Source dir")
INCLUDE(ManageRPM)
INCLUDE(ManageReleaseFedora)

SET(RPM_SPEC_PRE_SECTION_OUTPUT
"%pre
if [ \"$1\" -gt 1 ] \; then
    export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
    [ -r %{_sysconfdir}/gconf/schemas/%{name}.schemas ] &&
    gconftool-2 --makefile-uninstall-rule %{_sysconfdir}/gconf/schemas/%{name}.schemas\\
    >/dev/null || :

    # Upgrading 1.0.2.20090302-1.fc11 or older?
    [ -r %{_sysconfdir}/gconf/schemas/%{name}.schema ] &&
    gconftool-2 --makefile-uninstall-rule %{_sysconfdir}/gconf/schemas/%{name}.schema\\
    >/dev/null || :
fi"
)

SET(RPM_SPEC_POST_SECTION_OUTPUT
"%post
export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
gconftool-2 --makefile-install-rule %{_sysconfdir}/gconf/schemas/%{name}.schemas > /dev/null || :"
)

SET(RPM_SPEC_PREUN_SECTION_OUTPUT
"%preun
if [ \"$1\" -eq 0 ] ; then
    export GCONF_CONFIG_SOURCE=`gconftool-2 --get-default-source`
    gconftool-2 --makefile-uninstall-rule %{_sysconfdir}/gconf/schemas/%{name}.schemas > /dev/null || :
fi"
)


