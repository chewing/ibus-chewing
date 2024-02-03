#include "ibus-chewing-engine-private.h"
#include "ibus-chewing-engine.h"

/**
 * ibus_chewing_engine_start:
 * @self: IBusChewingEngine instance.
 *
 * This is different with init. This will be called in the beginning of 
 * reset, enable, and focus_in for setup.
 */
void ibus_chewing_engine_start(IBusChewingEngine * self)
{
#ifndef UNIT_TEST
    if (!ibus_chewing_engine_has_status_flag
        (self, ENGINE_FLAG_PROPERTIES_REGISTERED)) {
        IBUS_ENGINE_GET_CLASS(self)->property_show(IBUS_ENGINE(self),
                                                   "InputMode");
        IBUS_ENGINE_GET_CLASS(self)->property_show(IBUS_ENGINE(self),
                                                   "AlnumSize");
        IBUS_ENGINE_GET_CLASS(self)->property_show(IBUS_ENGINE(self),
                                                   "setup_prop");
        ibus_engine_register_properties(IBUS_ENGINE(self), self->prop_list);
        ibus_chewing_engine_set_status_flag(self,
                                            ENGINE_FLAG_PROPERTIES_REGISTERED);
    }
#endif
    ibus_chewing_engine_use_setting(self);
    ibus_chewing_engine_restore_mode(self);
    ibus_chewing_engine_refresh_property_list(self);

}

/**
 * ibus_chewing_engine_reset:
 * @self: IBusChewingEngine instance.
 *
 * Reset the outgoing and pre_edit buffer and cursor
 * chewing_reset will NOT called here, as it will chnage the KBType and input mode.
 */
void ibus_chewing_engine_reset(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* reset");

    /* Always clean buffer */
    ibus_chewing_pre_edit_clear(self->icPreEdit);
#ifndef UNIT_TEST
    IBusEngine *engine = IBUS_ENGINE(self);

    ibus_engine_hide_auxiliary_text(engine);
    ibus_engine_hide_lookup_table(engine);
    ibus_engine_update_preedit_text(engine,
                                    IBUS_CHEWING_ENGINE_GET_CLASS(self)->emptyText, 0, FALSE);

#endif
}

void ibus_chewing_engine_enable(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* enable(): statusFlags=%x",
                     self->_priv->statusFlags);
    ibus_chewing_engine_start(self);
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
    ibus_chewing_engine_start(self);
    /* Shouldn't have anything to commit when Focus-in */
    ibus_chewing_pre_edit_clear(self->icPreEdit);
    refresh_pre_edit_text(self);
    refresh_aux_text(self);
    refresh_outgoing_text(self);

    ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_FOCUS_IN);
    IBUS_CHEWING_LOG(INFO, "focus_in() statusFlags=%x: return",
                     self->_priv->statusFlags);
}

void ibus_chewing_engine_focus_out(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(MSG, "* focus_out(): statusFlags=%x",
                     self->_priv->statusFlags);
    ibus_chewing_engine_clear_status_flag(self,
                                          ENGINE_FLAG_FOCUS_IN |
                                          ENGINE_FLAG_PROPERTIES_REGISTERED);
    ibus_chewing_engine_hide_property_list(self);

    if (ibus_chewing_pre_edit_get_property_boolean
        (self->icPreEdit, "clean-buffer-focus-out")) {
        /* Clean the buffer when focus out */
        ibus_chewing_pre_edit_clear(self->icPreEdit);
        refresh_pre_edit_text(self);
        refresh_aux_text(self);
    }

    IBUS_CHEWING_LOG(DEBUG, "focus_out(): return");
}



#if IBUS_CHECK_VERSION(1, 5, 4)
void ibus_chewing_engine_set_content_type(IBusEngine * engine,
                                          guint purpose, guint hints)
{
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_set_content_type(%d, %d)",
                     purpose, hints);

    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    if (purpose == IBUS_INPUT_PURPOSE_PASSWORD ||
        purpose == IBUS_INPUT_PURPOSE_PIN) {
        ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_IS_PASSWORD);
    } else {
        ibus_chewing_engine_clear_status_flag(self, ENGINE_FLAG_IS_PASSWORD);
    }
}
#endif

/*=================================================
 * Display text update routines
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

void parent_update_pre_edit_text(IBusEngine * iEngine,
                                 IBusText * iText, guint cursor_pos,
                                 gboolean visible)
{
#ifdef UNIT_TEST
    printf("* parent_update_pre_edit_text(-, %s, %u, %x)\n",
           iText->text, cursor_pos, visible);
#else
    ibus_engine_update_preedit_text(iEngine, iText, cursor_pos, visible);
#endif
}

void parent_update_pre_edit_text_with_mode(IBusEngine * iEngine,
                                           IBusText * iText,
                                           guint cursor_pos,
                                           gboolean visible,
                                           IBusPreeditFocusMode mode)
{
#ifdef UNIT_TEST
    printf
        ("* parent_update_pre_edit_text_with_mode(-, %s, %u, %x, %x)\n",
         iText->text, cursor_pos, visible, mode);
#else
    ibus_engine_update_preedit_text_with_mode
        (iEngine, iText, cursor_pos, visible, mode);
#endif
}

void parent_update_auxiliary_text(IBusEngine * iEngine,
                                  IBusText * iText, gboolean visible)
{
#ifdef UNIT_TEST
    printf("* parent_update_auxiliary_text(-, %s, %x)\n",
           (iText) ? iText->text : "NULL", visible);
#else
    if (!visible || ibus_text_is_empty(iText)) {
        ibus_engine_hide_auxiliary_text(iEngine);
        return;
    }
    ibus_engine_update_auxiliary_text(iEngine, iText, visible);
    ibus_engine_show_auxiliary_text(iEngine);
#endif
}

IBusText *decorate_pre_edit(IBusChewingPreEdit * icPreEdit,
                            IBusCapabilite capabilite)
{
    gchar *preEdit = ibus_chewing_pre_edit_get_pre_edit(icPreEdit);
    IBusText *iText = ibus_text_new_from_string(preEdit);
    gint chiSymbolCursor = chewing_cursor_Current(icPreEdit->context);
    gint charLen = (gint) g_utf8_strlen(preEdit, -1);

    IBUS_CHEWING_LOG(DEBUG, "decorate_pre_edit() cursor=%d preEdit=%s charLen=%d",
                     chiSymbolCursor, preEdit, charLen);

    /* Use single underline to mark whole pre-edit buffer */
    ibus_text_append_attribute(iText,
                               IBUS_ATTR_TYPE_UNDERLINE,
                               IBUS_ATTR_UNDERLINE_SINGLE,
                               0, charLen);

    /* Use background color to show current cursor */
    if (chiSymbolCursor < charLen) {
        ibus_text_append_attribute(iText,
                                   IBUS_ATTR_TYPE_BACKGROUND,
                                   0x00c8c8f0,
                                   chiSymbolCursor, chiSymbolCursor + 1);
        ibus_text_append_attribute(iText,
                                   IBUS_ATTR_TYPE_FOREGROUND,
                                   0x00000000,
                                   chiSymbolCursor, chiSymbolCursor + 1);
    }

    return iText;
}

void refresh_pre_edit_text(IBusChewingEngine * self)
{
    IBusText *iText =
        decorate_pre_edit(self->icPreEdit, self->_priv->capabilite);
    if (self->preEditText) {
        g_object_unref(self->preEditText);
    }
    self->preEditText = g_object_ref_sink(iText);
}

void update_pre_edit_text(IBusChewingEngine * self)
{
    refresh_pre_edit_text(self);
    gboolean visible = TRUE;
    gint bpmfLen = self->icPreEdit->bpmfLen;

    IBusPreeditFocusMode mode;

    if (STRING_IS_EMPTY(self->preEditText->text)) {
        mode = IBUS_ENGINE_PREEDIT_CLEAR;
        visible = FALSE;
    } else {
        mode = IBUS_ENGINE_PREEDIT_COMMIT;
    }

    parent_update_pre_edit_text_with_mode(IBUS_ENGINE(self),
                                          self->preEditText,
                                          cursor_current + bpmfLen, visible, mode);
}

void refresh_aux_text(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(INFO, "refresh_aux_text()");

    if (self->auxText != NULL) {
        g_object_unref(self->auxText);
    }

    /* Make auxText (text to be displayed in auxiliary candidate window).
     * Use auxText to show messages from libchewing, such as "已有：".
     */

    gboolean showPageNumber =
        ibus_chewing_pre_edit_get_property_boolean(self->icPreEdit,
                                                   "show-page-number");

    if (chewing_aux_Length(self->icPreEdit->context) > 0) {
        IBUS_CHEWING_LOG(INFO, "update_aux_text() chewing_aux_Length=%x",
                         chewing_aux_Length(self->icPreEdit->context));
        gchar *auxStr = chewing_aux_String(self->icPreEdit->context);

        IBUS_CHEWING_LOG(INFO, "update_aux_text() auxStr=%s", auxStr);
        self->auxText = g_object_ref_sink(ibus_text_new_from_string(auxStr));
        g_free(auxStr);
    } else if (showPageNumber && (chewing_cand_TotalPage(self->icPreEdit->context) > 0)) {
        int TotalPage = chewing_cand_TotalPage(self->icPreEdit->context);
        int currentPage = chewing_cand_CurrentPage(self->icPreEdit->context) + 1;
        self->auxText = g_object_ref_sink(ibus_text_new_from_printf("(%i/%i)", currentPage, TotalPage));
    } else {
        /* clear out auxText, otherwise it will be displayed continually. */
        self->auxText = g_object_ref_sink(ibus_text_new_from_static_string(""));
    }
}

void update_aux_text(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(DEBUG, "update_aux_text()");
    refresh_aux_text(self);
    parent_update_auxiliary_text(IBUS_ENGINE(self), self->auxText, TRUE);
}

void update_lookup_table(IBusChewingEngine * self)
{
    IBUS_CHEWING_LOG(DEBUG, "update_lookup_table() CurrentPage=%d",
                     chewing_cand_CurrentPage(self->icPreEdit->context));

    gboolean isShow =
        ibus_chewing_pre_edit_has_flag(self->icPreEdit, FLAG_TABLE_SHOW);

    if (isShow) {
#ifndef UNIT_TEST
        ibus_engine_update_lookup_table(IBUS_ENGINE(self),
                                        self->icPreEdit->iTable, isShow);
        ibus_engine_show_lookup_table(IBUS_ENGINE(self));
#endif
    } else {
#ifndef UNIT_TEST
        ibus_engine_update_lookup_table(IBUS_ENGINE(self),
                                        self->icPreEdit->iTable, isShow);
        ibus_engine_hide_lookup_table(IBUS_ENGINE(self));
#endif
    }
}

void refresh_outgoing_text(IBusChewingEngine * self)
{
    gchar *outgoingStr = ibus_chewing_pre_edit_get_outgoing(self->icPreEdit);

    IBUS_CHEWING_LOG(INFO, "refresh_outgoing_text() outgoingStr=|%s|",
                     outgoingStr);

    if (self->outgoingText) {
        g_object_unref(self->outgoingText);
    }
    self->outgoingText =
        g_object_ref_sink(ibus_text_new_from_string(outgoingStr));
    IBUS_CHEWING_LOG(DEBUG, "refresh_outgoing_text() outgoingText=|%s|",
                     self->outgoingText->text);
}

void commit_text(IBusChewingEngine * self)
{
    refresh_outgoing_text(self);
    if (!ibus_text_is_empty(self->outgoingText)
        || !ibus_chewing_engine_has_status_flag(self, ENGINE_FLAG_FOCUS_IN)) {
        parent_commit_text(IBUS_ENGINE(self));
    }

    ibus_chewing_pre_edit_clear_outgoing(self->icPreEdit);
}
