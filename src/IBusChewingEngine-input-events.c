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

    gboolean result = ibus_chewing_pre_edit_process_key(self->icPreEdit, kSym,
                                                        unmaskedMod);

    IBUS_CHEWING_LOG(MSG, "process_key_event() result=%d", result);
    self_update(self);

    if (kSym == IBUS_KEY_Shift_L || kSym == IBUS_KEY_Shift_R || 
        kSym == IBUS_KEY_Caps_Lock) {
        /* Refresh property list (language bar) only when users toggle
         * Chi-Eng Mode or Shape Mode with Shift or Caps Lock, otherwise
         * the bar will stick to the cursor and block the candidats list.
         */
        self_refresh_property_list(self);
    }

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
    if (index >= chewing_get_candPerPage(self->icPreEdit->context)) {
        IBUS_CHEWING_LOG(DEBUG, "candidate_clicked() index out of ranged");
        return;
    }
    if (ibus_chewing_pre_edit_has_flag(self->icPreEdit, FLAG_TABLE_SHOW)) {
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

    if (STRING_EQUALS(prop_name, "InputMode")) {
        /* Toggle Chinese <-> English */
        ibus_chewing_pre_edit_toggle_chi_eng_mode(self->icPreEdit);
        IBUS_CHEWING_LOG(INFO,
                         "property_activate chinese=%d caps_lock=%d",
                         ibus_chewing_engine_is_chinese_mode(self),
                         is_caps_lock(self));

        if (ibus_chewing_pre_edit_has_flag(self->icPreEdit, FLAG_SYNC_FROM_IM)
            || ibus_chewing_pre_edit_has_flag(self->icPreEdit,
                                              FLAG_SYNC_FROM_KEYBOARD)) {
            if (ibus_chewing_engine_is_chinese_mode(self) == is_caps_lock(self)) {
                set_caps_led(!ibus_chewing_engine_is_chinese_mode(self),
                             self->_priv->pDisplay);
            }
        }
        self_refresh_property(self, prop_name);
    } else if (STRING_EQUALS(prop_name, "AlnumSize")) {
        /* Toggle Full <-> Half */
        chewing_set_ShapeMode(self->icPreEdit->context,
                              !chewing_get_ShapeMode(self->icPreEdit->context));
        self_refresh_property(self, prop_name);
    } else if (STRING_EQUALS(prop_name, "setup_prop")) {
        /* open preferences window */
        system(QUOTE_ME(LIBEXEC_DIR) "/ibus-setup-chewing");
    } else {
        IBUS_CHEWING_LOG(DEBUG,
                         "property_activate(-, %s, %u) not recognized",
                         prop_name, prop_state);
    }

}
