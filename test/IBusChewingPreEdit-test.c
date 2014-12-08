#include <string.h>
#include <glib.h>
#include "IBusChewingPreEdit.h"
#include "IBusChewingUtil.h"
#ifdef GSETTINGS_SUPPORT
#include "GSettingsBackend.h"
#elif GCONF2_SUPPORT
#include "GConf2Backend.h"
#endif
#include "MakerDialogUtil.h"
#include "test-util.h"
#define TEST_RUN_THIS(f) add_test_case("IBusChewingPreEdit", f)    

static IBusChewingPreEdit *self = NULL;

void free_test()
{
    ibus_chewing_pre_edit_free(self);
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

void check_pre_edit(const gchar * outgoing, const gchar * pre_edit)
{
    g_assert_cmpstr(outgoing, ==,
		    ibus_chewing_pre_edit_get_outgoing(self));
    g_assert_cmpstr(pre_edit, ==,
		    ibus_chewing_pre_edit_get_pre_edit(self));
}


/* Chinese mode: "中文" (5j/ jp6) and Enter*/
void process_key_normal_test()
{
    key_press_from_string("5j/ jp6");
    check_pre_edit("", "中文");
    g_assert_cmpint(2, ==, self->wordLen);
    key_press_from_key_sym(IBUS_KEY_Return, 0);
    check_pre_edit("中文", "");
    g_assert_cmpint(0, ==, self->wordLen);

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");

}

/* 他不重，他是我兄弟。 */
void process_key_text_with_symbol_test()
{
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

    check_pre_edit("他不重，他是我兄弟。", "");

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");
}

/* Mix english and chinese */
/* " 這是ibus-chewing 輸入法"*/
void process_key_mix_test()
{
    key_press_from_string(" 5k4g4");
    key_press_from_key_sym(IBUS_KEY_Shift_L, 0);
    key_press_from_string("ibus-chewing ");
    key_press_from_key_sym(IBUS_KEY_Shift_L, 0);
    key_press_from_string("gj bj4z83");
    key_press_from_key_sym(IBUS_KEY_Return, 0);
    check_pre_edit("這是ibus-chewing 輸入法", "");

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");
}

void process_key_incomplete_char_test()
{
    key_press_from_string("u");
    ibus_chewing_pre_edit_force_commit(self);
    check_pre_edit("ㄧ", "");

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");
}

void process_key_buffer_full_handling_test()
{
    key_press_from_string("ji3ru8 ap6fu06u.3vul3ck6");
    key_press_from_key_sym(',', IBUS_SHIFT_MASK);
    key_press_from_string("c.4au04u.3g0 qi ");
    key_press_from_key_sym(IBUS_KEY_Return, 0);
    check_pre_edit("我家門前有小河，後面有山坡", "");

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");
}

/* 程式 */
void process_key_down_arrow_test()
{
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
	    "plain-zhuyin", FALSE);
    key_press_from_string("t/6g4");
    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_string("1");
    check_pre_edit("", "城市");
    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_string("2");
    check_pre_edit("", "程式");

    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_key_sym(IBUS_KEY_Down, 0);
    key_press_from_string("4");
    check_pre_edit("", "程世");

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");
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
    check_pre_edit("ａｂ　", "");

    key_press_from_key_sym(IBUS_KEY_space, IBUS_SHIFT_MASK);
    g_assert(!chewing_get_ShapeMode(self->context));

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");
    ibus_chewing_pre_edit_toggle_chi_eng_mode(self);
}

void plain_zhuyin_test()
{
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
						     "plain-zhuyin", TRUE);

    g_assert(ibus_chewing_pre_edit_get_property_boolean
	     (self, "plain-zhuyin"));

    key_press_from_string("y ");

    /* Candidate window should be shown */
    g_assert(ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));
    /* The default is the most frequently used character, not
     * necessary "資"
     */
    /* check_pre_edit("","資"); */
    key_press_from_string("4");
    check_pre_edit("吱", "");
    /* Candidate window should be hidden */
    g_assert(!ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");
}

/*  你好，*/
void plain_zhuyin_shift_symbol_test()
{
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
						     "plain-zhuyin", TRUE);
    g_assert(ibus_chewing_pre_edit_get_property_boolean
	     (self, "plain-zhuyin"));
    ibus_chewing_pre_edit_set_apply_property_boolean(self,
						     "shift-toggle-chinese",
						     TRUE);

    key_press_from_string("su31cl31");

    /* ， */
    key_press_from_key_sym(IBUS_KEY_less, IBUS_SHIFT_MASK);
    /* Candidate window should be shown */
    g_assert(ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));
    key_press_from_string("1");

    /* Candidate window should be hidden */
    g_assert(!ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW));

    check_pre_edit("你好，", "");

    /* 打電話  */
    key_press_from_string("28312u041cj841");
    check_pre_edit("你好，打電話", "");

    /* ； */
    key_press_from_key_sym(IBUS_KEY_quotedbl, IBUS_SHIFT_MASK);
    key_press_from_string("1");
    check_pre_edit("你好，打電話；", "");

    /* Mix with shift */

    key_press_from_key_sym(IBUS_KEY_Shift_L, IBUS_SHIFT_MASK);

    /* String is bypass in English mode */
    key_press_from_string("882-5252");

    check_pre_edit("你好，打電話；", "");
    key_press_from_key_sym(IBUS_KEY_Shift_L, IBUS_SHIFT_MASK);
    /* " 來訂餐" */
    key_press_from_string(" x9612u/42h0 2");
    check_pre_edit("你好，打電話；來訂餐", "");

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");
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
    check_pre_edit("ａｂ　", "");

    key_press_from_key_sym(IBUS_KEY_space, IBUS_SHIFT_MASK);
    g_assert(!chewing_get_ShapeMode(self->context));

    ibus_chewing_pre_edit_clear(self);
    check_pre_edit("", "");
}

gint main(gint argc, gchar ** argv)
{
    g_test_init(&argc, &argv, NULL);
#ifdef GSETTINGS_SUPPORT
    MkdgBackend *backend =
	mkdg_g_settings_backend_new(QUOTE_ME(PROJECT_SCHEMA_ID),
				    QUOTE_ME(PROJECT_SCHEMA_DIR), NULL);
#elif GCONF2_SUPPORT
    MkdgBackend *backend =
	gconf2_backend_new("/desktop/ibus/engine", NULL);
#else
    MkdgBackend *backend = NULL;
    g_error("Flag GSETTINGS_SUPPORT or GCONF2_SUPPORT are required!");
    return 1;
#endif				/* GSETTINGS_SUPPORT */
    mkdg_log_set_level(DEBUG);
    self = ibus_chewing_pre_edit_new(backend);

    ibus_chewing_pre_edit_set_apply_property_int(self,
						 "max-chi-symbol-len", 8);

    ibus_chewing_pre_edit_set_apply_property_string(self,
						    "sel-keys",
						    "1234567890");

    ibus_chewing_pre_edit_set_apply_property_boolean(self,
						     "plain-zhuyin",
						     FALSE);

    g_assert(self != NULL);

    TEST_RUN_THIS(process_key_normal_test);
    TEST_RUN_THIS(process_key_text_with_symbol_test);
    TEST_RUN_THIS(process_key_mix_test);
    TEST_RUN_THIS(process_key_incomplete_char_test);
    TEST_RUN_THIS(process_key_buffer_full_handling_test);
    TEST_RUN_THIS(process_key_down_arrow_test);
    TEST_RUN_THIS(full_half_shape_test);
    TEST_RUN_THIS(plain_zhuyin_test);
    TEST_RUN_THIS(plain_zhuyin_shift_symbol_test);
    TEST_RUN_THIS(plain_zhuyin_full_half_shape_test);
    TEST_RUN_THIS(free_test);
    return g_test_run();
}
