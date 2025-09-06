#include "glib-object.h"
#include "ibus-chewing-engine-private.h"
#include "ibus-chewing-engine.h"
#include "test-util.h"
#include <glib.h>
#include <stdio.h>

#define TEST_RUN_THIS(f) add_test_case("ibus-chewing-engine", f)

static IBusChewingEngine *engine = NULL;

IBusChewingEngine *ibus_chewing_engine_new() {
    return (IBusChewingEngine *)g_object_new(IBUS_TYPE_CHEWING_ENGINE, NULL);
}

void check_output(const gchar *outgoing, const gchar *preEdit, const gchar *aux) {
    g_assert(engine->outgoingText);
    g_assert(engine->outgoingText->text);
    printf("# outgoingText->text=%s\n", engine->outgoingText->text);
    g_assert_cmpstr(outgoing, ==, engine->outgoingText->text);
    printf("# preEditText->text=%s\n", engine->preEditText->text);
    g_assert_cmpstr(preEdit, ==, engine->preEditText->text);
    printf("# auxText->text=%s\n", engine->auxText->text);
    g_assert_cmpint(strlen(aux), ==, strlen(engine->auxText->text));
}

void focus_out_then_focus_in_with_aux_text_test() {
    gboolean cleanBufferFocusOut;
    g_object_get(G_OBJECT(engine), "clean-buffer-focus-out", &cleanBufferFocusOut, NULL);
    g_object_set(G_OBJECT(engine), "add-phrase-direction", TRUE, NULL);

    g_signal_emit_by_name(engine, "set_capabilities", IBUS_CAP_AUXILIARY_TEXT);
    ibus_chewing_engine_focus_in(IBUS_ENGINE(engine));
    ibus_chewing_engine_enable(IBUS_ENGINE(engine));
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), 'j', 0x24, 0);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), 'j', 0x24, IBUS_RELEASE_MASK);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), '3', 0x04, 0);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), '3', 0x04, IBUS_RELEASE_MASK);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), 'j', 0x24, 0);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), 'j', 0x24, IBUS_RELEASE_MASK);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), '3', 0x04, 0);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), '3', 0x04, IBUS_RELEASE_MASK);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), '2', 0x03, IBUS_CONTROL_MASK);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), '2', 0x03, IBUS_RELEASE_MASK);
    check_output("", "五五", "已有：五五");

    /* focus out should not touch Texts */
    ibus_chewing_engine_focus_out(IBUS_ENGINE(engine));
    gboolean value;
    g_object_get(G_OBJECT(engine), "clean-buffer-focus-out", &value, NULL);
    g_assert(cleanBufferFocusOut == value);

    if (cleanBufferFocusOut) {
        check_output("", "", "");
    } else {
        check_output("", "五五", "已有：五五");
    }

    /* all should be clean */
    ibus_chewing_engine_focus_in(IBUS_ENGINE(engine));
    check_output("", "", "");

    ibus_chewing_pre_edit_clear(engine->icPreEdit);
    check_output("", "", "");
}

void focus_out_then_focus_in_with_aux_text_clean_buffer_on_test() {
    engine = ibus_chewing_engine_new();

    g_object_set(G_OBJECT(engine), "max-chi-symbol-len", 8, NULL);
    g_object_set(G_OBJECT(engine), "clean-buffer-focus-out", TRUE, NULL);

    focus_out_then_focus_in_with_aux_text_test();

    g_object_unref(engine);
}

void focus_out_then_focus_in_with_aux_text_clean_buffer_off_test() {
    engine = ibus_chewing_engine_new();

    g_object_set(G_OBJECT(engine), "max-chi-symbol-len", 8, NULL);
    g_object_set(G_OBJECT(engine), "clean-buffer-focus-out", FALSE, NULL);

    focus_out_then_focus_in_with_aux_text_test();

    g_object_unref(engine);
}

gint main(gint argc, gchar **argv) {
    g_test_init(&argc, &argv, NULL);

    TEST_RUN_THIS(focus_out_then_focus_in_with_aux_text_clean_buffer_off_test);
    TEST_RUN_THIS(focus_out_then_focus_in_with_aux_text_clean_buffer_on_test);

    return g_test_run();
}
