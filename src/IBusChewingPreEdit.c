#include <stdlib.h>
#include <ctype.h>
#include "IBusChewingUtil.h"
#include "IBusChewingProperties.h"
#include "IBusChewingPreEdit.h"
#include "IBusChewingPreEdit-private.h"

/**************************************
 * Methods
 */

IBusChewingPreEdit *ibus_chewing_pre_edit_new(MkdgBackend * backend)
{
    IBusChewingPreEdit *self = g_new0(IBusChewingPreEdit, 1);
    self->iProperties = ibus_chewing_properties_new(backend, self, NULL);
    self->preEdit = g_string_sized_new(IBUS_CHEWING_MAX_BYTES);
    self->outgoing = g_string_sized_new(IBUS_CHEWING_MAX_BYTES);
    self->keyLast = 0;
    self->bpmfLen = 0;
    self->wordLen = 0;
    self->engine = NULL;

    /* Create chewing context */
    gchar buf[100];
    g_snprintf(buf, 100, "%s/.chewing", getenv("HOME"));
#if !CHEWING_CHECK_VERSION(0,4,0)
    gint ret = chewing_Init(QUOTE_ME(CHEWING_DATADIR_REAL), buf);
    if (ret) {
	IBUS_CHEWING_LOG(ERROR,
			 "ibus_chewing_pre_edit_new() chewing_Init(%s,%s)",
			 QUOTE_ME(CHEWING_DATADIR_REAL), buf);
    }
#endif
    self->context = chewing_new();
    chewing_set_ChiEngMode(self->context, CHINESE_MODE);

    self->iTable =
	g_object_ref_sink(ibus_chewing_lookup_table_new
			  (self->iProperties, self->context));
    return self;
}

void ibus_chewing_pre_edit_free(IBusChewingPreEdit * self)
{
    /* properties need not be freed here */
    chewing_delete(self->context);
    g_string_free(self->preEdit, TRUE);
    g_string_free(self->outgoing, TRUE);
    ibus_lookup_table_clear(self->iTable);
    g_object_unref(self->iTable);
    g_free(self);
}

gchar *ibus_chewing_pre_edit_get_bopomofo_string(IBusChewingPreEdit * self)
{
#if CHEWING_CHECK_VERSION(0,4,0)
    const gchar *buf = chewing_bopomofo_String_static(self->context);
    self->bpmfLen = (gint) g_utf8_strlen(buf, 0);
    return g_strdup(buf);
#else
    return chewing_zuin_String(self->context, &(self->bpmfLen));
#endif
}

void ibus_chewing_pre_edit_use_all_configure(IBusChewingPreEdit * self)
{
    mkdg_properties_use_all(self->iProperties->properties, NULL);
}

void ibus_chewing_pre_edit_update_outgoing(IBusChewingPreEdit * self)
{
    if (ibus_chewing_pre_edit_has_flag(self, FLAG_UPDATED_OUTGOING)) {
	/* Commit already sent to outgoing, no need to update again */
	return;
    }
    if (chewing_commit_Check(self->context)) {
	/* commit_Check=1 means new commit available */
	gchar *commitStr = chewing_commit_String(self->context);
	IBUS_CHEWING_LOG(INFO, "commitStr=|%s|\n", commitStr);
	g_string_append(self->outgoing, commitStr);
	g_free(commitStr);
	ibus_chewing_pre_edit_set_flag(self, FLAG_UPDATED_OUTGOING);
    }
    IBUS_CHEWING_LOG(INFO, "outgoing=|%s|\n", self->outgoing->str);
    IBUS_CHEWING_LOG(DEBUG,
		     "ibus_chewing_pre_edit_update_outgoing(-): return: outgoing=|%s|",
		     self->outgoing->str);
}

void ibus_chewing_pre_edit_update(IBusChewingPreEdit * self)
{
    IBUS_CHEWING_LOG(DEBUG, "* ibus_chewing_pre_edit_update(-)");

    /* Make preEdit */
    gchar *bufferStr = chewing_buffer_String(self->context);
    gchar *bpmfStr = ibus_chewing_pre_edit_get_bopomofo_string(self);
    g_string_assign(self->preEdit, "");
    gint i;
    gchar *cP = bufferStr;
    gunichar uniCh;
    IBUS_CHEWING_LOG(INFO,
		     "* ibus_chewing_pre_edit_update(-)  bufferStr=|%s|, bpmfStr=|%s| bpmfLen=%d cursor=%d",
		     bufferStr, bpmfStr, self->bpmfLen, cursor_current);
    for (i = 0; i < chewing_buffer_Len(self->context) && cP != NULL; i++) {
	if (i == cursor_current) {
	    /* Insert bopomofo string */
	    g_string_append(self->preEdit, bpmfStr);
	}
	uniCh = g_utf8_get_char(cP);
	g_string_append_unichar(self->preEdit, uniCh);
	cP = g_utf8_next_char(cP);
    }
    if (chewing_buffer_Len(self->context) <= cursor_current) {
	g_string_append(self->preEdit, bpmfStr);
    }
    self->wordLen = i + self->bpmfLen;
    g_free(bufferStr);
    g_free(bpmfStr);

    ibus_chewing_pre_edit_update_outgoing(self);

}

guint ibus_chewing_pre_edit_length(IBusChewingPreEdit * self)
{
    return self->preEdit->len;
}

guint ibus_chewing_pre_edit_word_length(IBusChewingPreEdit * self)
{
    return self->wordLen;
}

guint ibus_chewing_pre_edit_word_limit(IBusChewingPreEdit * self)
{
    return chewing_get_maxChiSymbolLen(self->context);
}

gchar *ibus_chewing_pre_edit_get_pre_edit(IBusChewingPreEdit * self)
{
    return self->preEdit->str;
}

gchar *ibus_chewing_pre_edit_get_outgoing(IBusChewingPreEdit * self)
{
    return self->outgoing->str;
}

/* force_commit is run when receiving IGNORE event */
void ibus_chewing_pre_edit_force_commit(IBusChewingPreEdit * self)
{
    IBUS_CHEWING_LOG(INFO,
		     "ibus_chewing_pre_edit_force_commit(-) bpmf_check=%d buffer_check=%d",
		     bpmf_check, chewing_buffer_Check(self->context));

    /* Ignore the context and
     * Commit whatever in preedit buffer
     */
    g_string_append(self->outgoing, self->preEdit->str);

    ibus_chewing_pre_edit_clear_pre_edit(self);
    ibus_chewing_pre_edit_update(self);

}

void ibus_chewing_pre_edit_clear(IBusChewingPreEdit * self)
{
    IBUS_CHEWING_LOG(INFO, "ibus_chewing_pre_edit_clear(-)");

    ibus_chewing_pre_edit_clear_pre_edit(self);
    ibus_chewing_pre_edit_clear_outgoing(self);
    ibus_chewing_pre_edit_update(self);
}

void ibus_chewing_pre_edit_clear_bopomofo(IBusChewingPreEdit * self)
{
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_clear_bopomofo(-)");

    /* Esc key can close candidate list, clear bopomofo, and clear 
     * the whole pre-edit buffer. Make sure it acts as we expected.
     */
    if (table_is_showing) {
        chewing_handle_Esc(self->context);
    }

    if (bpmf_check) {
        chewing_handle_Esc(self->context);
    }

    ibus_chewing_pre_edit_update(self);
}

void ibus_chewing_pre_edit_clear_pre_edit(IBusChewingPreEdit * self)
{
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_clear_pre_edit(-)");

    ibus_chewing_pre_edit_clear_bopomofo(self);

    /* Save the orig Esc clean buffer state */
    gint origState = chewing_get_escCleanAllBuf(self->context);
    chewing_set_escCleanAllBuf(self->context, TRUE);

    chewing_handle_Esc(self->context);

    chewing_set_escCleanAllBuf(self->context, origState);
    ibus_chewing_pre_edit_update(self);
}

void ibus_chewing_pre_edit_clear_outgoing(IBusChewingPreEdit * self)
{
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_clear_outgoing(-)");
    g_string_assign(self->outgoing, "");

}

#define is_chinese ibus_chewing_pre_edit_get_chi_eng_mode(self)
#define is_full_shape ibus_chewing_pre_edit_get_full_half_mode(self)
gboolean ibus_chewing_pre_edit_get_chi_eng_mode(IBusChewingPreEdit * self)
{
    return chewing_get_ChiEngMode(self->context) != 0;
}

gboolean ibus_chewing_pre_edit_get_full_half_mode(IBusChewingPreEdit *
						  self)
{
    return chewing_get_ShapeMode(self->context) != 0;
}

void ibus_chewing_pre_edit_set_chi_eng_mode(IBusChewingPreEdit * self,
					    gboolean chineseMode)
{
    /* Clear bopomofo when toggling Chi-Eng Mode */
    if (!chineseMode && is_chinese && bpmf_check) {
        ibus_chewing_pre_edit_clear_bopomofo(self);
    }
    chewing_set_ChiEngMode(self->context, (chineseMode) ? 1 : 0);
}

void ibus_chewing_pre_edit_set_full_half_mode(IBusChewingPreEdit * self,
					      gboolean fullShapeMode)
{
    if (is_chinese && bpmf_check) {
    /* Clear bopomofo when toggling Full-Half Mode */
        ibus_chewing_pre_edit_clear_bopomofo(self);
    }
    chewing_set_ShapeMode(self->context, (fullShapeMode) ? 1 : 0);
}

/**************************************
 * ibus_chewing_pre_edit key processing
 */
KSym self_key_sym_fix(IBusChewingPreEdit * self, KSym kSym,
		      KeyModifiers unmaskedMod)
{
    gchar caseConversionMode = default_english_case_short;
    if (!ibus_chewing_pre_edit_get_property_boolean(self,
						    "capslock-toggle-chinese"))
    {
	caseConversionMode = 'n';
    }
    if (is_chinese) {
	/* 
	 * Ignore the status of CapsLock, thus
	 */
	if (is_shift) {
	    return toupper(kSym);
	}
	return tolower(kSym);
    } else {
	/* May need to change case if Caps Lock toggle chinese */
	switch (caseConversionMode) {
	case 'l':
	    if (is_shift) {
		/* Uppercase */
		return toupper(kSym);
	    }
	    /* Lowercase */
	    return tolower(kSym);
	case 'u':
	    if (is_shift) {
		/* Lowercase */
		return tolower(kSym);
	    }
	    /* Uppercase */
	    return toupper(kSym);
	default:
	    break;
	}
    }
    return kSym;
}

EventResponse self_handle_key_sym_default(IBusChewingPreEdit * self,
					  KSym kSym,
					  KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);
    handle_log("key_sym_default");

    /* Seem like we need to disable easy symbol temporarily 
     * otherwise the key won't process
     */
    gint easySymbolInput = chewing_get_easySymbolInput(self->context);
    if (maskedMod != IBUS_SHIFT_MASK) {
	chewing_set_easySymbolInput(self->context, 0);
    }
    EventResponse response = EVENT_RESPONSE_UNDECIDED;
    KSym fixedKSym = self_key_sym_fix(self, kSym, unmaskedMod);
    IBUS_CHEWING_LOG(DEBUG,
		     "* self_handle_key_sym_default(): new kSym %x(%s), %x(%s)",
		     fixedKSym, key_sym_get_name(fixedKSym), unmaskedMod,
		     modifiers_to_string(unmaskedMod));
    gint ret = chewing_handle_Default(self->context, fixedKSym);
    /* Handle quick commit */
    ibus_chewing_pre_edit_clear_flag(self, FLAG_UPDATED_OUTGOING);
    ibus_chewing_pre_edit_update_outgoing(self);

    switch (ret) {
    case 0:
    case KEYSTROKE_COMMIT:
	response = EVENT_RESPONSE_PROCESS;
	break;
    case KEYSTROKE_ABSORB:
	response = EVENT_RESPONSE_ABSORB;
	break;
    case KEYSTROKE_IGNORE:
	response = EVENT_RESPONSE_IGNORE;
	break;
    default:
	break;
    }

    IBUS_CHEWING_LOG(DEBUG,
		     "self_handle_key_sym_default() ret=%d response=%d",
		     ret, response);
    /* Restore easySymbolInput */
    chewing_set_easySymbolInput(self->context, easySymbolInput);
    return response;
}

/* Return FALSE if the key should not be processed with input method */
EventResponse self_handle_num(IBusChewingPreEdit * self, KSym kSym,
			      KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK | IBUS_CONTROL_MASK);
    absorb_when_release;
    handle_log("num");

    if (is_ctrl_only) {
	return
	    event_process_or_ignore(!chewing_handle_CtrlNum
				    (self->context, kSym));
    }
    /* maskedMod= 0 */
    return self_handle_key_sym_default(self, kSym, unmaskedMod);
}

EventResponse self_handle_num_keypad(IBusChewingPreEdit * self,
				     KSym kSym, KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK | IBUS_CONTROL_MASK);
    absorb_when_release;
    handle_log("num_keypad");

    if ((maskedMod != 0) && (!is_shift_only) && (!is_ctrl_only)) {
	return EVENT_RESPONSE_IGNORE;
    }


    KSym kSymFinal = kSym;
    KSym kSymEquiv = key_sym_KP_to_normal(kSym);
    gboolean numpadIsAlwaysNum =
	ibus_chewing_pre_edit_get_property_boolean(self,
						   "numpad-always-number");

    if (numpadIsAlwaysNum) {
	kSymFinal = kSymEquiv;
    }
    if (is_ctrl_only) {
	return
	    event_process_or_ignore(!chewing_handle_CtrlNum
				    (self->context, kSymFinal));
    }
    /* maskedMod= 0 */
    return self_handle_key_sym_default(self, kSym, unmaskedMod);
}

EventResponse self_handle_caps_lock(IBusChewingPreEdit * self, KSym kSym,
				    KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    if (!ibus_chewing_pre_edit_get_property_boolean(self,
						    "capslock-toggle-chinese")) {
        /* Ignore the Caps Lock event when it does not toggle Chinese */
        return EVENT_RESPONSE_IGNORE;
    }

    absorb_when_release;
    handle_log("caps_lock");

    /* Clear bopomofo when toggling Chi-Eng Mode */
    if (is_chinese && bpmf_check) {
        ibus_chewing_pre_edit_clear_bopomofo(self);
    }

    return
	event_process_or_ignore(!chewing_handle_Capslock(self->context));
}

EventResponse self_handle_shift(IBusChewingPreEdit * self, KSym kSym,
				KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);
    handle_log("shift");

    gboolean shiftIsToggleChinese =
	ibus_chewing_pre_edit_get_property_boolean(self,
						   "shift-toggle-chinese");

    if (!shiftIsToggleChinese) {
	return EVENT_RESPONSE_IGNORE;
    }

    if (!event_is_released(unmaskedMod)) {
	return EVENT_RESPONSE_ABSORB;
    }

    /* keyLast != Shift means Shift is just part of combination,
     * thus should not be recognized as single Shift key
     */
    if (self->keyLast != IBUS_KEY_Shift_L
	&& self->keyLast != IBUS_KEY_Shift_R) {
	return EVENT_RESPONSE_ABSORB;
    }

    ibus_chewing_pre_edit_toggle_chi_eng_mode(self);
    return EVENT_RESPONSE_ABSORB;
}

EventResponse self_handle_space(IBusChewingPreEdit * self, KSym kSym,
				KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK | IBUS_CONTROL_MASK);
    absorb_when_release;
    handle_log("space");

    if (is_shift_only) {
	    ibus_chewing_pre_edit_toggle_full_half_mode(self);
	    return EVENT_RESPONSE_PROCESS;
    }

    /* Bug of libchewing: 
     * when "space to select" is enabled, chewing_handle_Space() will
     * ignore the first space. Therefore, use chewing_handle_Space()
     * only if the buffer is not empty and we want to select.
     */
    gboolean spaceAsSelection =
	ibus_chewing_pre_edit_get_property_boolean(self,
						   "space-as-selection");

    if (is_chinese && !buffer_is_empty && !bpmf_check && spaceAsSelection) {
        return event_process_or_ignore(!chewing_handle_Space(self->context));
    } else {
        return self_handle_key_sym_default(self, kSym, unmaskedMod);
    }
}

EventResponse self_handle_return(IBusChewingPreEdit * self, KSym kSym,
				 KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;
    ignore_when_buffer_is_empty;
    handle_log("return");

    EventResponse response =
	event_process_or_ignore(!chewing_handle_Enter(self->context));

    /* Handle quick commit */
    ibus_chewing_pre_edit_clear_flag(self, FLAG_UPDATED_OUTGOING);
    ibus_chewing_pre_edit_update_outgoing(self);

    return response;
}

EventResponse self_handle_backspace(IBusChewingPreEdit * self, KSym kSym,
				    KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;

    if (buffer_is_empty && !table_is_showing) {
        return EVENT_RESPONSE_IGNORE;
    }

    handle_log("backspace");

    return
	event_process_or_ignore(!chewing_handle_Backspace(self->context));
}

EventResponse self_handle_delete(IBusChewingPreEdit * self, KSym kSym,
				 KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;
    ignore_when_buffer_is_empty;
    handle_log("delete");

    return event_process_or_ignore(!chewing_handle_Del(self->context));
}

EventResponse self_handle_escape(IBusChewingPreEdit * self, KSym kSym,
				 KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;

    if (buffer_is_empty && !table_is_showing) {
        return EVENT_RESPONSE_IGNORE;
    }

    handle_log("escape");

    return event_process_or_ignore(!chewing_handle_Esc(self->context));
}

EventResponse self_handle_left(IBusChewingPreEdit * self, KSym kSym,
			       KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);
    absorb_when_release;

    if (buffer_is_empty && !table_is_showing) {
        return EVENT_RESPONSE_IGNORE;
    }

    handle_log("left");

    if (is_shift_only) {
	return
	    event_process_or_ignore(!chewing_handle_ShiftLeft
				    (self->context));
    }

    return event_process_or_ignore(!chewing_handle_Left(self->context));
}

EventResponse self_handle_up(IBusChewingPreEdit * self, KSym kSym,
			     KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;

    if (buffer_is_empty && !table_is_showing) {
        return EVENT_RESPONSE_IGNORE;
    }

    handle_log("up");

    return event_process_or_ignore(!chewing_handle_Up(self->context));
}

EventResponse self_handle_right(IBusChewingPreEdit * self, KSym kSym,
				KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);
    absorb_when_release;

    if (buffer_is_empty && !table_is_showing) {
        return EVENT_RESPONSE_IGNORE;
    }

    handle_log("right");

    if (is_shift_only) {
	return
	    event_process_or_ignore(!chewing_handle_ShiftRight
				    (self->context));
    }

    return event_process_or_ignore(!chewing_handle_Right(self->context));
}

EventResponse self_handle_down(IBusChewingPreEdit * self, KSym kSym,
			       KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;

    if (buffer_is_empty && !table_is_showing) {
        return EVENT_RESPONSE_IGNORE;
    }

    handle_log("down");

    return event_process_or_ignore(!chewing_handle_Down(self->context));
}

EventResponse self_handle_page_up(IBusChewingPreEdit * self, KSym kSym,
				  KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;

    if (buffer_is_empty && !table_is_showing) {
        return EVENT_RESPONSE_IGNORE;
    }

    handle_log("page_up");

    return event_process_or_ignore(!chewing_handle_PageUp(self->context));
}

EventResponse self_handle_page_down(IBusChewingPreEdit * self, KSym kSym,
				    KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;

    if (buffer_is_empty && !table_is_showing) {
        return EVENT_RESPONSE_IGNORE;
    }

    handle_log("page_down");

    return
	event_process_or_ignore(!chewing_handle_PageDown(self->context));
}

EventResponse self_handle_tab(IBusChewingPreEdit * self, KSym kSym,
			      KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;
    ignore_when_buffer_is_empty;
    handle_log("tab");

    return event_process_or_ignore(!chewing_handle_Tab(self->context));
}

EventResponse self_handle_home(IBusChewingPreEdit * self, KSym kSym,
			       KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;
    ignore_when_buffer_is_empty;
    handle_log("home");

    return event_process_or_ignore(!chewing_handle_Home(self->context));
}

EventResponse self_handle_end(IBusChewingPreEdit * self, KSym kSym,
			      KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;
    ignore_when_buffer_is_empty;
    handle_log("end");

    return event_process_or_ignore(!chewing_handle_End(self->context));
}

EventResponse self_handle_special(IBusChewingPreEdit * self, KSym kSym,
				  KeyModifiers unmaskedMod)
{
    /* KSym >=128 is special key, which IM ignore. */
    return EVENT_RESPONSE_IGNORE;
}

EventResponse self_handle_default(IBusChewingPreEdit * self, KSym kSym,
				  KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);
    absorb_when_release;
    handle_log("default");

    return self_handle_key_sym_default(self, kSym, unmaskedMod);
}

KeyHandlingRule keyHandlingRules[] = {
    {
     IBUS_KEY_0, IBUS_KEY_9, self_handle_num}
    , {
       IBUS_KEY_KP_0, IBUS_KEY_KP_9, self_handle_num_keypad}
    , {
       IBUS_KEY_Caps_Lock, IBUS_KEY_Caps_Lock, self_handle_caps_lock}
    , {
       IBUS_KEY_Shift_L, IBUS_KEY_Shift_R, self_handle_shift}
    , {
       IBUS_KEY_space, IBUS_KEY_space, self_handle_space}
    , {
       IBUS_KEY_Return, IBUS_KEY_Return, self_handle_return}
    , {
       IBUS_KEY_KP_Enter, IBUS_KEY_KP_Enter, self_handle_return}
    , {
       IBUS_KEY_BackSpace, IBUS_KEY_BackSpace, self_handle_backspace}
    , {
       IBUS_KEY_Delete, IBUS_KEY_Delete, self_handle_delete}
    , {
       IBUS_KEY_KP_Delete, IBUS_KEY_KP_Delete, self_handle_delete}
    , {
       IBUS_KEY_Escape, IBUS_KEY_Escape, self_handle_escape}
    , {
       IBUS_KEY_Left, IBUS_KEY_Left, self_handle_left}
    , {
       IBUS_KEY_KP_Left, IBUS_KEY_KP_Left, self_handle_left}
    , {
       IBUS_KEY_Up, IBUS_KEY_Up, self_handle_up}
    , {
       IBUS_KEY_KP_Up, IBUS_KEY_KP_Up, self_handle_up}
    , {
       IBUS_KEY_Right, IBUS_KEY_Right, self_handle_right}
    , {
       IBUS_KEY_KP_Right, IBUS_KEY_KP_Right, self_handle_right}
    , {
       IBUS_KEY_Down, IBUS_KEY_Down, self_handle_down}
    , {
       IBUS_KEY_KP_Down, IBUS_KEY_KP_Down, self_handle_down}
    , {
       IBUS_KEY_Page_Up, IBUS_KEY_Page_Up, self_handle_page_up}
    , {
       IBUS_KEY_KP_Page_Up, IBUS_KEY_KP_Page_Up, self_handle_page_up}
    , {
       IBUS_KEY_Page_Down, IBUS_KEY_Page_Down, self_handle_page_down}
    , {
       IBUS_KEY_KP_Page_Down, IBUS_KEY_KP_Page_Down, self_handle_page_down}
    , {
       IBUS_KEY_Tab, IBUS_KEY_Tab, self_handle_tab}
    , {
       IBUS_KEY_Home, IBUS_KEY_Home, self_handle_home}
    , {
       IBUS_KEY_KP_Home, IBUS_KEY_KP_Home, self_handle_home}
    , {
       IBUS_KEY_End, IBUS_KEY_End, self_handle_end}
    , {
       IBUS_KEY_KP_End, IBUS_KEY_KP_End, self_handle_end}
    , {
       128, G_MAXUINT, self_handle_special}
    , {
       0, 0, self_handle_default}
    ,
};

static KeyHandlingRule *self_key_sym_find_key_handling_rule(KSym kSym)
{
    gint i;
    for (i = 0; keyHandlingRules[i].kSymLower != 0; i++) {
	if ((keyHandlingRules[i].kSymLower <= kSym) &&
	    (kSym <= keyHandlingRules[i].kSymUpper)) {
	    return &(keyHandlingRules[i]);
	}
    }
    return &(keyHandlingRules[i]);
}

#define handle_key(kSym, unmaskedMod) (self_key_sym_find_key_handling_rule(kSym))->keyFunc(self, kSym, unmaskedMod)

#define process_key_debug(prompt) IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_process_key(): %s flags=%x buff_check=%d bpmf_check=%d cursor=%d total_choice=%d is_chinese=%d is_full_shape=%d is_plain_zhuyin=%d" ,\
	prompt,	self->flags, chewing_buffer_Check(self->context),\
	bpmf_check, cursor_current, total_choice, is_chinese, is_full_shape, is_plain_zhuyin)


/* keyCode should be converted to kSym already */
gboolean ibus_chewing_pre_edit_process_key
    (IBusChewingPreEdit * self, KSym kSym, KeyModifiers unmaskedMod) {
    IBUS_CHEWING_LOG(INFO,
		     "***** ibus_chewing_pre_edit_process_key(-,%x(%s),%x(%s))",
		     kSym, key_sym_get_name(kSym),
		     unmaskedMod, modifiers_to_string(unmaskedMod));
    process_key_debug("Before response");

    /* Find corresponding rule */
    EventResponse response;
    response = handle_key(kSym, unmaskedMod);

    IBUS_CHEWING_LOG(DEBUG,
		     "ibus_chewing_pre_edit_process_key() response=%x",
		     response);
    process_key_debug("After response");
    self->keyLast = kSym;
    switch (response) {
    case EVENT_RESPONSE_ABSORB:
	return TRUE;
    case EVENT_RESPONSE_IGNORE:
	return FALSE;
    default:
	break;
    }

    /** 
     *Plain zhuyin mode
     */
    if (is_plain_zhuyin && !bpmf_check) {
	/* libchewing functions are used here to skip the check 
	 * that handle_key functions perform.
	 */
	if (kSym == IBUS_KEY_Escape) {
	    ibus_chewing_pre_edit_clear_pre_edit(self);
	} else if (kSym == IBUS_KEY_Return && table_is_showing) {
	    /* Use Enter to select the last chosen */
	    chewing_handle_Up(self->context);
	    chewing_handle_Enter(self->context);
	} else if (is_chinese && !table_is_showing) {
	    /* Character completed, and lookup table is not show */
	    /* Then open lookup table */

	    if (is_shift) {
		/* For Chinese symbols */
		chewing_handle_Left(self->context);
	    }
	    chewing_handle_Down(self->context);
	} else if (total_choice == 0) {
	    /* lookup table is shown */
	    /* but selection is done */
	    chewing_handle_Enter(self->context);
	}
    }
    process_key_debug("After plain-zhuyin handling");

    ibus_chewing_pre_edit_update(self);

    guint candidateCount =
	ibus_chewing_lookup_table_update(self->iTable, self->iProperties,
					 self->context);
    IBUS_CHEWING_LOG(INFO,
		     "ibus_chewing_pre_edit_process_key() candidateCount=%d",
		     candidateCount);

    if (candidateCount) {
	ibus_chewing_pre_edit_set_flag(self, FLAG_TABLE_SHOW);
    } else {
	ibus_chewing_pre_edit_clear_flag(self, FLAG_TABLE_SHOW);
    }
    return TRUE;
}

KSym ibus_chewing_pre_edit_key_code_to_key_sym
    (IBusChewingPreEdit * self, KSym keySym,
     guint keyCode, KeyModifiers unmaskedMod) {
    KSym kSym = keySym;
    if (!is_chinese) {
	/* English mode, pass as-is */
	return kSym;
    }

    if (!ibus_chewing_pre_edit_is_system_keyboard_layout(self)) {
	/* Use en_US keyboard layout */
	/* ibus_keymap_lookup_key_sym treats keycode >= 256 */
	/* as IBUS_VoidSymbol */
	kSym =
	    ibus_keymap_lookup_keysym(ibus_keymap_get
				      ("us"), keyCode, unmaskedMod);
	if (kSym == IBUS_VoidSymbol) {
	    /* Restore key_sym */
	    kSym = keySym;
	}
    }

    return kSym;
}
