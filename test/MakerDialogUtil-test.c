#include <glib.h>
#include "test-util.h"
#include "MakerDialogUtil.h"
#define TEST_RUN_THIS(f) add_test_case("MakerDialogUtil", f)

void mkdg_g_value_to_string_test()
{
    /* Test int */
    GValue intValue = { 0 };
    g_value_init(&intValue, G_TYPE_INT);
    g_value_set_int(&intValue, 0);
    g_assert_cmpstr("0", ==, mkdg_g_value_to_string(&intValue));

    g_value_set_int(&intValue, -1);
    g_assert_cmpstr("-1", ==, mkdg_g_value_to_string(&intValue));
    g_value_unset(&intValue);

    /* Test uint */
    GValue uintValue = { 0 };
    g_value_init(&uintValue, G_TYPE_UINT);
    g_value_set_uint(&uintValue, 0);
    g_assert_cmpstr("0", ==, mkdg_g_value_to_string(&uintValue));

    g_value_set_uint(&uintValue, 1);
    g_assert_cmpstr("1", ==, mkdg_g_value_to_string(&uintValue));
    g_value_unset(&uintValue);

    /* Test boolean */
    GValue booleanValue = { 0 };
    g_value_init(&booleanValue, G_TYPE_BOOLEAN);
    g_value_set_boolean(&booleanValue, TRUE);
    g_assert_cmpstr("1", ==, mkdg_g_value_to_string(&booleanValue));

    g_value_set_boolean(&booleanValue, FALSE);
    g_assert_cmpstr("0", ==, mkdg_g_value_to_string(&booleanValue));
    g_value_unset(&booleanValue);
}

void mkdg_g_value_from_string_test()
{
    /* Test int */
    GValue intValue = { 0 };
    g_value_init(&intValue, G_TYPE_INT);

    mkdg_g_value_from_string(&intValue, "0");
    g_assert_cmpint(0, ==, g_value_get_int(&intValue));

    mkdg_g_value_from_string(&intValue, "-1");
    g_assert_cmpint(-1, ==, g_value_get_int(&intValue));

    g_value_unset(&intValue);

    /* Test uint */
    GValue uintValue = { 0 };
    g_value_init(&uintValue, G_TYPE_UINT);

    mkdg_g_value_from_string(&uintValue, "0");
    g_assert_cmpuint(0, ==, g_value_get_uint(&uintValue));

    mkdg_g_value_from_string(&uintValue, "1");
    g_assert_cmpuint(1, ==, g_value_get_uint(&uintValue));

    g_value_unset(&uintValue);

    /* Test boolean */
    GValue booleanValue = { 0 };
    g_value_init(&booleanValue, G_TYPE_BOOLEAN);

    mkdg_g_value_from_string(&booleanValue, "1");
    g_assert(g_value_get_boolean(&booleanValue));

    mkdg_g_value_from_string(&booleanValue, "0");
    g_assert(!g_value_get_boolean(&booleanValue));

    mkdg_g_value_from_string(&booleanValue, "T");
    g_assert(g_value_get_boolean(&booleanValue));

    mkdg_g_value_from_string(&booleanValue, "F");
    g_assert(!g_value_get_boolean(&booleanValue));

    mkdg_g_value_from_string(&booleanValue, "true");
    g_assert(g_value_get_boolean(&booleanValue));

    mkdg_g_value_from_string(&booleanValue, "false");
    g_assert(!g_value_get_boolean(&booleanValue));

    g_value_unset(&booleanValue);
}


/**************************************
 * String Utility Macros
 */

void QUOTE_ME_test()
{
#define PRJ_TEST  MKDG
    g_assert_cmpstr("MKDG", ==, QUOTE_ME(PRJ_TEST));
#undef PRJ_TEST
}

void STRING_IS_EMPTY_test()
{
    gchar *nulStr = NULL;

    g_assert(STRING_IS_EMPTY(nulStr));
    g_assert(STRING_IS_EMPTY(""));
    g_assert(!STRING_IS_EMPTY("NULL"));
}

void STRING_EQUALS_test()
{
    gchar *nulStr = NULL;

    g_assert(STRING_EQUALS("", ""));
    g_assert(!STRING_EQUALS("", nulStr));
    g_assert(STRING_EQUALS(nulStr, nulStr));
    g_assert(!STRING_EQUALS("HI", "Hi"));
    g_assert(STRING_EQUALS("YO\"", "YO\""));
}


gint main(gint argc, gchar ** argv)
{
    g_test_init(&argc, &argv, NULL);
    TEST_RUN_THIS(mkdg_g_value_to_string_test);
    TEST_RUN_THIS(mkdg_g_value_from_string_test);
    TEST_RUN_THIS(QUOTE_ME_test);
    TEST_RUN_THIS(STRING_IS_EMPTY_test);
#if 0
    TEST_RUN_THIS(STRING_EQUALS_test);
#endif
    return g_test_run();
}
