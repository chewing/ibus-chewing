#include "glib-object.h"
#include "ibus-chewing-engine-private.h"
#include "ibus-chewing-engine.h"
#include "ibus.h"
#include "test-util.h"
#include <glib.h>
#include <stdio.h>

#define TEST_RUN_THIS(f) add_test_case("ibus-chewing-engine", f)

static IBusChewingEngine *engine = NULL;

IBusChewingEngine *ibus_chewing_engine_new() {
    return (IBusChewingEngine *)g_object_new(IBUS_TYPE_CHEWING_ENGINE, NULL);
}

#define check_output(outgoing, preedit, aux)                                                       \
    {                                                                                              \
        g_assert(engine->outgoingText);                                                            \
        g_assert(engine->outgoingText->text);                                                      \
        printf("# outgoingText->text=%s\n", engine->outgoingText->text);                           \
        g_assert_cmpstr(outgoing, ==, engine->outgoingText->text);                                 \
        printf("# preEditText->text=%s\n", engine->preEditText->text);                             \
        g_assert_cmpstr(preedit, ==, engine->preEditText->text);                                   \
        printf("# auxText->text=%s\n", engine->auxText->text);                                     \
        g_assert_cmpint(strlen(aux), ==, strlen(engine->auxText->text));                           \
    }

void commit_text_test() {
    engine = ibus_chewing_engine_new();
    g_test_queue_unref(engine);

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
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), IBUS_KEY_Return, 28, 0);
    check_output("五五", "", "");
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), IBUS_KEY_Return, 28,
                                          IBUS_RELEASE_MASK);
    check_output("", "", "");
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

void show_hide_property() {
    engine = ibus_chewing_engine_new();
    g_test_queue_unref(engine);

    IBUS_ENGINE_GET_CLASS(engine)->property_show(IBUS_ENGINE(engine), "InputMode");
    IBUS_ENGINE_GET_CLASS(engine)->property_hide(IBUS_ENGINE(engine), "InputMode");
    IBUS_ENGINE_GET_CLASS(engine)->property_show(IBUS_ENGINE(engine), "AlnumSize");
    IBUS_ENGINE_GET_CLASS(engine)->property_hide(IBUS_ENGINE(engine), "AlnumSize");
    IBUS_ENGINE_GET_CLASS(engine)->property_show(IBUS_ENGINE(engine), "setup_prop");
    IBUS_ENGINE_GET_CLASS(engine)->property_hide(IBUS_ENGINE(engine), "setup_prop");
}

void enable_disable() {
    engine = ibus_chewing_engine_new();
    g_test_queue_unref(engine);

    IBUS_ENGINE_GET_CLASS(engine)->enable(IBUS_ENGINE(engine));
    IBUS_ENGINE_GET_CLASS(engine)->reset(IBUS_ENGINE(engine));
    IBUS_ENGINE_GET_CLASS(engine)->disable(IBUS_ENGINE(engine));
}

void activate_property() {
    engine = ibus_chewing_engine_new();
    g_test_queue_unref(engine);

    IBUS_ENGINE_GET_CLASS(engine)->property_activate(IBUS_ENGINE(engine), "InputMode", 0);
    IBUS_ENGINE_GET_CLASS(engine)->property_activate(IBUS_ENGINE(engine), "AlnumSize", 0);
    IBUS_ENGINE_GET_CLASS(engine)->property_activate(IBUS_ENGINE(engine), "setup_prop", 0);
    IBUS_ENGINE_GET_CLASS(engine)->property_activate(IBUS_ENGINE(engine), "UNKNOWN_PROP", 0);
}

void content_type() {
    engine = ibus_chewing_engine_new();
    g_test_queue_unref(engine);

    ibus_chewing_engine_set_content_type(IBUS_ENGINE(engine), IBUS_INPUT_PURPOSE_PASSWORD, 0);
    ibus_chewing_engine_set_content_type(IBUS_ENGINE(engine), IBUS_INPUT_PURPOSE_PIN, 0);
    ibus_chewing_engine_set_content_type(IBUS_ENGINE(engine), IBUS_INPUT_PURPOSE_DIGITS, 0);
}

void page_up_down() {
    engine = ibus_chewing_engine_new();
    g_test_queue_unref(engine);

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
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), IBUS_KEY_Down, 0x6c, 0);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), IBUS_KEY_Down, 0x6c,
                                          IBUS_RELEASE_MASK);
    IBUS_ENGINE_GET_CLASS(engine)->page_down(IBUS_ENGINE(engine));
    IBUS_ENGINE_GET_CLASS(engine)->page_up(IBUS_ENGINE(engine));
}

void cursor_up_down() {
    engine = ibus_chewing_engine_new();
    g_test_queue_unref(engine);

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
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), IBUS_KEY_Down, 0x6c, 0);
    ibus_chewing_engine_process_key_event(IBUS_ENGINE(engine), IBUS_KEY_Down, 0x6c,
                                          IBUS_RELEASE_MASK);
    IBUS_ENGINE_GET_CLASS(engine)->cursor_down(IBUS_ENGINE(engine));
    IBUS_ENGINE_GET_CLASS(engine)->cursor_up(IBUS_ENGINE(engine));
}

gint main(gint argc, gchar **argv) {
    g_test_init(&argc, &argv, NULL);

    TEST_RUN_THIS(commit_text_test);
    TEST_RUN_THIS(focus_out_then_focus_in_with_aux_text_clean_buffer_off_test);
    TEST_RUN_THIS(focus_out_then_focus_in_with_aux_text_clean_buffer_on_test);
    TEST_RUN_THIS(show_hide_property);
    TEST_RUN_THIS(enable_disable);
    TEST_RUN_THIS(activate_property);
    TEST_RUN_THIS(content_type);
    TEST_RUN_THIS(page_up_down);
    TEST_RUN_THIS(cursor_up_down);

    return g_test_run();
}
