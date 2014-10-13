/**
 * Signal process subroutines
 *
 */

void ibus_chewing_engine_enable(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* enable(): statusFlags=%x",
		     self->_priv->statusFlags);
    ibus_chewing_engine_use_setting(self);
    ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_ENABLED);
    self->inputMode = CHEWING_INPUT_MODE_SELECTION_DONE;
}

void ibus_chewing_engine_disable(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* disable(): statusFlags=%x",
		     self->_priv->statusFlags);
    ibus_chewing_engine_clear_status_flag(self, ENGINE_FLAG_ENABLED);
    self->inputMode = CHEWING_INPUT_MODE_BYPASS;
}

void ibus_chewing_engine_focus_in(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* focus_in(): statusFlags=%x",
		     self->_priv->statusFlags);
    if (!self->iProperties) {
	ibus_chewing_engine_use_setting(self);
    }
    ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_FOCUS_IN);
    ibus_chewing_engine_restore_mode(self);
    ibus_chewing_engine_refresh_property_list(self);

    /* Shouldn't have anything to commit when Focus-in */
    ibus_chewing_engine_clean_chewing_buffer(self);
    ibus_chewing_engine_clear_status_flag(self,
	    ENGINE_FLAG_FORCE_COMMIT |
	    ENGINE_FLAG_NEED_COMMIT);

    //      self_update(self);
    IBUS_CHEWING_LOG(INFO, "focus_in(): return");
}

void ibus_chewing_engine_focus_out(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* focus_out(): statusFlags=%x",
		     self->_priv->statusFlags);
    ibus_chewing_engine_clear_status_flag(self,
					  ENGINE_FLAG_FOCUS_IN |
					  ENGINE_FLAG_PROPERTIES_REGISTERED);
    ibus_chewing_engine_hide_property_list(self);

    /*
     * force commit is no longer needed,
     * as ibus_engine_update_preedit_text_with_mode now send to focus out with
     * IBUS_ENGINE_PREEDIT_COMMIT
     */
    //      self_force_commit(self);
    IBUS_CHEWING_LOG(DEBUG, "focus_out(): return");

}

