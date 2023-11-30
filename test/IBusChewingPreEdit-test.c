#include <string.h>
#include <glib.h>
#include "IBusChewingPreEdit.h"
#include "IBusChewingPreEdit-private.h"
#include "IBusChewingUtil.h"
#ifdef USE_GSETTINGS
#    include "GSettingsBackend.h"
#elif defined USE_GCONF2
#    include "GConf2Backend.h"
#endif
#include "MakerDialogUtil.h"
#include "test-util.h"
#define TEST_RUN_THIS(f) add_test_case("IBusChewingPreEdit", f)
#define TEST_CASE_INIT() reset_properties_default(self);\
    ibus_chewing_pre_edit_clear(self);\
    ibus_chewing_pre_edit_set_full_half_mode(self,FALSE);\
    ibus_chewing_pre_edit_set_chi_eng_mode(self,TRUE)

static IBusChewingPreEdit *self = NULL;

/*== Utility functions start ==*/
#define assert_pre_edit_substring(needle, begin, length) assert_substring(ibus_chewing_pre_edit_get_pre_edit(self), needle, begin, length)



void assert_outgoing_pre_edit(const gchar * outgoing, const gchar * pre_edit)
{
    g_assert_cmpstr(outgoing, ==, ibus_chewing_pre_edit_get_outgoing(self));
    g_assert_cmpstr(pre_edit, ==, ibus_chewing_pre_edit_get_pre_edit(self));
}

void assert_substring(const gchar * haystack, const gchar * needle,
                      gint begin, gint length)
{
    gchar *subStr = g_utf8_substring(haystack, begin, begin + length);

    g_assert_cmpstr(subStr, ==, needle);
    g_free(subStr);
}

void key_press_from_key_sym(KSym keySym, KeyModifiers modifiers)
{
    switch (keySym) {
    case IBUS_KEY_Shift_L:
    case IBUS_KEY_Shift_R:
        ibus_chewing_pre_edit_process_key(self, keySym, modifiers);
        ibus_chewing_pre_edit_process_key(self, keySym,
                                          modifiers | IBUS_RELEASE_MASK |
                                          IBUS_SHIFT_MASK);
        break;
    default:
        if (modifiers & IBUS_SHIFT_MASK) {
            ibus_chewing_pre_edit_process_key(self, IBUS_KEY_Shift_L, 0);
        }
        ibus_chewing_pre_edit_process_key(self, keySym, modifiers);
        ibus_chewing_pre_edit_process_key(self, keySym,
                                          modifiers | IBUS_RELEASE_MASK);
        if (modifiers & IBUS_SHIFT_MASK) {
            ibus_chewing_pre_edit_process_key(self, IBUS_KEY_Shift_L,
                                              IBUS_SHIFT_MASK |
                                              IBUS_RELEASE_MASK);
        }
        break;

    }

    printf
        ("key_press_from_key_sym(%x(%s),%x), buffer=|%s| outgoing=|%s|\n",
         keySym, key_sym_get_name(keySym), modifiers,
         ibus_chewing_pre_edit_get_pre_edit(self),
         ibus_chewing_pre_edit_get_outgoing(self));
}

void key_press_from_string(const gchar * keySeq)
{
    gint i;

    for (i = 0; i < strlen(keySeq); i++) {
        key_press_from_key_sym((guint) keySeq[i], 0);
    }
}

void reset_properties_default(IBusChewingPreEdit * self)
{
    gsize size = mkdg_properties_size(self->iProperties->properties);

    for (gsize i = 0; i < size; i++) {
        PropertyContext *ctx =
            mkdg_properties_index(self->iProperties->properties, i);
        property_context_default(ctx);
    }
}

/*== Unit cases start ==*/
EventResponse filter_modifiers_test_0_0()
{
    KeyModifiers allow = 0, unmaskedMod = 0;

    filter_modifiers(allow);
    return EVENT_RESPONSE_PROCESS;
}

EventResponse filter_modifiers_test_0_shift()
{
    KeyModifiers allow = 0, unmaskedMod = IBUS_SHIFT_MASK;

    filter_modifiers(allow);
    return EVENT_RESPONSE_PROCESS;
}

EventResponse filter_modifiers_test_0_control()
{
    KeyModifiers allow = 0, unmaskedMod = IBUS_CONTROL_MASK;

    filter_modifiers(allow);
    return EVENT_RESPONSE_PROCESS;
}

EventResponse filter_modifiers_test_0_shift_control()
{
    KeyModifiers allow = 0, unmaskedMod = IBUS_SHIFT_MASK | IBUS_CONTROL_MASK;

    filter_modifiers(allow);
    return EVENT_RESPONSE_PROCESS;
}

EventResponse filter_modifiers_test_shift_0()
{
    KeyModifiers allow = IBUS_SHIFT_MASK, unmaskedMod = 0;

    filter_modifiers(allow);
    return EVENT_RESPONSE_PROCESS;
}

EventResponse filter_modifiers_test_shift_shift()
{
    KeyModifiers allow = IBUS_SHIFT_MASK, unmaskedMod = IBUS_SHIFT_MASK;

    filter_modifiers(allow);
    return EVENT_RESPONSE_PROCESS;
}

EventResponse filter_modifiers_test_shift_control()
{
    KeyModifiers allow = IBUS_SHIFT_MASK, unmaskedMod = IBUS_CONTROL_MASK;

    filter_modifiers(allow);
    return EVENT_RESPONSE_PROCESS;
}

EventResponse filter_modifiers_test_shift_shift_control()
{
    KeyModifiers allow = IBUS_SHIFT_MASK, unmaskedMod =
        IBUS_SHIFT_MASK | IBUS_CONTROL_MASK;
    filter_modifiers(allow);
    return EVENT_RESPONSE_PROCESS;
}


void filter_modifiers_test()
{
    g_assert(filter_modifiers_test_0_0() == EVENT_RESPONSE_PROCESS);
    g_assert(filter_modifiers_test_0_shift() == EVENT_RESPONSE_IGNORE);
    g_assert(filter_modifiers_test_0_control() == EVENT_RESPONSE_IGNORE);
    g_assert(filter_modifiers_test_0_shift_control() == EVENT_RESPONSE_IGNORE);

    g_assert(filter_modifiers_test_shift_0() == EVENT_RESPONSE_PROCESS);
    g_assert(filter_modifiers_test_shift_shift() == EVENT_RESPONSE_PROCESS);
    g_assert(filter_modifiers_test_shift_control() == EVENT_RESPONSE_IGNORE);
    g_assert(filter_modifiers_test_shift_shift_control() ==
             EVENT_RESPONSE_IGNORE);
}

void self_key_sym_fix_test()
{
    ibus_chewing_pre_edit_set_chi_eng_mode(self, FALSE);

    ibus_chewing_pre_edit_set_property_string(self, "chi-eng-mode-toggle",
                                              "caps_lock");

    ibus_chewing_pre_edit_set_property_string(self, "default-english-case",
                                              "no control");
    g_assert(self_key_sym_fix(self, '1', 0) == '1');
    g_assert(self_key_sym_fix(self, 'a', 0) == 'a');
    g_assert(self_key_sym_fix(self, 'A', 0) == 'A');
    g_assert(self_key_sym_fix(self, 'a', IBUS_SHIFT_MASK) == 'a');
    g_assert(self_key_sym_fix(self, 'A', IBUS_SHIFT_MASK) == 'A');
    g_assert(self_key_sym_fix(self, 'a', IBUS_LOCK_MASK) == 'a');
    g_assert(self_key_sym_fix(self, 'A', IBUS_LOCK_MASK) == 'A');
    g_assert(self_key_sym_fix(self, 'a', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'a');
    g_assert(self_key_sym_fix(self, 'A', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'A');

    ibus_chewing_pre_edit_set_property_string(self,
                                              "default-english-case",
                                              "lowercase");
    g_assert(self_key_sym_fix(self, '2', 0) == '2');
    g_assert(self_key_sym_fix(self, 'b', 0) == 'b');
    g_assert(self_key_sym_fix(self, 'B', 0) == 'b');
    g_assert(self_key_sym_fix(self, 'b', IBUS_SHIFT_MASK) == 'B');
    g_assert(self_key_sym_fix(self, 'B', IBUS_SHIFT_MASK) == 'B');
    g_assert(self_key_sym_fix(self, 'b', IBUS_LOCK_MASK) == 'b');
    g_assert(self_key_sym_fix(self, 'B', IBUS_LOCK_MASK) == 'b');
    g_assert(self_key_sym_fix(self, 'b', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'B');
    g_assert(self_key_sym_fix(self, 'B', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'B');

    ibus_chewing_pre_edit_set_property_string(self,
                                              "default-english-case",
                                              "uppercase");
    g_assert(self_key_sym_fix(self, 'c', 0) == 'C');
    g_assert(self_key_sym_fix(self, 'C', 0) == 'C');
    g_assert(self_key_sym_fix(self, 'c', IBUS_SHIFT_MASK) == 'c');
    g_assert(self_key_sym_fix(self, 'C', IBUS_SHIFT_MASK) == 'c');
    g_assert(self_key_sym_fix(self, 'c', IBUS_LOCK_MASK) == 'C');
    g_assert(self_key_sym_fix(self, 'C', IBUS_LOCK_MASK) == 'C');
    g_assert(self_key_sym_fix(self, 'c', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'c');
    g_assert(self_key_sym_fix(self, 'C', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'c');

    ibus_chewing_pre_edit_set_property_string(self, "chi-eng-mode-toggle",
                                              "shift");

    ibus_chewing_pre_edit_set_property_string(self, "default-english-case",
                                              "no control");
    g_assert(self_key_sym_fix(self, 'd', 0) == 'd');
    g_assert(self_key_sym_fix(self, 'D', 0) == 'D');
    g_assert(self_key_sym_fix(self, 'd', IBUS_SHIFT_MASK) == 'd');
    g_assert(self_key_sym_fix(self, 'D', IBUS_SHIFT_MASK) == 'D');
    g_assert(self_key_sym_fix(self, 'd', IBUS_LOCK_MASK) == 'd');
    g_assert(self_key_sym_fix(self, 'D', IBUS_LOCK_MASK) == 'D');
    g_assert(self_key_sym_fix(self, 'd', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'd');
    g_assert(self_key_sym_fix(self, 'D', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'D');

    /* This should act as "no control" */
    ibus_chewing_pre_edit_set_property_string(self,
                                              "default-english-case",
                                              "lowercase");
    g_assert(self_key_sym_fix(self, 'd', 0) == 'd');
    g_assert(self_key_sym_fix(self, 'D', 0) == 'D');
    g_assert(self_key_sym_fix(self, 'd', IBUS_SHIFT_MASK) == 'd');
    g_assert(self_key_sym_fix(self, 'D', IBUS_SHIFT_MASK) == 'D');
    g_assert(self_key_sym_fix(self, 'd', IBUS_LOCK_MASK) == 'd');
    g_assert(self_key_sym_fix(self, 'D', IBUS_LOCK_MASK) == 'D');
    g_assert(self_key_sym_fix(self, 'd', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'd');
    g_assert(self_key_sym_fix(self, 'D', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'D');

    /* This should act as "no control" */
    ibus_chewing_pre_edit_set_property_string(self,
                                              "default-english-case",
                                              "uppercase");
    g_assert(self_key_sym_fix(self, 'd', 0) == 'd');
    g_assert(self_key_sym_fix(self, 'D', 0) == 'D');
    g_assert(self_key_sym_fix(self, 'd', IBUS_SHIFT_MASK) == 'd');
    g_assert(self_key_sym_fix(self, 'D', IBUS_SHIFT_MASK) == 'D');
    g_assert(self_key_sym_fix(self, 'd', IBUS_LOCK_MASK) == 'd');
    g_assert(self_key_sym_fix(self, 'D', IBUS_LOCK_MASK) == 'D');
    g_assert(self_key_sym_fix(self, 'd', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'd');
    g_assert(self_key_sym_fix(self, 'D', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'D');

    /* In Chinese mode, the case should be determine by the shift, not the
     * case itself.
     * Otherwise, when Caps ON, Chinese mode, you won't be about to type
     * Chinese properly.
     */
    ibus_chewing_pre_edit_set_chi_eng_mode(self, TRUE);

    ibus_chewing_pre_edit_set_property_string(self, "chi-eng-mode-toggle",
                                              "caps_lock");

    ibus_chewing_pre_edit_set_property_string(self, "default-english-case",
                                              "no control");
    g_assert(self_key_sym_fix(self, 'e', 0) == 'e');
    g_assert(self_key_sym_fix(self, 'E', 0) == 'e');
    g_assert(self_key_sym_fix(self, 'e', IBUS_SHIFT_MASK) == 'E');
    g_assert(self_key_sym_fix(self, 'E', IBUS_SHIFT_MASK) == 'E');
    g_assert(self_key_sym_fix(self, 'e', IBUS_LOCK_MASK) == 'e');
    g_assert(self_key_sym_fix(self, 'E', IBUS_LOCK_MASK) == 'e');
    g_assert(self_key_sym_fix(self, 'e', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'E');
    g_assert(self_key_sym_fix(self, 'E', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'E');

    ibus_chewing_pre_edit_set_property_string(self,
                                              "default-english-case",
                                              "lowercase");
    g_assert(self_key_sym_fix(self, 'f', 0) == 'f');
    g_assert(self_key_sym_fix(self, 'F', 0) == 'f');
    g_assert(self_key_sym_fix(self, 'f', IBUS_SHIFT_MASK) == 'F');
    g_assert(self_key_sym_fix(self, 'F', IBUS_SHIFT_MASK) == 'F');
    g_assert(self_key_sym_fix(self, 'f', IBUS_LOCK_MASK) == 'f');
    g_assert(self_key_sym_fix(self, 'F', IBUS_LOCK_MASK) == 'f');
    g_assert(self_key_sym_fix(self, 'f', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'F');
    g_assert(self_key_sym_fix(self, 'F', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'F');

    ibus_chewing_pre_edit_set_property_string(self,
                                              "default-english-case",
                                              "uppercase");
    g_assert(self_key_sym_fix(self, 'g', 0) == 'g');
    g_assert(self_key_sym_fix(self, 'G', 0) == 'g');
    g_assert(self_key_sym_fix(self, 'g', IBUS_SHIFT_MASK) == 'G');
    g_assert(self_key_sym_fix(self, 'G', IBUS_SHIFT_MASK) == 'G');
    g_assert(self_key_sym_fix(self, 'g', IBUS_LOCK_MASK) == 'g');
    g_assert(self_key_sym_fix(self, 'G', IBUS_LOCK_MASK) == 'g');
    g_assert(self_key_sym_fix(self, 'g', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'G');
    g_assert(self_key_sym_fix(self, 'G', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'G');

    ibus_chewing_pre_edit_set_property_string(self, "chi-eng-mode-toggle",
                                              "shift");

    ibus_chewing_pre_edit_set_property_string(self, "default-english-case",
                                              "no control");
    g_assert(self_key_sym_fix(self, 'h', 0) == 'h');
    g_assert(self_key_sym_fix(self, 'H', 0) == 'h');
    g_assert(self_key_sym_fix(self, 'h', IBUS_SHIFT_MASK) == 'H');
    g_assert(self_key_sym_fix(self, 'H', IBUS_SHIFT_MASK) == 'H');
    g_assert(self_key_sym_fix(self, 'h', IBUS_LOCK_MASK) == 'h');
    g_assert(self_key_sym_fix(self, 'H', IBUS_LOCK_MASK) == 'h');
    g_assert(self_key_sym_fix(self, 'h', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'H');
    g_assert(self_key_sym_fix(self, 'H', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'H');

    ibus_chewing_pre_edit_set_property_string(self,
                                              "default-english-case",
                                              "lowercase");
    g_assert(self_key_sym_fix(self, 'i', 0) == 'i');
    g_assert(self_key_sym_fix(self, 'I', 0) == 'i');
    g_assert(self_key_sym_fix(self, 'i', IBUS_SHIFT_MASK) == 'I');
    g_assert(self_key_sym_fix(self, 'I', IBUS_SHIFT_MASK) == 'I');
    g_assert(self_key_sym_fix(self, 'i', IBUS_LOCK_MASK) == 'i');
    g_assert(self_key_sym_fix(self, 'I', IBUS_LOCK_MASK) == 'i');
    g_assert(self_key_sym_fix(self, 'i', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'I');
    g_assert(self_key_sym_fix(self, 'I', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'I');

    ibus_chewing_pre_edit_set_property_string(self,
                                              "default-english-case",
                                              "uppercase");
    g_assert(self_key_sym_fix(self, 'j', 0) == 'j');
    g_assert(self_key_sym_fix(self, 'J', 0) == 'j');
    g_assert(self_key_sym_fix(self, 'j', IBUS_SHIFT_MASK) == 'J');
    g_assert(self_key_sym_fix(self, 'J', IBUS_SHIFT_MASK) == 'J');
    g_assert(self_key_sym_fix(self, 'j', IBUS_LOCK_MASK) == 'j');
    g_assert(self_key_sym_fix(self, 'J', IBUS_LOCK_MASK) == 'j');
    g_assert(self_key_sym_fix(self, 'j', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'J');
    g_assert(self_key_sym_fix(self, 'J', IBUS_SHIFT_MASK | IBUS_LOCK_MASK)
             == 'J');
}

void self_handle_key_sym_default_test()
{
    TEST_CASE_INIT();

    EventResponse eResponse = self_handle_key_sym_default(self, 'q', 0);

    g_assert_cmpuint(eResponse, ==, EVENT_RESPONSE_PROCESS);
    /* self_handle_key_sym_default does not update pre_edit in self */
    ibus_chewing_pre_edit_update(self);
    assert_outgoing_pre_edit("", "ㄆ");
    ibus_chewing_pre_edit_clear(self);

#if 0
    /*=== Test default-english-case handling ===*/
    ibus_chewing_pre_edit_set_chi_eng_mode(self, FALSE);
    ibus_chewing_pre_edit_set_apply_property_string(self,
                                                    "default-english-case",
                                                    "lowercase");
    eResponse = self_handle_key_sym_default(self, 'a', 0);
    ibus_chewing_pre_edit_update(self);
    assert_outgoing_pre_edit("a", "");
    eResponse = self_handle_key_sym_default(self, 'A', 0);
    assert_outgoing_pre_edit("aa", "");
    eResponse = self_handle_key_sym_default(self, 'a', IBUS_SHIFT_MASK);
    assert_outgoing_pre_edit("aaA", "");
    eResponse = self_handle_key_sym_default(self, 'A', IBUS_SHIFT_MASK);
    assert_outgoing_pre_edit("aaAA", "");
    eResponse = self_handle_key_sym_default(self, 'a', IBUS_LOCK_MASK);
    assert_outgoing_pre_edit("aaAAa", "");
    eResponse = self_handle_key_sym_default(self, 'A', IBUS_LOCK_MASK);
    assert_outgoing_pre_edit("aaAAaa", "");
    eResponse =
        self_handle_key_sym_default(self, 'a',
                                    IBUS_SHIFT_MASK | IBUS_LOCK_MASK);
    assert_outgoing_pre_edit("aaAAaaA", "");
    eResponse =
        self_handle_key_sym_default(self, 'A',
                                    IBUS_SHIFT_MASK | IBUS_LOCK_MASK);
    assert_outgoing_pre_edit("aaAAaaAA", "");

    ibus_chewing_pre_edit_clear(self);
#endif

    assert_outgoing_pre_edit("", "");
}

/*== Test cases start ==*/
void free_test()
{
    ibus_chewing_pre_edit_free(self);
}

/* Chinese mode: "中文" (5j/ jp6) and Enter*/
void process_key_normal_test()
{
    TEST_CASE_INIT();
    key_press_from_string("5j/ jp6");
    assert_outgoing_pre_edit("", "中文");
    g_assert_cmpint(2, ==, self->wordLen);
    key_press_from_key_sym(IBUS_KEY_Return, 0);
    assert_outgoing_pre_edit("中文", "");
    g_assert_cmpint(0, ==, self->wordLen);

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");

}

/* 他不重，他是我兄弟。 */
void process_key_text_with_symbol_test()
{
    TEST_CASE_INIT();
    key_press_from_string("w8 ");
    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_string("2");

    key_press_from_string("1j65j/4");
    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_string("3");

    key_press_from_key_sym(IBUS_KEY_less, IBUS_SHIFT_MASK);

    key_press_from_string("w8 g4ji3vm/ 2u4");

    key_press_from_key_sym(IBUS_KEY_greater, IBUS_SHIFT_MASK);
    key_press_from_key_sym(IBUS_KEY_Return, 0);

    assert_outgoing_pre_edit("他不重，他是我兄弟。", "");

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
}

/* Mix english and chinese */
/* "這是ibus-chewing 輸入法"*/
void process_key_mix_test()
{
    TEST_CASE_INIT();

    ibus_chewing_pre_edit_set_property_string(self, "chi-eng-mode-toggle",
                                              "shift");

    key_press_from_string("5k4g4");
    key_press_from_key_sym(IBUS_KEY_Shift_L, 0);
    key_press_from_string("ibus-chewing ");
    key_press_from_key_sym(IBUS_KEY_Shift_L, 0);
    key_press_from_string("gj bj4z83");
    key_press_from_key_sym(IBUS_KEY_Return, 0);
    assert_outgoing_pre_edit("這是ibus-chewing 輸入法", "");

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
}

void process_key_incomplete_char_test()
{
    TEST_CASE_INIT();
    key_press_from_string("u");
    ibus_chewing_pre_edit_force_commit(self);
    assert_outgoing_pre_edit("ㄧ", "");

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
}

void process_key_buffer_full_handling_test()
{
    TEST_CASE_INIT();
    key_press_from_string("ji3ru8 ap6fu06u.3vul3ck6");
    key_press_from_key_sym(IBUS_KEY_less, IBUS_SHIFT_MASK);
    key_press_from_string("c.4au04u.3g0 qi ");
    key_press_from_key_sym(IBUS_KEY_Return, 0);
    assert_outgoing_pre_edit("我家門前有小河，後面有山坡", "");

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
}

/* 程式 */
void process_key_down_arrow_test()
{
    TEST_CASE_INIT();
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "plain-zhuyin", FALSE);
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "phrase-choice-from-last",
                                                     TRUE);
    key_press_from_string("t/6g4");
    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_string("1");
    assert_outgoing_pre_edit("", "城市");
    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_string("2");
    assert_outgoing_pre_edit("", "程式");

    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_string("4");
    assert_pre_edit_substring("世", 1, 1);

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
}

/* Test shift then caps then caps then shift */
/* String: 我要去 Brisbane 了。Daddy 好嗎 */
/* Bug before 1.5.0 */
/* Should be okay to remove this since we force users to choose between caps and shift */
void process_key_shift_and_caps_test()
{
#if 0

    TEST_CASE_INIT();
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "plain-zhuyin", FALSE);
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "space-as-selection",
                                                     FALSE);
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "shift-toggle-chinese",
                                                     TRUE);
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "capslock-toggle-chinese",
                                                     TRUE);
    ibus_chewing_pre_edit_set_apply_property_int(self,
                                                 "max-chi-symbol-len", 33);
    g_assert(ibus_chewing_pre_edit_get_chi_eng_mode(self));

    key_press_from_string("ji3ul4fm4 ");
    assert_outgoing_pre_edit("", "我要去 ");

    /* Shift: change to Eng-Mode */
    key_press_from_key_sym(IBUS_KEY_Shift_L, 0);
    g_assert(!ibus_chewing_pre_edit_get_chi_eng_mode(self));
    key_press_from_key_sym(IBUS_KEY_B, IBUS_SHIFT_MASK);        /* B */
    key_press_from_string("risbane ");
    assert_outgoing_pre_edit("", "我要去 Brisbane ");

    /* Caps Lock ON: change to Chi-Mode */
    key_press_from_key_sym(IBUS_KEY_Caps_Lock, 0);
    g_assert(ibus_chewing_pre_edit_get_chi_eng_mode(self));
    key_press_from_string("xk7");
    key_press_from_key_sym(IBUS_KEY_greater, IBUS_SHIFT_MASK);
    assert_outgoing_pre_edit("", "我要去 Brisbane 了。");

    /* Caps Lock OFF: change to Eng-Mode */
    key_press_from_key_sym(IBUS_KEY_Caps_Lock, 0);
    g_assert(!ibus_chewing_pre_edit_get_chi_eng_mode(self));
    key_press_from_key_sym(IBUS_KEY_D, IBUS_SHIFT_MASK);        /* D */
    key_press_from_string("addy ");

    /* Shift: change to Chi-Mode */
    key_press_from_key_sym(IBUS_KEY_Shift_L, 0);
    g_assert(ibus_chewing_pre_edit_get_chi_eng_mode(self));
    key_press_from_string("cl3a87");
    assert_outgoing_pre_edit("", "我要去 Brisbane 了。Daddy 好嗎");

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
#endif
}

void full_half_shape_test()
{
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "plain-zhuyin", FALSE);
    g_assert(ibus_chewing_pre_edit_get_chi_eng_mode(self));
    ibus_chewing_pre_edit_toggle_chi_eng_mode(self);
    g_assert(!ibus_chewing_pre_edit_get_chi_eng_mode(self));

    key_press_from_key_sym(IBUS_KEY_space, IBUS_SHIFT_MASK);
    g_assert(chewing_get_ShapeMode(self->context));

    key_press_from_string("ab ");
    assert_outgoing_pre_edit("ａｂ　", "");

    key_press_from_key_sym(IBUS_KEY_space, IBUS_SHIFT_MASK);
    g_assert(!chewing_get_ShapeMode(self->context));

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
    ibus_chewing_pre_edit_toggle_chi_eng_mode(self);
}

void plain_zhuyin_test()
{
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "plain-zhuyin", TRUE);

    g_assert(ibus_chewing_pre_edit_get_property_boolean(self, "plain-zhuyin"));

    key_press_from_string("y ");

    /* Candidate window should be shown */
    g_assert(ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));
    /* The default is the most frequently used character, not
     * necessary "資"
     */
    /* assert_outgoing_pre_edit("","資"); */
    key_press_from_string("4");
    assert_outgoing_pre_edit("吱", "");
    /* Candidate window should be hidden */
    g_assert(!ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
}

/*  你好，*/
void plain_zhuyin_shift_symbol_test()
{
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "plain-zhuyin", TRUE);
    g_assert(ibus_chewing_pre_edit_get_property_boolean(self, "plain-zhuyin"));

    ibus_chewing_pre_edit_set_property_string(self, "chi-eng-mode-toggle",
                                              "shift");

    key_press_from_string("su31cl31");

    /* ， */
    key_press_from_key_sym(IBUS_KEY_less, IBUS_SHIFT_MASK);
    /* Candidate window should be shown */
    g_assert(ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));
    key_press_from_string("1");

    /* Candidate window should be hidden */
    g_assert(!ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));

    assert_outgoing_pre_edit("你好，", "");

    /* 打電話  */
    key_press_from_string("28312u041cj841");
    assert_outgoing_pre_edit("你好，打電話", "");

    /* ； */
    key_press_from_key_sym(IBUS_KEY_quotedbl, IBUS_SHIFT_MASK);
    key_press_from_string("1");
    assert_outgoing_pre_edit("你好，打電話；", "");

    /* Mix with shift */

    key_press_from_key_sym(IBUS_KEY_Shift_L, IBUS_SHIFT_MASK);

    /* String is bypass in English mode */
    key_press_from_string("4321-9876 ");

    assert_outgoing_pre_edit("你好，打電話；", "");
    key_press_from_key_sym(IBUS_KEY_Shift_L, IBUS_SHIFT_MASK);
    /* "來訂餐" */
    key_press_from_string("x9612u/42h0 2");
    assert_outgoing_pre_edit("你好，打電話；來訂餐", "");

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
}

void plain_zhuyin_full_half_shape_test()
{
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "plain-zhuyin", TRUE);
    g_assert(ibus_chewing_pre_edit_get_chi_eng_mode(self));
    ibus_chewing_pre_edit_toggle_chi_eng_mode(self);
    g_assert(!ibus_chewing_pre_edit_get_chi_eng_mode(self));

    key_press_from_key_sym(IBUS_KEY_space, IBUS_SHIFT_MASK);
    g_assert(chewing_get_ShapeMode(self->context));

    key_press_from_string("ab ");
    assert_outgoing_pre_edit("ａｂ　", "");

    key_press_from_key_sym(IBUS_KEY_space, IBUS_SHIFT_MASK);
    g_assert(!chewing_get_ShapeMode(self->context));

    ibus_chewing_pre_edit_clear(self);
    assert_outgoing_pre_edit("", "");
}

void test_ibus_chewing_pre_edit_clear_bopomofo()
{
    TEST_CASE_INIT();

    key_press_from_string("su3cl");     /* 你ㄏㄠ (尚未完成組字) */
    assert_outgoing_pre_edit("", "你ㄏㄠ");

    ibus_chewing_pre_edit_clear_bopomofo(self);
    assert_outgoing_pre_edit("", "你");
}

void test_ibus_chewing_pre_edit_clear_pre_edit()
{
    TEST_CASE_INIT();

    key_press_from_string("su3cl");     /* 你ㄏㄠ (尚未完成組字) */
    assert_outgoing_pre_edit("", "你ㄏㄠ");

    ibus_chewing_pre_edit_clear_pre_edit(self);
    assert_outgoing_pre_edit("", "");
}

void test_ibus_chewing_pre_edit_set_chi_eng_mode()
{
    TEST_CASE_INIT();

    key_press_from_string("su3cl");     /* 你ㄏㄠ (尚未完成組字) */
    ibus_chewing_pre_edit_set_chi_eng_mode(self, FALSE);
    g_assert(chewing_get_ChiEngMode(self->context) == 0);
    assert_outgoing_pre_edit("", "你");
}

void test_space_as_selection()
{
/* GitHub #79: Cannot input space when "space to select" is enabled  */
    TEST_CASE_INIT();
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "space-as-selection",
                                                     TRUE);

    key_press_from_key_sym(IBUS_KEY_space, 0);
    assert_outgoing_pre_edit(" ", "");
}

void test_arrow_keys_buffer_empty()
{
/* GitHub #50: Cannot use Up, Down, PgUp, Ese ... etc. within "`" menu */

    TEST_CASE_INIT();
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
                                                     "vertical-lookup-table",
                                                     TRUE);

    key_press_from_string("`");
    g_assert(ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));
    g_assert(chewing_cand_CurrentPage(self->context) == 0);
    key_press_from_key_sym(IBUS_KEY_Right, 0);
    g_assert(chewing_cand_CurrentPage(self->context) == 1);
    key_press_from_key_sym(IBUS_KEY_Left, 0);
    g_assert(chewing_cand_CurrentPage(self->context) == 0);
    key_press_from_key_sym(IBUS_KEY_Page_Down, 0);
    g_assert(chewing_cand_CurrentPage(self->context) == 1);
    key_press_from_key_sym(IBUS_KEY_Page_Up, 0);
    g_assert(chewing_cand_CurrentPage(self->context) == 0);
    key_press_from_key_sym(IBUS_KEY_Escape, 0);
    g_assert(!ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));

    key_press_from_string("`");
    g_assert(ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));
    key_press_from_key_sym(IBUS_KEY_BackSpace, 0);
    g_assert(!ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));
}

void test_ctrl_1_open_candidate_list()
{
/* GitHub #63: Cannnot add user-phrase via ctrl+num */

    TEST_CASE_INIT();

    key_press_from_key_sym(IBUS_KEY_1, IBUS_CONTROL_MASK);
    g_assert(ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));
    key_press_from_key_sym(IBUS_KEY_Escape, 0);
}

void test_kp_eng_mode()
{
/* Eng-Mode: keypad outputs numbers directly */

    TEST_CASE_INIT();

    ibus_chewing_pre_edit_set_chi_eng_mode(self, FALSE);
    g_assert(chewing_get_ChiEngMode(self->context) == 0);

    key_press_from_key_sym(IBUS_KP_1, 0);
    key_press_from_key_sym(IBUS_KEY_KP_9, 0);
    key_press_from_key_sym(IBUS_KP_0, 0);
    assert_outgoing_pre_edit("", "");
}

void test_kp_eng_mode_buffer()
{
/* Eng-Mode: When buffer is not empty, keypad outputs numbers into buffer. */

    TEST_CASE_INIT();

    key_press_from_string("su3cl3");    /* 你好 */
    ibus_chewing_pre_edit_set_chi_eng_mode(self, FALSE);
    g_assert(chewing_get_ChiEngMode(self->context) == 0);
    key_press_from_key_sym(IBUS_KP_1, 0);
    key_press_from_key_sym(IBUS_KEY_KP_9, 0);
    key_press_from_key_sym(IBUS_KP_0, 0);
    assert_outgoing_pre_edit("", "你好190");
}

void test_kp_chi_mode()
{
/* Chi-Mode: keypad outputs numbers by default */

    TEST_CASE_INIT();

    g_assert(chewing_get_ChiEngMode(self->context) == 1);

    key_press_from_key_sym(IBUS_KP_1, 0);
    key_press_from_key_sym(IBUS_KEY_KP_9, 0);
    key_press_from_key_sym(IBUS_KP_0, 0);
    assert_outgoing_pre_edit("", "");

    /* should remain chi-mode */
    g_assert(chewing_get_ChiEngMode(self->context) == 1);
}

void test_kp_chi_mode_buffer()
{
/* Chi-Mode: When buffer is not empty, keypad outputs numbers into buffer */

    TEST_CASE_INIT();

    g_assert(chewing_get_ChiEngMode(self->context) == 1);

    key_press_from_string("su3cl3");    /* 你好 */
    key_press_from_key_sym(IBUS_KP_1, 0);
    key_press_from_key_sym(IBUS_KEY_KP_9, 0);
    key_press_from_key_sym(IBUS_KP_0, 0);
    assert_outgoing_pre_edit("", "你好190");

    /* should remain chi-mode */
    g_assert(chewing_get_ChiEngMode(self->context) == 1);
}

void test_kp_chi_incomplete()
{
/* Chi-Mode with bopmofos (incomplete characters):
 * clear bopomofos and output numbers into buffer.
 */

    TEST_CASE_INIT();

    g_assert(chewing_get_ChiEngMode(self->context) == 1);

    key_press_from_string("su3cl");     /* 你ㄏㄠ (尚未完成組字) */
    key_press_from_key_sym(IBUS_KP_1, 0);
    assert_outgoing_pre_edit("", "你1");
}

void test_kp_selecting()
{
/* While selecting candidates: select or do nothing */

    TEST_CASE_INIT();

    key_press_from_key_sym(IBUS_KP_1, IBUS_CONTROL_MASK);
    g_assert(ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));
    key_press_from_key_sym(IBUS_KP_2, 0);
    assert_outgoing_pre_edit("", "※");

    key_press_from_key_sym(IBUS_KP_1, IBUS_CONTROL_MASK);
    key_press_from_key_sym(IBUS_KP_1, 0);
    assert_outgoing_pre_edit("", "※…");

//  TODO: need to check if selkeys are 1234567890
}

void test_kp_other_keys()
{
    TEST_CASE_INIT();

    g_assert(chewing_get_ChiEngMode(self->context) == 1);

    key_press_from_string("su3cl3");    /* 你好 */
    key_press_from_key_sym(IBUS_KP_Multiply, 0);
    key_press_from_key_sym(IBUS_KP_Add, 0);
    key_press_from_key_sym(IBUS_KP_Separator, 0);
    key_press_from_key_sym(IBUS_KP_Subtract, 0);
    key_press_from_key_sym(IBUS_KP_Decimal, 0);
    key_press_from_key_sym(IBUS_KP_Divide, 0);
    assert_outgoing_pre_edit("", "你好*+,-./");

    /* should remain chi-mode */
    g_assert(chewing_get_ChiEngMode(self->context) == 1);
}

void test_keypad()
{
    test_kp_eng_mode();
    test_kp_eng_mode_buffer();
    test_kp_chi_mode();
    test_kp_chi_mode_buffer();
    test_kp_chi_incomplete();
    test_kp_selecting();
    test_kp_other_keys();
}

gint main(gint argc, gchar ** argv)
{
    g_test_init(&argc, &argv, NULL);
#ifdef USE_GSETTINGS
    MkdgBackend *backend =
        mkdg_g_settings_backend_new(QUOTE_ME(PROJECT_SCHEMA_ID),
                                    QUOTE_ME(PROJECT_SCHEMA_DIR), NULL);
#elif defined USE_GCONF2
    MkdgBackend *backend =
        gconf2_backend_new(QUOTE_ME(PROJECT_SCHEMA_BASE), NULL);
#else
    MkdgBackend *backend = NULL;

    g_error("Flag GSETTINGS_SUPPORT or GCONF2_SUPPORT are required!");
    return 1;
#endif                          /* USE_GSETTINGS */
    mkdg_log_set_level(DEBUG);
    self = ibus_chewing_pre_edit_new(backend);
    ibus_chewing_pre_edit_use_all_configure(self);

    g_assert(self != NULL);
    TEST_RUN_THIS(filter_modifiers_test);
    TEST_RUN_THIS(self_key_sym_fix_test);
    TEST_RUN_THIS(self_handle_key_sym_default_test);
    TEST_RUN_THIS(process_key_normal_test);
    TEST_RUN_THIS(process_key_text_with_symbol_test);
    TEST_RUN_THIS(process_key_mix_test);
    TEST_RUN_THIS(process_key_incomplete_char_test);
    TEST_RUN_THIS(process_key_buffer_full_handling_test);
    TEST_RUN_THIS(process_key_down_arrow_test);
    TEST_RUN_THIS(process_key_shift_and_caps_test);
    TEST_RUN_THIS(full_half_shape_test);
    TEST_RUN_THIS(plain_zhuyin_test);
    TEST_RUN_THIS(plain_zhuyin_shift_symbol_test);
    TEST_RUN_THIS(plain_zhuyin_full_half_shape_test);
    TEST_RUN_THIS(test_ibus_chewing_pre_edit_clear_bopomofo);
    TEST_RUN_THIS(test_ibus_chewing_pre_edit_clear_pre_edit);
    TEST_RUN_THIS(test_ibus_chewing_pre_edit_set_chi_eng_mode);
    TEST_RUN_THIS(test_space_as_selection);
    TEST_RUN_THIS(test_arrow_keys_buffer_empty);
    TEST_RUN_THIS(test_ctrl_1_open_candidate_list);
    TEST_RUN_THIS(test_keypad);
    TEST_RUN_THIS(free_test);
    return g_test_run();
}
