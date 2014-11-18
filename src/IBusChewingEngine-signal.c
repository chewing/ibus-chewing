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
}

void ibus_chewing_engine_disable(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* disable(): statusFlags=%x",
		     self->_priv->statusFlags);
    ibus_chewing_engine_clear_status_flag(self, ENGINE_FLAG_ENABLED);
}

void ibus_chewing_engine_focus_in(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* focus_in(): statusFlags=%x",
		     self->_priv->statusFlags);
    ibus_chewing_engine_use_setting(self);
    ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_FOCUS_IN);
    ibus_chewing_engine_restore_mode(self);
    ibus_chewing_engine_refresh_property_list(self);
    /* Shouldn't have anything to commit when Focus-in */
    ibus_chewing_pre_edit_clear(self->icPreEdit);
    refresh_pre_edit_text(self);
    refresh_aux_text(self);
#ifdef UNIT_TEST
    refresh_outgoing_text(self);
#else
    commit_text(self);
#endif
    IBUS_CHEWING_LOG(INFO, "focus_in() statusFlags=%x: return", self->_priv->statusFlags);
}

void ibus_chewing_engine_focus_out(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* focus_out(): statusFlags=%x",
		     self->_priv->statusFlags);
    ibus_chewing_engine_clear_status_flag(self,
					  ENGINE_FLAG_FOCUS_IN |
					  ENGINE_FLAG_PROPERTIES_REGISTERED);
    ibus_chewing_engine_hide_property_list(self);

    IBUS_CHEWING_LOG(DEBUG, "focus_out(): return");
}

#if IBUS_CHECK_VERSION(1, 5, 4)
void ibus_chewing_engine_set_content_type(IBusEngine * engine,
	guint purpose, guint hints)
{
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_set_content_type(%d, %d)",
	    purpose, hints);

    Self *self = SELF(engine);
    if (purpose == IBUS_INPUT_PURPOSE_PASSWORD ||
	    purpose == IBUS_INPUT_PURPOSE_PIN) {
	ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_IS_PASSWORD);
    } else {
	ibus_chewing_engine_clear_status_flag(self,
		ENGINE_FLAG_IS_PASSWORD);
    }
}
#endif

/**
 * Display text update subroutines
 *
 */

void parent_commit_text(IBusEngine * iEngine)
{
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(iEngine);
    IBUS_CHEWING_LOG(MSG, "* parent_commit_text(-): outgoingText=%s",
		     self->outgoingText->text);

#ifdef UNIT_TEST
    printf("* parent_commit_text(-, %s)\n", self->outgoingText->text);
#else
    ibus_engine_commit_text(iEngine, self->outgoingText);
#endif
}

void parent_update_preedit_text
    (IBusEngine * iEngine,
     IBusText * iText, guint cursor_pos, gboolean visible) {
#ifdef UNIT_TEST
    printf("* parent_update_preedit_text(-, %s, %u, %x)\n",
	   iText->text, cursor_pos, visible);
#else
    ibus_engine_update_preedit_text(iEngine, iText, cursor_pos, visible);
#endif
}

void parent_update_preedit_text_with_mode
    (IBusEngine * iEngine,
     IBusText * iText,
     guint cursor_pos, gboolean visible, IBusPreeditFocusMode mode) {
#ifdef UNIT_TEST
    printf
	("* parent_update_preedit_text_with_mode(-, %s, %u, %x, %x)\n",
	 iText->text, cursor_pos, visible, mode);
#else
    ibus_engine_update_preedit_text_with_mode
	(iEngine, iText, cursor_pos, visible, mode);
#endif
}

void parent_update_auxiliary_text
    (IBusEngine * iEngine, IBusText * iText, gboolean visible) {
#ifdef UNIT_TEST
    printf
	("* parent_update_auxiliary_text(-, %s, %x)\n",
	 (iText) ? iText->text : "NULL", visible);
#else
    if (!visible) {
	ibus_engine_hide_auxiliary_text(iEngine);
	return;
    }

    if (ibus_text_is_empty(iText)) {
	ibus_engine_hide_auxiliary_text(iEngine);
	return;
    }
    ibus_engine_show_auxiliary_text(iEngine);
#endif
}

void refresh_pre_edit_text(IBusChewingEngine * self)
{
    IBusText *iText=decorate_preedit(self->icPreEdit);
    if (self->preEditText){
	g_object_unref(self->preEditText);
    }
    self->preEditText = g_object_ref_sink(iText);
}

void update_pre_edit_text(IBusChewingEngine * self)
{
    refresh_pre_edit_text(self);
    gboolean visible = TRUE;

    IBusPreeditFocusMode mode;
    if (STRING_IS_EMPTY(self->preEditText->text)) {
	mode = IBUS_ENGINE_PREEDIT_CLEAR;
	visible = FALSE;
    } else {
	mode = IBUS_ENGINE_PREEDIT_COMMIT;
    }

    gint chiSymbolCursor = chewing_cursor_Current(self->icPreEdit->context);
    parent_update_preedit_text_with_mode(IBUS_ENGINE(self),
	    self->preEditText,
	    chiSymbolCursor, visible, mode);
}

void refresh_aux_text(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(INFO, "refresh_aux_text()");
    gint bpmfLen;
    gchar *auxStr;
    if (self->auxText != NULL) {
	g_object_unref(self->auxText);
    }

    if (chewing_aux_Length(self->icPreEdit->context) > 0) {
	auxStr=chewing_aux_String(self->icPreEdit->context);
	IBUS_CHEWING_LOG(INFO, "update_aux_text() auxStr=%s", auxStr);
    }else{
	IBUS_CHEWING_LOG(INFO, "update_aux_text() bpmf_check=%x", ibus_chewing_bopomofo_check(self->icPreEdit->context));
	gchar *bpmfStr=ibus_chewing_bopomofo_string(self->icPreEdit->context, &bpmfLen);
	IBUS_CHEWING_LOG(INFO, "update_aux_text() bpmfStr=%s", bpmfStr);
	self->auxText = g_object_ref_sink(ibus_text_new_from_string(bpmfStr));
	g_free(bpmfStr);
    }
}

void update_aux_text(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(DEBUG, "update_aux_text()");
    refresh_aux_text(self);
    parent_update_auxiliary_text(IBUS_ENGINE(self), self->auxText, TRUE);
}

void refresh_outgoing_text(IBusChewingEngine * self)
{
    gchar *outgoingStr=ibus_chewing_pre_edit_get_outgoing(self->icPreEdit);
    IBUS_CHEWING_LOG(INFO, "update_outgoing_text() outgoingStr=|%s|", outgoingStr);

    if (self->outgoingText){
	g_object_unref(self->outgoingText);
    }
    self->outgoingText = g_object_ref_sink(ibus_text_new_from_string(outgoingStr));
}

void commit_text(IBusChewingEngine * self)
{
    refresh_outgoing_text(self);
    parent_commit_text(IBUS_ENGINE(self));
    ibus_chewing_pre_edit_clear_outgoing(self->icPreEdit);    
}

