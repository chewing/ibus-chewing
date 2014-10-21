#include <stdlib.h>
#include <ctype.h>
#include "IBusChewingUtil.h"
#include "IBusChewingProperties.h"
#include "IBusChewingPreEdit.h"

/* ibus_chewing_bopomofo_check==1 means word is completed */
#define is_word_completed(self) ibus_chewing_bopomofo_check(self->context)

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

    return self;
}

void ibus_chewing_pre_edit_free(IBusChewingPreEdit * self)
{
    /* properties need not be freed here */
    chewing_delete(self->context);
    g_string_free(self->preEdit, TRUE);
    g_string_free(self->outgoing, TRUE);
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
    printf("** bufferStr=|%s|, bpmfStr=|%s| count=%d cursor=%d\n", bufferStr,
	   bpmfStr, count, chewing_cursor_Current(self->context));
    for (i = 0; i < chewing_buffer_Len(self->context) && cP != NULL; i++) {
	if (i == chewing_cursor_Current(self->context)) {
	    /* Insert bopomofo string */
	    g_string_append(self->preEdit, bpmfStr);
	}
	uniCh = g_utf8_get_char(cP);
	g_string_append_unichar(self->preEdit, uniCh);
	cP = g_utf8_next_char(cP);
    }
    if (chewing_buffer_Len(self->context) <=
	chewing_cursor_Current(self->context)) {
	g_string_append(self->preEdit, bpmfStr);
    }
    self->wordLen = i + count;
    g_free(bufferStr);
    g_free(bpmfStr);

    /* Make outgoing */
    IBUS_CHEWING_LOG(INFO,
	    "ibus_chewing_pre_edit_update(-): preEdit=|%s| commit_check=%d outgoing=|%s|",
	    self->preEdit->str,
	    chewing_commit_Check(self->context),
	    self->outgoing->str);
    if (chewing_commit_Check(self->context)) {
	/* commit_Check=1 means new commit available */
	gchar *commitStr = chewing_commit_String(self->context);
	printf("commitStr=|%s|\n", commitStr);
	g_string_append(self->outgoing, commitStr);
	g_free(commitStr);
    }
    IBUS_CHEWING_LOG(INFO,
		     "ibus_chewing_pre_edit_update(-): outgoing=|%s|",
		     self->preEdit->str,
		     chewing_commit_Check(self->context),
		     self->outgoing->str);
    printf("preEdit=|%s| outgoing=|%s|\n", self->preEdit->str,
	   self->outgoing->str);
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
		     ibus_chewing_bopomofo_check(self->context),
		     chewing_buffer_Check(self->context));

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

typedef enum {
    EVENT_RESPONSE_PROCESS = 0,	/* Event process by IM */
    EVENT_RESPONSE_ABSORB,	/* Event throw away by IM (e.g. Release event) */
    EVENT_RESPONSE_IGNORE,	/* Event that should be passed to application, but not process by IM */
    EVENT_RESPONSE_UNDECIDED,
} EventResponse;

typedef EventResponse(*KeyHandlingFunc) (IBusChewingPreEdit * self,
					 KSym kSym,
					 KeyModifiers unmaskedMod);
typedef struct {
    KSym kSymLower;
    KSym kSymUpper;
    KeyHandlingFunc keyFunc;
} KeyHandlingRule;

#define modifiers_mask(unmaskedMod) unmaskedMod & (IBUS_SHIFT_MASK | IBUS_CONTROL_MASK | IBUS_MOD1_MASK)

#define event_is_released(unmaskedMod) ((unmaskedMod & IBUS_RELEASE_MASK) !=0 )
#define is_shift (maskedMod == IBUS_SHIFT_MASK)
#define is_ctrl (maskedMod == IBUS_CONTROL_MASK)
#define is_chinese(self) (chewing_get_ChiEngMode(self->context)!=0)
#define buffer_is_empty (ibus_chewing_pre_edit_is_empty(self))
#define filter_modifiers(mask) KeyModifiers maskedMod = modifiers_mask(unmaskedMod); \
    if (maskedMod & (~mask)){ return EVENT_RESPONSE_IGNORE; } \
    if (maskedMod == (IBUS_SHIFT_MASK | IBUS_CONTROL_MASK)){ return EVENT_RESPONSE_IGNORE; }

#define event_process_or_ignore(cond) (cond) ? EVENT_RESPONSE_PROCESS: EVENT_RESPONSE_IGNORE

EventResponse self_handle_key_sym_default(IBusChewingPreEdit * self,
					  KSym kSym,
					  KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);

    /* Seem like we need to disable easy symbol temporarily 
     * otherwise the key won't process
     */
    gint easySymbolInput = chewing_get_easySymbolInput(self->context);
    if (maskedMod != IBUS_SHIFT_MASK) {
	chewing_set_easySymbolInput(self->context, 0);
    }
    EventResponse response = EVENT_RESPONSE_UNDECIDED;

    if (!is_chinese(self)
	&& ibus_chewing_pre_edit_get_property_boolean(self,
						      "force-lowercase-english"))
    {
	/* English mode and force-lower-case-engligh=TRUE */
	if (isupper(kSym) && !is_shift) {
	    kSym = tolower(kSym);
	} else if (islower(kSym) && is_shift) {
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
		     "self_handle_key_sym_default() ret=%d response=%d\n",
		     ret, response);
    /* Restore easySymbolInput */
    chewing_set_easySymbolInput(self->context, easySymbolInput);
    return response;
}

/* Return FALSE if the key should not be processed with input method */
EventResponse self_handle_num(IBusChewingPreEdit * self, KSym kSym,
			      KeyModifiers unmaskedMod)
{
    IBUS_CHEWING_LOG(INFO,
		     "* self_handle_num(-,%x(%s),%x(%s))",
		     kSym, key_sym_get_name(kSym), unmaskedMod,
		     modifiers_to_string(unmaskedMod));
    filter_modifiers(IBUS_SHIFT_MASK | IBUS_CONTROL_MASK);

    if (event_is_released(unmaskedMod)) {
	return EVENT_RESPONSE_ABSORB;
    }

    if (is_ctrl) {
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

    if ((maskedMod != 0) && (!is_shift) && (!is_ctrl)) {
	return EVENT_RESPONSE_IGNORE;
    }

    if (event_is_released(unmaskedMod)) {
	return EVENT_RESPONSE_ABSORB;
    }

    KSym kSymFinal = kSym;
    KSym kSymEquiv = key_sym_KP_to_normal(kSym);
    gboolean numpadIsAlwaysNum =
	ibus_chewing_pre_edit_get_property_boolean(self,
						   "numpad-always-number");

    if (numpadIsAlwaysNum) {
	kSymFinal = kSymEquiv;
    }
    if (is_ctrl) {
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

    if (event_is_released(unmaskedMod)) {
	return EVENT_RESPONSE_ABSORB;
    }
#if !CHEWING_CHECK_VERSION(0,4,0)
    /* When Chi->Eng with incomplete character */
    if (is_chinese(self) && !is_word_completed(self)) {
	ibus_chewing_pre_edit_force_commit(self);
    }
#endif
    return
	event_process_or_ignore(!chewing_handle_Capslock(self->context));
}

EventResponse self_handle_shift(IBusChewingPreEdit * self, KSym kSym,
				KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    gboolean shiftIsToggleChinese =
	ibus_chewing_pre_edit_get_property_boolean(self,
						   "shift-toggle-chinese");

    if (!shiftIsToggleChinese) {
	return EVENT_RESPONSE_IGNORE;
    }

    if (!event_is_released(unmaskedMod)) {
	return EVENT_RESPONSE_ABSORB;
    }
#if !CHEWING_CHECK_VERSION(0,4,0)
    /* When Chi->Eng with incomplete character */
    if (is_chinese(self) && !is_word_completed(self)) {
	ibus_chewing_pre_edit_force_commit(self);
    }
#endif
    return
	event_process_or_ignore(!chewing_handle_Capslock(self->context));
}

EventResponse self_handle_space(IBusChewingPreEdit * self, KSym kSym,
				KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK | IBUS_CONTROL_MASK);

    if (event_is_released(unmaskedMod)) {
	return EVENT_RESPONSE_ABSORB;
    }

    if (is_shift) {
	return
	    event_process_or_ignore(!chewing_handle_ShiftSpace
				    (self->context));
    }

    gboolean plainChewing =
	ibus_chewing_pre_edit_get_property_boolean(self, "plain-zhuyin");

    if (plainChewing) {
	if (chewing_cand_TotalChoice(self->context) == 0) {
	    return
		event_process_or_ignore(!chewing_handle_Space
					(self->context));
	}
    }

    gint easySymbolInput = chewing_get_easySymbolInput(self->context);
    /**
     * Fix for space in Temporary English mode.
     */
    if (!is_shift) {
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

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    if (unmaskedMod & IBUS_RELEASE_MASK) {
	return EVENT_RESPONSE_ABSORB;
    }

    return event_process_or_ignore(!chewing_handle_Enter(self->context));
}

EventResponse self_handle_backspace(IBusChewingPreEdit * self, KSym kSym,
				 KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    if (unmaskedMod & IBUS_RELEASE_MASK) {
	return EVENT_RESPONSE_ABSORB;
    }

    return event_process_or_ignore(!chewing_handle_Backspace(self->context));
}

EventResponse self_handle_delete(IBusChewingPreEdit * self, KSym kSym,
	KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    if (unmaskedMod & IBUS_RELEASE_MASK) {
	return EVENT_RESPONSE_ABSORB;
    }

    return event_process_or_ignore(!chewing_handle_Del(self->context));
}

EventResponse self_handle_escape(IBusChewingPreEdit * self, KSym kSym,
				 KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    return event_process_or_ignore(!chewing_handle_Esc(self->context));
}

EventResponse self_handle_left(IBusChewingPreEdit * self, KSym kSym,
			       KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);
    
    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    if (is_shift) {
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

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    return event_process_or_ignore(!chewing_handle_Up(self->context));
}

EventResponse self_handle_right(IBusChewingPreEdit * self, KSym kSym,
				KeyModifiers unmaskedMod)
{
    filter_modifiers(IBUS_SHIFT_MASK);

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    if (is_shift) {
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

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    return event_process_or_ignore(!chewing_handle_Down(self->context));
}

EventResponse self_handle_page_up(IBusChewingPreEdit * self, KSym kSym,
				  KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    return event_process_or_ignore(!chewing_handle_PageUp(self->context));
}

EventResponse self_handle_page_down(IBusChewingPreEdit * self, KSym kSym,
				    KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    return
	event_process_or_ignore(!chewing_handle_PageDown(self->context));
}

EventResponse self_handle_tab(IBusChewingPreEdit * self, KSym kSym,
			      KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    return event_process_or_ignore(!chewing_handle_Tab(self->context));
}

EventResponse self_handle_home(IBusChewingPreEdit * self, KSym kSym,
			       KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

    return event_process_or_ignore(!chewing_handle_Home(self->context));
}

EventResponse self_handle_end(IBusChewingPreEdit * self, KSym kSym,
			      KeyModifiers unmaskedMod)
{
    filter_modifiers(0);

    if (buffer_is_empty){
	return EVENT_RESPONSE_IGNORE;
    }

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

    IBUS_CHEWING_LOG(INFO,
		     "* self_handle_default(-,%x(%s),%x(%s))",
		     kSym, key_sym_get_name(kSym), unmaskedMod,
		     modifiers_to_string(unmaskedMod));
    filter_modifiers(IBUS_SHIFT_MASK);

    if (unmaskedMod & IBUS_RELEASE_MASK) {
	return EVENT_RESPONSE_ABSORB;
    }

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
    return NULL;
}

#if 0
gboolean ibus_chewing_pre_edit_is_leaving_chinese(IBusChewingPreEdit *
						  self, KSym kSym)
{
    if (!is_chinese(self))
	return FALSE;
    gboolean numpadIsAlwaysNum =
	ibus_chewing_pre_edit_get_property_boolean(self,
						   "numpad-always-number");
    gboolean shiftIsToggleChinese =
	ibus_chewing_pre_edit_get_property_boolean(self,
						   "shift-toggle-chinese");
    switch (kSym) {
    case IBUS_KEY_Caps_Lock:
	return TRUE;
    case IBUS_Shift_L:
    case IBUS_Shift_R:
	if (shiftIsToggleChinese) {
	    return TRUE;
	}
	break;
    default:
	if (numpadIsAlwaysNum && (key_sym_KP_to_normal(kSym) != 0)) {
	    /* Numpad key, and numpadIsAlwaysNum is on */
	    return TRUE;
	}
	break;
    }
    return FALSE;
}

#endif

/* keyCode should be converted to kSym already */
gboolean ibus_chewing_pre_edit_process_key
    (IBusChewingPreEdit * self, KSym kSym, KeyModifiers unmaskedMod) {
    IBUS_CHEWING_LOG(MSG,
		     "* ibus_chewing_pre_edit_process_key(-,%x(%s),%x(%s))",
		     kSym, key_sym_get_name(kSym),
		     unmaskedMod, modifiers_to_string(unmaskedMod));
    gboolean bufferEmpty =
	(ibus_chewing_pre_edit_length(self) == 0) ? TRUE : FALSE;
//    self_key_preprocess(self, kSym);
    /* Find corresponding rule */
    KeyHandlingRule *rule = self_key_sym_find_key_handling_rule(kSym);
    EventResponse response;
    if (rule == NULL) {
	/* default */
	response = self_handle_default(self, kSym, unmaskedMod);
    } else {
	response = rule->keyFunc(self, kSym, unmaskedMod);
    }
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
    ibus_chewing_pre_edit_update(self);
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

