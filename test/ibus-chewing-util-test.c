#include "ibus-chewing-util.h"
#include "test-util.h"
#include <glib.h>

#define TEST_RUN_THIS(f) add_test_case("IBusChewingUtil", f)

void QUOTE_ME_test() {
#define PRJ_TEST MKDG
    g_assert_cmpstr("MKDG", ==, QUOTE_ME(PRJ_TEST));
#undef PRJ_TEST
}

void STRING_IS_EMPTY_test() {
    gchar *nulStr = NULL;

    g_assert(STRING_IS_EMPTY(nulStr));
    g_assert(STRING_IS_EMPTY(""));
    g_assert(!STRING_IS_EMPTY("NULL"));
}

void modifiers_to_string_test() {
    const char *modifiers = modifiers_to_string(0xFFFFFFFF);
    g_assert(!STRING_IS_EMPTY(modifiers));
    g_assert(STRING_IS_EMPTY(modifiers_to_string(0)));
}

gint main(gint argc, gchar **argv) {
    g_test_init(&argc, &argv, NULL);
    TEST_RUN_THIS(QUOTE_ME_test);
    TEST_RUN_THIS(STRING_IS_EMPTY_test);
    TEST_RUN_THIS(modifiers_to_string_test);
    return g_test_run();
}
