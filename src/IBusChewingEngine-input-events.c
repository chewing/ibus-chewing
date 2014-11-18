#include "IBusChewingProperties.h"

gboolean ibus_chewing_engine_process_key_event(IBusEngine * engine,
					       KSym keySym, guint keycode,
					       KeyModifiers unmaskedMod)
{
    IBUS_CHEWING_LOG(INFO, "******** process_key_event(-,%x(%s),%x,%x) %s",
		     keySym, key_sym_get_name(keySym), keycode,
		     unmaskedMod, modifiers_to_string(unmaskedMod));

    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);
    if (is_password(self))
	return FALSE;

    KSym kSym =
	ibus_chewing_pre_edit_key_code_to_key_sym(self->icPreEdit, keySym,
						  keycode,
						  unmaskedMod);

    gboolean result =
	ibus_chewing_pre_edit_process_key(self->icPreEdit, kSym,
					  unmaskedMod);

    IBUS_CHEWING_LOG(MSG,
	    "process_key_event() result=%d",
	    result);
    self_update(self);
    
    return result;
}

/*===================================================
 * Mouse events
 */
void ibus_chewing_engine_candidate_clicked(IBusEngine * engine,
					   guint index, guint button,
					   guint state)
{
    IBUS_CHEWING_LOG(INFO,
		     "*** candidate_clicked(-, %x, %x, %x) ... proceed.",
		     index, button, state);
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);
    if (is_password(self))
	return;
    if (index >= chewing_get_candPerPage(self->icPreEdit->context)
	|| index < 0) {
	IBUS_CHEWING_LOG(DEBUG, "candidate_clicked() index out of ranged");
	return;
    }
    if (ibus_chewing_pre_edit_has_flag
	(self->icPreEdit, FLAG_TABLE_SHOW)) {
	gint *selKeys = chewing_get_selKey(self->icPreEdit->context);
	KSym k = (KSym) selKeys[index];
	ibus_chewing_pre_edit_process_key(self->icPreEdit, k, 0);
	g_free(selKeys);
	self_update(self);
    } else {
	IBUS_CHEWING_LOG(DEBUG,
			 "candidate_clicked() ... candidates are not showing");
    }
}

void ibus_chewing_engine_property_activate(IBusEngine * engine,
					   const gchar * prop_name,
					   guint prop_state)
{
    IBUS_CHEWING_LOG(INFO, "property_activate(-, %s, %u)", prop_name,
		     prop_state);
    Self *self = SELF(engine);
    gboolean needRefresh = TRUE;
    if (STRING_EQUALS(prop_name, "chewing_chieng_prop")) {
	/* Toggle Chinese <-> English */
	chewing_set_ChiEngMode(self->icPreEdit->context,
			       (is_chinese(self)) ? 0 : CHINESE_MODE);
	IBUS_CHEWING_LOG(INFO,
			 "property_activate chinese=%d caps_lock=%d",
			 is_chinese(self), is_caps_lock(self));

	if (ibus_chewing_pre_edit_has_flag
	    (self->icPreEdit, FLAG_SYNC_FROM_IM)
	    || ibus_chewing_pre_edit_has_flag(self->icPreEdit,
					      FLAG_SYNC_FROM_KEYBOARD)) {
	    if (is_chinese(self) == is_caps_lock(self)) {
		set_caps_led(!is_chinese(self), self->_priv->pDisplay);
	    }
	}
    } else if (STRING_EQUALS(prop_name, "chewing_alnumSize_prop")) {
	/* Toggle Full <-> Half */
	chewing_set_ShapeMode(self->icPreEdit->context,
			      !chewing_get_ShapeMode(self->
						     icPreEdit->context));
    } else if (STRING_EQUALS(prop_name, "chewing_settings_prop")) {
	if (ibus_chewing_property_get_state(self->settings_prop) ==
	    PROP_STATE_UNCHECKED) {
	    if (!self->sDialog) {
		MakerDialog *mDialog = maker_dialog_new_full
		    (self->icPreEdit->iProperties->properties,
		     _("Setting"),
		     MKDG_WIDGET_FLAG_APPLY_IMMEDIATELY,
		     MKDG_BUTTON_FLAG_OK | MKDG_BUTTON_FLAG_CANCEL);
		self->sDialog = GTK_WIDGET(mDialog);
	    }
	    gtk_widget_show_all(self->sDialog);
	    if (gtk_dialog_run(GTK_DIALOG(self->sDialog))
		== MKDG_BUTTON_RESPONSE_OK) {
		maker_dialog_assign_all_widgets_values(MAKER_DIALOG
						       (self->sDialog),
						       NULL);
	    }
	    gtk_widget_hide(self->sDialog);
#if IBUS_CHECK_VERSION(1, 4, 0)
	    ibus_property_set_state(self->settings_prop,
				    PROP_STATE_UNCHECKED);
#else
	    self->settings_prop->state = PROP_STATE_UNCHECKED;
#endif
	}
    } else {
	IBUS_CHEWING_LOG(DEBUG,
			 "property_activate(-, %s, %u) not recognized",
			 prop_name, prop_state);
	needRefresh = FALSE;
    }
    if (needRefresh)
	self_refresh_property(self, prop_name);
}


