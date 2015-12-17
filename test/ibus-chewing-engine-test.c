#include <glib.h>
#include "ibus-chewing-engine.h"
#include "ibus-chewing-engine-private.h"
#include "MakerDialogUtil.h"
#include "test-util.h"
#define TEST_RUN_THIS(f) add_test_case("ibus-chewing-engine", f)

static IBusChewingEngine *engine = NULL;

IBusChewingEngine *ibus_chewing_engine_new()
{
    return (IBusChewingEngine *) g_object_new(IBUS_TYPE_CHEWING_ENGINE,
					      NULL);
}

void check_output(const gchar * outgoing, const gchar * preEdit,
		  const gchar * aux)
{
    g_assert(engine->outgoingText);
    g_assert(engine->outgoingText->text);
    printf("outgoingText->text=%s\n", engine->outgoingText->text);
    g_assert_cmpstr(outgoing, ==, engine->outgoingText->text);
    printf("preEditText->text=%s\n", engine->preEditText->text);
    g_assert_cmpstr(preEdit, ==, engine->preEditText->text);
    printf("auxText->text=%s\n", engine->auxText->text);
    g_assert_cmpstr(aux, ==, engine->auxText->text);
}

void focus_out_then_focus_in_with_aux_text_test()
{
    ibus_chewing_engine_set_capabilite(engine, IBUS_CAP_AUXILIARY_TEXT);
    ibus_chewing_engine_focus_in(engine);
    ibus_chewing_engine_enable(engine);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), 'j', 0x24,
					  0);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), 'j', 0x24,
					  IBUS_RELEASE_MASK);
    check_output("", "ㄨ", "ㄨ");

    /* focus out should not touch Texts
     */
    ibus_chewing_engine_focus_out(engine);
    check_output("", "ㄨ", "ㄨ");

    /* all should be clean */
    ibus_chewing_engine_focus_in(engine);
    check_output("", "", "");

    ibus_chewing_pre_edit_clear(engine->icPreEdit);
    check_output("", "", "");

}


gint main(gint argc, gchar ** argv)
{
    g_test_init(&argc, &argv, NULL);
    mkdg_log_set_level(DEBUG);
    engine = ibus_chewing_engine_new();
    ibus_chewing_pre_edit_set_property_int(engine->icPreEdit,
					   "max-chi-symbol-len", 8);
    ibus_chewing_pre_edit_apply_property(engine->icPreEdit,
					 "max-chi-symbol-len");

    ibus_chewing_pre_edit_set_property_boolean(engine->icPreEdit,
					       "plain-zhuyin", FALSE);
    ibus_chewing_pre_edit_apply_property(engine->icPreEdit,
					 "plain-zhuyin");

    TEST_RUN_THIS(focus_out_then_focus_in_with_aux_text_test);
    return g_test_run();
}
