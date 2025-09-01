#include "ibus-chewing-util.h"
#include "test-util.h"
#include <ctype.h>
#include <glib.h>
#define TEST_RUN_THIS(f) add_test_case("IBusChewingUtil", f)

void key_sym_get_name_test() {
    /* Test  */
    guint kSym;

    for (kSym = 0; kSym <= 0xffff; kSym++) {
        g_assert(key_sym_get_name(kSym) != NULL);
        IBUS_CHEWING_LOG(INFO, "key %x(%s)", kSym, key_sym_get_name(kSym));
    }
    /* Should handle invalid Key Sym properly */
    g_assert_cmpstr(key_sym_get_name(0x10000), ==, "WARN");
    g_assert_cmpstr(key_sym_get_name(-1), ==, "WARN");
}

gint main(gint argc, gchar **argv) {
    g_test_init(&argc, &argv, NULL);
    mkdg_log_set_level(INFO);
    TEST_RUN_THIS(key_sym_get_name_test);
    return g_test_run();
}
