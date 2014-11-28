#include <stdlib.h>
#include <ctype.h>
#include "IBusChewingUtil.h"
#include "IBusChewingProperties.h"
#include "IBusChewingPreEdit.h"

#define is_plain_zhuyin ibus_chewing_pre_edit_get_property_boolean(self, "plain-zhuyin")
#define bpmf_check ibus_chewing_bopomofo_check(self->context)
#define table_is_showing ibus_chewing_pre_edit_has_flag(self,FLAG_TABLE_SHOW)

#define cursor_current chewing_cursor_Current(self->context)
#define total_choice chewing_cand_TotalChoice(self->context)


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
    ibus_chewing_pre_edit_use_all_configure(self);
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

void ibus_chewing_pre_edit_use_all_configure(IBusChewingPreEdit * self)
{
    mkdg_properties_use_all(self->iProperties->properties, NULL);
}

void ibus_chewing_pre_edit_update(IBusChewingPreEdit * self)
{
    IBUS_CHEWING_LOG(MSG, "* ibus_chewing_pre_edit_update(-)");

    /* Make preEdit */
    gchar *bufferStr = chewing_buffer_String(self->context);
    gint count;
    gchar *bpmfStr = ibus_chewing_bopomofo_string(self->context, &count);
    self->bpmfLen = count;
    g_string_assign(self->preEdit, "");
    gint i;
    gchar *cP = bufferStr;
    gunichar uniCh;
    IBUS_CHEWING_LOG(INFO,
		     "* ibus_chewing_pre_edit_update(-)  bufferStr=|%s|, bpmfStr=|%s| count=%d cursor=%d",
		     bufferStr, bpmfStr, count, cursor_current);
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
    self->wordLen = i + count;
    g_free(bufferStr);
    g_free(bpmfStr);

    /* Make outgoing */
    IBUS_CHEWING_LOG(DEBUG,
		     "ibus_chewing_pre_edit_update(-): preEdit=|%s| commit_check=%d outgoing=|%s|",
		     self->preEdit->str,
		     chewing_commit_Check(self->context),
		     self->outgoing->str);
    if (chewing_commit_Check(self->context)) {
	/* commit_Check=1 means new commit available */
	gchar *commitStr = chewing_commit_String(self->context);
	g_string_append(self->outgoing, commitStr);
	g_free(commitStr);
    }
    IBUS_CHEWING_LOG(DEBUG,
		     "ibus_chewing_pre_edit_update(-): preEdit=|%s| outgoing=|%s|",
		     self->preEdit->str, self->outgoing->str);
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
     * TODO: an option to control this behavior
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

void ibus_chewing_pre_edit_clear_pre_edit(IBusChewingPreEdit * self)
{
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_clear_pre_edit(-)");
    /* Clear chewing buffer */
    /* Save the orig Esc clean buffer state */
    gint origState = chewing_get_escCleanAllBuf(self->context);

    /* Send a false event, then clean it to wipe the commit  */
    chewing_handle_Default(self->context, '1');

    chewing_set_escCleanAllBuf(self->context, TRUE);
    chewing_handle_Esc(self->context);
    chewing_set_escCleanAllBuf(self->context, origState);
}

void ibus_chewing_pre_edit_clear_outgoing(IBusChewingPreEdit * self)
{
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_clear_outgoing(-)");
    g_string_assign(self->outgoing, "");
}

/**************************************
 * ibus_chewing_pre_edit key processing
 */


#define modifiers_mask(unmaskedMod) unmaskedMod & (IBUS_SHIFT_MASK | IBUS_CONTROL_MASK | IBUS_MOD1_MASK)

#define event_is_released(unmaskedMod) ((unmaskedMod & IBUS_RELEASE_MASK) !=0 )
#define is_shift_only (maskedMod == IBUS_SHIFT_MASK)
#define is_shift (unmaskedMod & IBUS_SHIFT_MASK)
#define is_ctrl_only (maskedMod == IBUS_CONTROL_MASK)
#define is_chinese (chewing_get_ChiEngMode(self->context)!=0)
#define buffer_is_empty (ibus_chewing_pre_edit_is_empty(self))
#define filter_modifiers(mask) KeyModifiers maskedMod = modifiers_mask(unmaskedMod); \
    if (maskedMod & (~mask)){ return EVENT_RESPONSE_IGNORE; } \
    if (maskedMod == (IBUS_SHIFT_MASK | IBUS_CONTROL_MASK)){ return EVENT_RESPONSE_IGNORE; }
#define absorb_when_release if (event_is_released(unmaskedMod)){ return EVENT_RESPONSE_ABSORB; }
#define ignore_when_buffer_is_empty if (buffer_is_empty) { return EVENT_RESPONSE_IGNORE;}



#define event_process_or_ignore(cond) (cond) ? EVENT_RESPONSE_PROCESS: EVENT_RESPONSE_IGNORE

#define handle_log(funcName) IBUS_CHEWING_LOG(INFO, "* self_handle_%s(-,%x(%s),%x(%s))", funcName, kSym, key_sym_get_name(kSym), unmaskedMod, modifiers_to_string(unmaskedMod));


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

    if (!is_chinese
	&& ibus_chewing_pre_edit_get_property_boolean(self,
						      "force-lowercase-english"))
    {
	/* English mode and force-lower-case-engligh=TRUE */
	if (isupper(kSym) && !is_shift_only) {
	    kSym = tolower(kSym);
	} else if (islower(kSym) && is_shift_only) {
	    kSym = toupper(kSym);
	}
    }

    gint ret = chewing_handle_Default(self->context, kSym);

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
    absorb_when_release;
    handle_log("caps_lock");

    /* When Chi->Eng with incomplete character */
    if (is_chinese && bpmf_check) {
	ibus_chewing_pre_edit_force_commit(self);
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
//#if !CHEWING_CHECK_VERSION(0,4,0)
    /* When Chi->Eng with incomplete character */
    if (is_chinese && bpmf_check) {
	ibus_chewing_pre_edit_force_commit(self);
    }
//#endif
    chewing_set_ChiEngMode(self->context, !is_chinese);
    return EVENT_RESPONSE_ABSORB;
}

EventResponse self_handle_space(IBusChewingPreEdit * self, KSym kSym,
				KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK | IBUS_CONTROL_MASK);
    absorb_when_release;
    handle_log("space");

    if (is_shift_only) {
	return
	    event_process_or_ignore(!chewing_handle_ShiftSpace
				    (self->context));
    }


    gint easySymbolInput = chewing_get_easySymbolInput(self->context);
    /**
     * Fix for space in Temporary English mode.
     */
    if (!is_shift_only) {
	/* fixed #33 first space wouldn't be outgoing */
	chewing_set_easySymbolInput(self->context, 0);
    }
    EventResponse response =
	event_process_or_ignore(!chewing_handle_Space(self->context));

    chewing_set_easySymbolInput(self->context, easySymbolInput);
    return response;
}

EventResponse self_handle_return(IBusChewingPreEdit * self, KSym kSym,
				 KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;
    ignore_when_buffer_is_empty;
    handle_log("return");

    return event_process_or_ignore(!chewing_handle_Enter(self->context));
}

EventResponse self_handle_backspace(IBusChewingPreEdit * self, KSym kSym,
				    KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;
    ignore_when_buffer_is_empty;
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
    ignore_when_buffer_is_empty;
    handle_log("escape");

    return event_process_or_ignore(!chewing_handle_Esc(self->context));
}

EventResponse self_handle_left(IBusChewingPreEdit * self, KSym kSym,
			       KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);
    absorb_when_release;
    ignore_when_buffer_is_empty;
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
    ignore_when_buffer_is_empty;
    handle_log("up");

    return event_process_or_ignore(!chewing_handle_Up(self->context));
}

EventResponse self_handle_right(IBusChewingPreEdit * self, KSym kSym,
				KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);
    absorb_when_release;
    ignore_when_buffer_is_empty;
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
    ignore_when_buffer_is_empty;
    handle_log("down");

    return event_process_or_ignore(!chewing_handle_Down(self->context));
}

EventResponse self_handle_page_up(IBusChewingPreEdit * self, KSym kSym,
				  KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;
    ignore_when_buffer_is_empty;
    handle_log("page_up");

    return event_process_or_ignore(!chewing_handle_PageUp(self->context));
}

EventResponse self_handle_page_down(IBusChewingPreEdit * self, KSym kSym,
				    KeyModifiers unmaskedMod)
{
    filter_modifiers(0);
    absorb_when_release;
    ignore_when_buffer_is_empty;
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
    {IBUS_KEY_0, IBUS_KEY_9, self_handle_num}
    ,
    {IBUS_KEY_KP_0, IBUS_KEY_KP_9, self_handle_num_keypad}
    ,
    {IBUS_KEY_Caps_Lock, IBUS_KEY_Caps_Lock, self_handle_caps_lock}
    ,
    {IBUS_KEY_Shift_L, IBUS_KEY_Shift_R, self_handle_shift}
    ,
    {IBUS_KEY_space, IBUS_KEY_space, self_handle_space}
    ,
    {IBUS_KEY_Return, IBUS_KEY_Return, self_handle_return}
    ,
    {IBUS_KEY_KP_Enter, IBUS_KEY_KP_Enter, self_handle_return}
    ,
    {IBUS_KEY_BackSpace, IBUS_KEY_BackSpace, self_handle_backspace}
    ,
    {IBUS_KEY_Delete, IBUS_KEY_Delete, self_handle_delete}
    ,
    {IBUS_KEY_Escape, IBUS_KEY_Escape, self_handle_escape}
    ,
    {IBUS_KEY_Left, IBUS_KEY_Left, self_handle_left}
    ,
    {IBUS_KEY_KP_Left, IBUS_KEY_KP_Left, self_handle_left}
    ,
    {IBUS_KEY_Up, IBUS_KEY_Up, self_handle_up}
    ,
    {IBUS_KEY_KP_Up, IBUS_KEY_KP_Up, self_handle_up}
    ,
    {IBUS_KEY_Right, IBUS_KEY_Right, self_handle_right}
    ,
    {IBUS_KEY_KP_Right, IBUS_KEY_KP_Right, self_handle_right}
    ,
    {IBUS_KEY_Down, IBUS_KEY_Down, self_handle_down}
    ,
    {IBUS_KEY_KP_Down, IBUS_KEY_KP_Down, self_handle_down}
    ,
    {IBUS_KEY_Page_Up, IBUS_KEY_Page_Up, self_handle_page_up}
    ,
    {IBUS_KEY_KP_Page_Up, IBUS_KEY_KP_Page_Up, self_handle_page_up}
    ,
    {IBUS_KEY_Page_Down, IBUS_KEY_Page_Down, self_handle_page_down}
    ,
    {IBUS_KEY_KP_Page_Down, IBUS_KEY_KP_Page_Down, self_handle_page_down}
    ,
    {IBUS_KEY_Tab, IBUS_KEY_Tab, self_handle_tab}
    ,
    {IBUS_KEY_Home, IBUS_KEY_Home, self_handle_home}
    ,
    {IBUS_KEY_KP_Home, IBUS_KEY_KP_Home, self_handle_home}
    ,
    {IBUS_KEY_End, IBUS_KEY_Home, self_handle_end}
    ,
    {IBUS_KEY_KP_End, IBUS_KEY_KP_End, self_handle_end}
    ,
    {128, G_MAXUINT, self_handle_special}
    ,
    {0, 0, self_handle_default}
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

#define process_key_debug(prompt) IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_process_key(): %s flags=%x buff_check=%d bpmf_check=%d cursor=%d total_choice=%d is_chinese=%d is_plain_zhuyin=%d",\
	prompt,	self->flags, chewing_buffer_Check(self->context),\
	bpmf_check, cursor_current, total_choice, is_chinese, is_plain_zhuyin)


/* keyCode should be converted to kSym already */
gboolean ibus_chewing_pre_edit_process_key
    (IBusChewingPreEdit * self, KSym kSym, KeyModifiers unmaskedMod) {
    IBUS_CHEWING_LOG(MSG,
		     "***** ibus_chewing_pre_edit_process_key(-,%x(%s),%x(%s))",
		     kSym, key_sym_get_name(kSym),
		     unmaskedMod, modifiers_to_string(unmaskedMod));
    gboolean bufferEmpty =
	(ibus_chewing_pre_edit_length(self) == 0) ? TRUE : FALSE;
    /* Find corresponding rule */
    EventResponse response;
    response = handle_key(kSym, unmaskedMod);

    IBUS_CHEWING_LOG(INFO,
		     "ibus_chewing_pre_edit_process_key() response=%x",
		     response);
    self->keyLast = kSym;
    switch (response) {
    case EVENT_RESPONSE_ABSORB:
	return TRUE;
    case EVENT_RESPONSE_IGNORE:
	/* Play conservatively, commit everything when unhandled key is received.
	 * Use case: In gedit, press Ctrl-F to open search UI 
	 * Better to commit every thing before the search field active.
	 */
	if (!bufferEmpty) {
	    ibus_chewing_pre_edit_force_commit(self);
	}
	return FALSE;
    default:
	break;
    }

    /* EVENT_RESPONSE_PROCESS */
    process_key_debug("After response");

    /* libchewing functions are used here to skip the check 
     * that handle_key functions perform.
     */
    if (is_plain_zhuyin && !bpmf_check) {
	if (!is_chinese){
	    /* Don't hold key in English mode */
	    return FALSE;
	}
	if (kSym == IBUS_KEY_Escape) {
	    ibus_chewing_pre_edit_clear_pre_edit(self);
	} else if (kSym == IBUS_KEY_Return && table_is_showing) {
	    /* Use Enter to select the last chosen */
	    chewing_handle_Up(self->context);
	    chewing_handle_Enter(self->context);
	} else if (!table_is_showing) {
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
    if (!chewing_get_ChiEngMode(self->context)) {
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
