/*=================================================
 * GUI components
 */

void ibus_chewing_engine_systray_icon_create(IBusChewingEngine * self)
{
    if (ibus_chewing_pre_edit_get_property_boolean
	(self->icPreEdit, "show-systray")) {
	if (self->iChiEngSystrayIcon == NULL) {
	    self->iChiEngSystrayIcon =
		ibus_chewing_systray_chi_eng_icon_new(self->icPreEdit);
	}
	ibus_chewing_systray_icon_set_visible(self->iChiEngSystrayIcon,
					      TRUE);
    }
}

void ibus_chewing_engine_apply_show_systray_icon(IBusChewingEngine * self)
{
    if (ibus_chewing_pre_edit_get_property_boolean
	(self->icPreEdit, "show-systray")) {
	ibus_chewing_engine_systray_icon_create(self);
    } else {
	ibus_chewing_systray_icon_free(self->iChiEngSystrayIcon);
    }

}

void ibus_chewing_engine_refresh_systray_icon(IBusChewingEngine * self)
{
    if (ibus_chewing_pre_edit_get_property_boolean
	(self->icPreEdit, "show-systray")) {
	ibus_chewing_systray_chi_eng_refresh_value(self->
						   iChiEngSystrayIcon,
						   self->icPreEdit);
    }
}
