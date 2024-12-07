#include "IBusChewingPreEdit.h"
#include "IBusChewingPreEdit-private.h"
#include "IBusChewingUtil.h"
#include "MakerDialogUtil.h"
#include "ibus-chewing-engine.h"
#include <chewing.h>

/**************************************
 * Methods
 */

IBusChewingPreEdit *ibus_chewing_pre_edit_new() {
    IBusChewingPreEdit *self = g_new0(IBusChewingPreEdit, 1);

    // self->iProperties = ibus_chewing_properties_new(backend, self, NULL);
    self->preEdit = g_string_sized_new(IBUS_CHEWING_MAX_BYTES);
    self->outgoing = g_string_sized_new(IBUS_CHEWING_MAX_BYTES);
    self->keyLast = 0;
    self->bpmfLen = 0;
    self->wordLen = 0;
    self->engine = NULL;

    self->context = chewing_new();
    // TODO add default mode setting
    chewing_set_ChiEngMode(self->context, CHINESE_MODE);

    self->iTable = g_object_ref_sink(ibus_chewing_lookup_table_new(self->context));
    return self;
}

void ibus_chewing_pre_edit_free(IBusChewingPreEdit *self) {
    /* properties need not be freed here */
    chewing_delete(self->context);
    g_string_free(self->preEdit, TRUE);
    g_string_free(self->outgoing, TRUE);
    ibus_lookup_table_clear(self->iTable);
    g_object_unref(self->iTable);
    g_free(self);
}

gchar *ibus_chewing_pre_edit_get_bopomofo_string(IBusChewingPreEdit *self) {
    // FIXME: libchewing should provide chewing_bopomofo_String(ctx)
    const gchar *buf = chewing_bopomofo_String_static(self->context);

    return g_strdup(buf);
}

void ibus_chewing_pre_edit_update_outgoing(IBusChewingPreEdit *self) {
    if (chewing_commit_Check(self->context)) {
        /* commit_Check=1 means new commit available */
        gchar *commitStr = chewing_commit_String(self->context);

        IBUS_CHEWING_LOG(INFO, "commitStr=|%s|\n", commitStr);
        g_string_append(self->outgoing, commitStr);

        chewing_free(commitStr);
        chewing_ack(self->context);
    }
    IBUS_CHEWING_LOG(INFO, "outgoing=|%s|\n", self->outgoing->str);
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_update_outgoing(-): return: outgoing=|%s|",
                     self->outgoing->str);
}

void ibus_chewing_pre_edit_update(IBusChewingPreEdit *self) {
    IBUS_CHEWING_LOG(DEBUG, "* ibus_chewing_pre_edit_update(-)");

    /* Make preEdit */
    gchar *bufferStr = chewing_buffer_String(self->context);
    gchar *bpmfStr = ibus_chewing_pre_edit_get_bopomofo_string(self);

    self->bpmfLen = (gint)g_utf8_strlen(bpmfStr, -1);

    g_string_assign(self->preEdit, "");
    gint i;
    gchar *cP = bufferStr;
    gunichar uniCh;

    IBUS_CHEWING_LOG(INFO,
                     "* ibus_chewing_pre_edit_update(-)  bufferStr=|%s|, "
                     "bpmfStr=|%s| bpmfLen=%d cursor=%d",
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

    chewing_free(bufferStr);
    g_free(bpmfStr);

    ibus_chewing_pre_edit_update_outgoing(self);
}

guint ibus_chewing_pre_edit_length(IBusChewingPreEdit *self) { return self->preEdit->len; }

guint ibus_chewing_pre_edit_word_length(IBusChewingPreEdit *self) { return self->wordLen; }

guint ibus_chewing_pre_edit_word_limit(IBusChewingPreEdit *self) {
    return chewing_get_maxChiSymbolLen(self->context);
}

gchar *ibus_chewing_pre_edit_get_pre_edit(IBusChewingPreEdit *self) { return self->preEdit->str; }

gchar *ibus_chewing_pre_edit_get_outgoing(IBusChewingPreEdit *self) { return self->outgoing->str; }

/* currently, ibus_chewing_pre_edit_force_commit() is called only by test cases.
 */
void ibus_chewing_pre_edit_force_commit(IBusChewingPreEdit *self) {
    IBUS_CHEWING_LOG(INFO, "ibus_chewing_pre_edit_force_commit(-) bpmf_check=%d buffer_check=%d",
                     bpmf_check, chewing_buffer_Check(self->context));

    /* Ignore the context and commit whatever in preedit buffer */
    g_string_append(self->outgoing, self->preEdit->str);
    ibus_chewing_pre_edit_clear_pre_edit(self);
    ibus_chewing_pre_edit_update(self);
}

void ibus_chewing_pre_edit_clear(IBusChewingPreEdit *self) {
    IBUS_CHEWING_LOG(INFO, "ibus_chewing_pre_edit_clear(-)");
    ibus_chewing_pre_edit_clear_outgoing(self);
    ibus_chewing_pre_edit_clear_pre_edit(self);
}

void ibus_chewing_pre_edit_clear_bopomofo(IBusChewingPreEdit *self) {
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_clear_bopomofo(-)");

    /* Esc key can close candidate list, clear bopomofo, and clear
     * the whole pre-edit buffer. Make sure it acts as we expected.
     */
    if (table_is_showing) {
        chewing_handle_Esc(self->context);
    }
    if (bpmf_check) {
        chewing_handle_Esc(self->context);
        ibus_chewing_pre_edit_update(self);
    }
}

void ibus_chewing_pre_edit_clear_pre_edit(IBusChewingPreEdit *self) {
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_clear_pre_edit(-)");

    ibus_chewing_pre_edit_clear_bopomofo(self);

    /* Save the orig Esc clean buffer state */
    gint origState = chewing_get_escCleanAllBuf(self->context);

    chewing_set_escCleanAllBuf(self->context, TRUE);

    chewing_handle_Esc(self->context);

    chewing_set_escCleanAllBuf(self->context, origState);
    ibus_chewing_pre_edit_update(self);
}

void ibus_chewing_pre_edit_clear_outgoing(IBusChewingPreEdit *self) {
    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_clear_outgoing(-)");
    g_string_assign(self->outgoing, "");
}

#define is_chinese ibus_chewing_pre_edit_get_chi_eng_mode(self)
#define is_full_shape ibus_chewing_pre_edit_get_full_half_mode(self)
gboolean ibus_chewing_pre_edit_get_chi_eng_mode(IBusChewingPreEdit *self) {
    return chewing_get_ChiEngMode(self->context) != 0;
}

gboolean ibus_chewing_pre_edit_get_full_half_mode(IBusChewingPreEdit *self) {
    return chewing_get_ShapeMode(self->context) != 0;
}

void ibus_chewing_pre_edit_set_chi_eng_mode(IBusChewingPreEdit *self, gboolean chineseMode) {
    /* Clear bopomofo when toggling Chi-Eng Mode */
    if (!chineseMode && is_chinese && bpmf_check) {
        ibus_chewing_pre_edit_clear_bopomofo(self);
    }
    chewing_set_ChiEngMode(self->context, (chineseMode) ? 1 : 0);
}

void ibus_chewing_pre_edit_set_full_half_mode(IBusChewingPreEdit *self, gboolean fullShapeMode) {
    if (is_chinese && bpmf_check) {
        /* Clear bopomofo when toggling Full-Half Mode */
        ibus_chewing_pre_edit_clear_bopomofo(self);
    }
    chewing_set_ShapeMode(self->context, (fullShapeMode) ? 1 : 0);
}

/**************************************
 * ibus_chewing_pre_edit key processing
 */
KSym self_key_sym_fix(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    IBusChewingEngine *engine = IBUS_CHEWING_ENGINE(self->engine);
    gchar caseConversionMode = ibus_chewing_engine_get_default_english_case(engine);
    gchar toggleChinese = ibus_chewing_engine_get_chinese_english_toggle_key(engine);

    if (toggleChinese != 'c') {

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

EventResponse self_handle_key_sym_default(IBusChewingPreEdit *self, KSym kSym,
                                          KeyModifiers unmaskedMod) {
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

    IBUS_CHEWING_LOG(DEBUG, "* self_handle_key_sym_default(): new kSym %x(%s), %x(%s)", fixedKSym,
                     key_sym_get_name(fixedKSym), unmaskedMod, modifiers_to_string(unmaskedMod));
    gint ret = chewing_handle_Default(self->context, fixedKSym);

    /* Handle quick commit */
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

    IBUS_CHEWING_LOG(DEBUG, "self_handle_key_sym_default() ret=%d response=%d", ret, response);
    /* Restore easySymbolInput */
    chewing_set_easySymbolInput(self->context, easySymbolInput);
    return response;
}

/* Return FALSE if the key should not be processed with input method */
EventResponse self_handle_num(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(IBUS_SHIFT_MASK | IBUS_CONTROL_MASK);
    if ((maskedMod == 0) && !is_full_shape && !is_chinese) {
        /* Users treat English Sub-mode as IM Disabled,
         * So key strokes should be passed to client directly. Github 144.
         */
        ignore_when_buffer_is_empty_and_table_not_showing;
    }
    ignore_when_release;
    handle_log("num");

    if (is_ctrl_only) {
        return event_process_or_ignore(!chewing_handle_CtrlNum(self->context, kSym));
    }
    /* maskedMod = 0 */
    return self_handle_key_sym_default(self, kSym, unmaskedMod);
}

EventResponse self_handle_num_keypad(IBusChewingPreEdit *self, KSym kSym,
                                     KeyModifiers unmaskedMod) {
    filter_modifiers(IBUS_SHIFT_MASK | IBUS_CONTROL_MASK);
    if ((maskedMod == 0) && (!is_full_shape)) {
        /* Let libchewing handles Keypad keys only when needed.
         * Otherwise it might cause some issues. Github 144.
         */
        ignore_when_buffer_is_empty_and_table_not_showing;
    }
    ignore_when_release;
    handle_log("num_keypad");

    KSym kSymEquiv = key_sym_KP_to_normal(kSym);

    if ((maskedMod != 0) && (!is_shift_only) && (!is_ctrl_only)) {
        return EVENT_RESPONSE_IGNORE;
    }

    if (is_ctrl_only) {
        return event_process_or_ignore(!chewing_handle_CtrlNum(self->context, kSymEquiv));
    }

    /* maskedMod = 0 */
    /* switch to eng-mode temporary */
    gint origChiEngMode = chewing_get_ChiEngMode(self->context);

    ibus_chewing_pre_edit_set_chi_eng_mode(self, FALSE);

    EventResponse response = self_handle_key_sym_default(self, kSymEquiv, unmaskedMod);

    chewing_set_ChiEngMode(self->context, origChiEngMode);

    return response;
}

EventResponse self_handle_caps_lock(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(IBUS_LOCK_MASK);

    IBusChewingEngine *engine = IBUS_CHEWING_ENGINE(self->engine);
    gchar toggleChinese = ibus_chewing_engine_get_chinese_english_toggle_key(engine);

    if (toggleChinese != 'c') {
        /* Ignore the Caps Lock event when it does not toggle Chinese */
        return EVENT_RESPONSE_IGNORE;
    }

    absorb_when_release;
    handle_log("caps_lock");

    /* Clear bopomofo when toggling Chi-Eng Mode */
    if (is_chinese && bpmf_check) {
        ibus_chewing_pre_edit_clear_bopomofo(self);
    }

    return event_process_or_ignore(!chewing_handle_Capslock(self->context));
}

EventResponse self_handle_shift_left(IBusChewingPreEdit *self, KSym kSym,
                                     KeyModifiers unmaskedMod) {
    filter_modifiers(IBUS_SHIFT_MASK);
    handle_log("shift_left");

    IBusChewingEngine *engine = IBUS_CHEWING_ENGINE(self->engine);
    gchar toggleChinese = ibus_chewing_engine_get_chinese_english_toggle_key(engine);

    if (toggleChinese != 's' && toggleChinese != 'l') {
        return EVENT_RESPONSE_IGNORE;
    }

    if (!event_is_released(unmaskedMod)) {
        return EVENT_RESPONSE_IGNORE;
    }

    /* keyLast != Shift means Shift is just part of combination,
     * thus should not be recognized as single Shift key
     */
    if (self->keyLast != IBUS_KEY_Shift_L && self->keyLast != IBUS_KEY_Shift_R) {
        return EVENT_RESPONSE_IGNORE;
    }

    ibus_chewing_pre_edit_toggle_chi_eng_mode(self);
    ibus_chewing_engine_notify_chinese_english_mode_change(IBUS_CHEWING_ENGINE(self->engine));
    return EVENT_RESPONSE_IGNORE;
}

EventResponse self_handle_shift_right(IBusChewingPreEdit *self, KSym kSym,
                                      KeyModifiers unmaskedMod) {
    filter_modifiers(IBUS_SHIFT_MASK);
    handle_log("shift_right");

    IBusChewingEngine *engine = IBUS_CHEWING_ENGINE(self->engine);
    gchar toggleChinese = ibus_chewing_engine_get_chinese_english_toggle_key(engine);

    if (toggleChinese != 's' && toggleChinese != 'r') {
        return EVENT_RESPONSE_IGNORE;
    }

    if (!event_is_released(unmaskedMod)) {
        return EVENT_RESPONSE_IGNORE;
    }

    /* keyLast != Shift means Shift is just part of combination,
     * thus should not be recognized as single Shift key
     */
    if (self->keyLast != IBUS_KEY_Shift_L && self->keyLast != IBUS_KEY_Shift_R) {
        return EVENT_RESPONSE_IGNORE;
    }

    ibus_chewing_pre_edit_toggle_chi_eng_mode(self);
    ibus_chewing_engine_notify_chinese_english_mode_change(IBUS_CHEWING_ENGINE(self->engine));
    return EVENT_RESPONSE_IGNORE;
}

EventResponse self_handle_page_up(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("page_up");

    int currentPage = chewing_cand_CurrentPage(self->context);
    int hasNext = chewing_cand_list_has_next(self->context);
    int hasPrev = chewing_cand_list_has_prev(self->context);

    if (table_is_showing && (currentPage == 0)) {
        if (hasNext == 1 || hasPrev == 1) {
            return event_process_or_ignore(!chewing_handle_Down(self->context));
        }
    }

    return event_process_or_ignore(!chewing_handle_PageUp(self->context));
}

EventResponse self_handle_page_down(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("page_down");

    int totalPage = chewing_cand_TotalPage(self->context);
    int currentPage = chewing_cand_CurrentPage(self->context);

    if (table_is_showing && (currentPage == totalPage - 1)) {
        return event_process_or_ignore(!chewing_handle_Down(self->context));
    }

    return event_process_or_ignore(!chewing_handle_PageDown(self->context));
}

EventResponse self_handle_space(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(IBUS_SHIFT_MASK | IBUS_CONTROL_MASK);

    if (!is_shift_only && !is_chinese && !is_full_shape) {
        /* Let libchewing handles Keypad keys only when needed.
         * Otherwise it might cause some issues. Github 144.
         */
        ignore_when_buffer_is_empty_and_table_not_showing;
    }

    ignore_when_release;
    handle_log("space");

    if (is_shift_only) {
        handle_log("Shift+Space");
        chewing_handle_ShiftSpace(self->context);
        ibus_chewing_engine_notify_fullwidth_mode_change(IBUS_CHEWING_ENGINE(self->engine));
        return EVENT_RESPONSE_PROCESS;
    }

    return event_process_or_ignore(!chewing_handle_Space(self->context));
}

EventResponse self_handle_return(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("return");

    if (table_is_showing) {
        int cursorInPage = ibus_lookup_table_get_cursor_in_page(self->iTable);

        cursorInPage = chewing_get_selKey(self->context)[cursorInPage];
        return self_handle_key_sym_default(self, cursorInPage, unmaskedMod);
    }

    EventResponse response = event_process_or_ignore(!chewing_handle_Enter(self->context));

    /* Handle quick commit */
    ibus_chewing_pre_edit_update_outgoing(self);

    return response;
}

EventResponse self_handle_backspace(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    absorb_when_release; // Triggers focus-out and focus-in on ignore, so use
    // absorb.
    handle_log("backspace");

    return event_process_or_ignore(!chewing_handle_Backspace(self->context));
}

EventResponse self_handle_delete(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("delete");

    return event_process_or_ignore(!chewing_handle_Del(self->context));
}

EventResponse self_handle_escape(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("escape");

    return event_process_or_ignore(!chewing_handle_Esc(self->context));
}

EventResponse self_handle_left(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(IBUS_SHIFT_MASK);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("left");

    if (is_shift_only) {
        return event_process_or_ignore(!chewing_handle_ShiftLeft(self->context));
    }

    if (table_is_showing) {
        IBusChewingEngine *engine = IBUS_CHEWING_ENGINE(self->engine);
        if (!ibus_chewing_engine_use_vertical_lookup_table(engine)) {
            /* horizontal look-up table */
            int pos = ibus_lookup_table_get_cursor_in_page(self->iTable);

            if (pos) {
                ibus_lookup_table_cursor_up(self->iTable);
                return EVENT_RESPONSE_ABSORB;
            }
        }

        return self_handle_page_up(self, kSym, unmaskedMod);
    }

    return event_process_or_ignore(!chewing_handle_Left(self->context));
}

EventResponse self_handle_up(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("up");

    if (table_is_showing) {
        IBusChewingEngine *engine = IBUS_CHEWING_ENGINE(self->engine);
        if (!ibus_chewing_engine_use_vertical_lookup_table(engine)) {
            /* vertical look-up table */
            int pos = ibus_lookup_table_get_cursor_in_page(self->iTable);

            if (pos) {
                ibus_lookup_table_cursor_up(self->iTable);
                return EVENT_RESPONSE_ABSORB;
            }
        }

        return self_handle_page_up(self, kSym, unmaskedMod);
    }

    return event_process_or_ignore(!chewing_handle_Up(self->context));
}

EventResponse self_handle_right(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(IBUS_SHIFT_MASK);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("right");

    if (is_shift_only) {
        return event_process_or_ignore(!chewing_handle_ShiftRight(self->context));
    }

    if (table_is_showing) {
        IBusChewingEngine *engine = IBUS_CHEWING_ENGINE(self->engine);
        if (!ibus_chewing_engine_use_vertical_lookup_table(engine)) {
            /* horizontal look-up table */
            int numberCand = ibus_lookup_table_get_number_of_candidates(self->iTable);
            int cursorInPage = ibus_lookup_table_get_cursor_in_page(self->iTable) + 1;
            if (cursorInPage != numberCand) {
                ibus_lookup_table_cursor_down(self->iTable);
                return EVENT_RESPONSE_ABSORB;
            }
        }

        return self_handle_page_down(self, kSym, unmaskedMod);
    }

    return event_process_or_ignore(!chewing_handle_Right(self->context));
}

EventResponse self_handle_down(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("down");

    if (table_is_showing) {
        IBusChewingEngine *engine = IBUS_CHEWING_ENGINE(self->engine);
        if (!ibus_chewing_engine_use_vertical_lookup_table(engine)) {
            /* vertical look-up table */
            int numberCand = ibus_lookup_table_get_number_of_candidates(self->iTable);
            int cursorInPage = ibus_lookup_table_get_cursor_in_page(self->iTable) + 1;
            if (cursorInPage != numberCand) {
                ibus_lookup_table_cursor_down(self->iTable);
                return EVENT_RESPONSE_ABSORB;
            }
        }

        /* horizontal look-up table */
        return self_handle_page_down(self, kSym, unmaskedMod);
    }

    return event_process_or_ignore(!chewing_handle_Down(self->context));
}

EventResponse self_handle_tab(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("tab");

    return event_process_or_ignore(!chewing_handle_Tab(self->context));
}

EventResponse self_handle_home(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("home");

    return event_process_or_ignore(!chewing_handle_Home(self->context));
}

EventResponse self_handle_end(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(0);
    ignore_when_buffer_is_empty_and_table_not_showing;
    ignore_when_release;
    handle_log("end");

    return event_process_or_ignore(!chewing_handle_End(self->context));
}

EventResponse self_handle_special([[maybe_unused]] IBusChewingPreEdit *self,
                                  [[maybe_unused]] KSym kSym,
                                  [[maybe_unused]] KeyModifiers unmaskedMod) {
    /* KSym >=128 is special key, which IM ignore. */
    IBUS_CHEWING_LOG(MSG, "ignore special key");
    return EVENT_RESPONSE_IGNORE;
}

EventResponse self_handle_default(IBusChewingPreEdit *self, KSym kSym, KeyModifiers unmaskedMod) {
    filter_modifiers(IBUS_SHIFT_MASK);
    ignore_when_release;
    handle_log("default");

    return self_handle_key_sym_default(self, kSym, unmaskedMod);
}

KeyHandlingRule keyHandlingRules[] = {
    {IBUS_KEY_0, IBUS_KEY_9, self_handle_num},

    {IBUS_KEY_KP_0, IBUS_KEY_KP_9, self_handle_num_keypad},

    {IBUS_KEY_Caps_Lock, IBUS_KEY_Caps_Lock, self_handle_caps_lock},

    {IBUS_KEY_Shift_L, IBUS_KEY_Shift_L, self_handle_shift_left},

    {IBUS_KEY_Shift_R, IBUS_KEY_Shift_R, self_handle_shift_right},

    {IBUS_KEY_space, IBUS_KEY_space, self_handle_space},

    {IBUS_KEY_Return, IBUS_KEY_Return, self_handle_return},

    {IBUS_KEY_KP_Enter, IBUS_KEY_KP_Enter, self_handle_return},

    {IBUS_KEY_BackSpace, IBUS_KEY_BackSpace, self_handle_backspace},

    {IBUS_KEY_Delete, IBUS_KEY_Delete, self_handle_delete},

    {IBUS_KEY_KP_Delete, IBUS_KEY_KP_Delete, self_handle_delete},

    {IBUS_KEY_Escape, IBUS_KEY_Escape, self_handle_escape},

    {IBUS_KEY_Left, IBUS_KEY_Left, self_handle_left},

    {IBUS_KEY_KP_Left, IBUS_KEY_KP_Left, self_handle_left},

    {IBUS_KEY_Up, IBUS_KEY_Up, self_handle_up},

    {IBUS_KEY_KP_Up, IBUS_KEY_KP_Up, self_handle_up},

    {IBUS_KEY_Right, IBUS_KEY_Right, self_handle_right},

    {IBUS_KEY_KP_Right, IBUS_KEY_KP_Right, self_handle_right},

    {IBUS_KEY_Down, IBUS_KEY_Down, self_handle_down},

    {IBUS_KEY_KP_Down, IBUS_KEY_KP_Down, self_handle_down},

    {IBUS_KEY_Page_Up, IBUS_KEY_Page_Up, self_handle_page_up},

    {IBUS_KEY_KP_Page_Up, IBUS_KEY_KP_Page_Up, self_handle_page_up},

    {IBUS_KEY_Page_Down, IBUS_KEY_Page_Down, self_handle_page_down},

    {IBUS_KEY_KP_Page_Down, IBUS_KEY_KP_Page_Down, self_handle_page_down},

    {IBUS_KEY_Tab, IBUS_KEY_Tab, self_handle_tab},

    {IBUS_KEY_Home, IBUS_KEY_Home, self_handle_home},

    {IBUS_KEY_KP_Home, IBUS_KEY_KP_Home, self_handle_home},

    {IBUS_KEY_End, IBUS_KEY_End, self_handle_end},

    {IBUS_KEY_KP_End, IBUS_KEY_KP_End, self_handle_end},

    {IBUS_KP_Multiply, IBUS_KP_Divide, self_handle_num_keypad},

    {32, 127, self_handle_default},
    /* we need only printable ascii characters (32~127) and keys
     * like enter, esc, backspace ... etc. The rest can be ignored.
     */
    {0, G_MAXUINT, self_handle_special},
};

static KeyHandlingRule *self_key_sym_find_key_handling_rule(KSym kSym) {
    gint i;

    for (i = 0; keyHandlingRules[i].kSymLower != 0; i++) {
        if ((keyHandlingRules[i].kSymLower <= kSym) && (kSym <= keyHandlingRules[i].kSymUpper)) {
            return &(keyHandlingRules[i]);
        }
    }
    return &(keyHandlingRules[i]);
}

#define handle_key(kSym, unmaskedMod)                                                              \
    (self_key_sym_find_key_handling_rule(kSym))->keyFunc(self, kSym, unmaskedMod)

#define process_key_debug(prompt)                                                                  \
    IBUS_CHEWING_LOG(DEBUG,                                                                        \
                     "ibus_chewing_pre_edit_process_key(): %s flags=%x "                           \
                     "buff_check=%d bpmf_check=%d cursor=%d total_choice=%d "                      \
                     "is_chinese=%d is_full_shape=%d",                                             \
                     prompt, self->flags, chewing_buffer_Check(self->context), bpmf_check,         \
                     cursor_current, total_choice, is_chinese, is_full_shape)

gboolean is_shift_key(KSym kSym) { return kSym == IBUS_KEY_Shift_L || kSym == IBUS_KEY_Shift_R; }

gboolean is_shift_toggle(KSym keyLast, KSym kSym, KeyModifiers unmaskedMod) {
    KeyModifiers maskedMod = modifiers_mask(unmaskedMod);

    if (is_shift_only && is_shift_key(keyLast) && is_shift_key(kSym)) {
        return TRUE;
    }
    return FALSE;
}

/* keyCode should be converted to kSym already */
gboolean ibus_chewing_pre_edit_process_key(IBusChewingPreEdit *self, KSym kSym,
                                           KeyModifiers unmaskedMod) {
    IBUS_CHEWING_LOG(INFO, "***** ibus_chewing_pre_edit_process_key(-,%x(%s),%x(%s))", kSym,
                     key_sym_get_name(kSym), unmaskedMod, modifiers_to_string(unmaskedMod));
    process_key_debug("Before response");

    /* Find corresponding rule */
    EventResponse response;

    if (!is_full_shape && !is_chinese &&
        !(kSym == IBUS_KEY_space && unmaskedMod == IBUS_SHIFT_MASK) &&
        !(kSym == IBUS_KEY_Caps_Lock && is_capslock) &&
        !is_shift_toggle(self->keyLast, kSym, unmaskedMod)) {
        /* Users treat English Sub-mode as IM Disabled,
         * So key strokes should be passed to client directly. Github 144.
         */
        if (((ibus_chewing_pre_edit_length(self) == 0)) &&
            !((self->flags & FLAG_TABLE_SHOW) == FLAG_TABLE_SHOW)) {
            self->keyLast = kSym;
            return FALSE;
        };
    }

    response = handle_key(kSym, unmaskedMod);

    IBUS_CHEWING_LOG(DEBUG, "ibus_chewing_pre_edit_process_key() response=%x", response);
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

    ibus_chewing_pre_edit_update(self);

    guint candidateCount = ibus_chewing_lookup_table_update(self->iTable, self->context);

    IBUS_CHEWING_LOG(INFO, "ibus_chewing_pre_edit_process_key() candidateCount=%d", candidateCount);

    if (candidateCount) {
        ibus_chewing_pre_edit_set_flag(self, FLAG_TABLE_SHOW);
    } else {
        ibus_chewing_pre_edit_clear_flag(self, FLAG_TABLE_SHOW);
    }
    return TRUE;
}

KSym ibus_chewing_pre_edit_key_code_to_key_sym(IBusChewingPreEdit *self, KSym keySym, guint keyCode,
                                               KeyModifiers unmaskedMod) {
    KSym kSym = keySym;

    if (!is_chinese) {
        /* English mode, pass as-is */
        return kSym;
    }

    IBusChewingEngine *engine = IBUS_CHEWING_ENGINE(self->engine);
    if (!ibus_chewing_engine_use_system_layout(engine)) {
        /* Use en_US keyboard layout */
        /* ibus_keymap_lookup_key_sym treats keycode >= 256 */
        /* as IBUS_VoidSymbol */
        kSym = ibus_keymap_lookup_keysym(ibus_keymap_get("us"), keyCode, unmaskedMod);
        if (kSym == IBUS_VoidSymbol) {
            /* Restore key_sym */
            kSym = keySym;
        }
    }

    return kSym;
}
