#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "MakerDialogUtil.h"
#include "MakerDialogBackend.h"
#ifdef GSETTINGS_SUPPORT
#include "GSettingsBackend.h"
#elif GCONF2_SUPPORT
#include "GConf2Backend.h"
#endif

MkdgBackend *backend = NULL;
#define COMMAND_BUFFER_SIZE 200
#define FILE_BUFFER_SIZE 1024

GValue *gsettings_command_get_key_value(const gchar * key, GValue * value)
{
    gchar cmdBuf[COMMAND_BUFFER_SIZE];
    g_snprintf(cmdBuf, COMMAND_BUFFER_SIZE, "gsettings get %s %s",
	       QUOTE_ME(PROJECT_SCHEMA_ID), key);
    FILE *stream = popen(cmdBuf, "r");
    if (stream == NULL) {
	g_error("Cannot pipe from gsettings");
	exit(3);
    }
    gchar readBuffer[FILE_BUFFER_SIZE];
    gchar *line = fgets(readBuffer, FILE_BUFFER_SIZE, stream);
    pclose(stream);
    line[strlen(line) - 1] = '\0';
    printf("line=%s\n", line);
    mkdg_g_value_from_string(value, line);
    return value;
}

gboolean mkdg_g_value_is_equal(GValue * value1, GValue * value2)
{
    switch (G_VALUE_TYPE(value1)) {
    case G_TYPE_BOOLEAN:
	return (g_value_get_boolean(value1) ==
		g_value_get_boolean(value2));
    case G_TYPE_INT:
	return (g_value_get_int(value1) == g_value_get_int(value2));
    case G_TYPE_UINT:
	return (g_value_get_uint(value1) == g_value_get_uint(value2));
    case G_TYPE_STRING:
	return (STRING_EQUALS
		(g_value_get_string(value1), g_value_get_string(value2)));
    default:
	break;
    }
    return FALSE;
}

void boolean_w_test()
{
    GValue origValue = { 0 };
    g_value_init(&origValue, G_TYPE_BOOLEAN);
    gsettings_command_get_key_value("plain-zhuyin", &origValue);

    GValue newValue = { 0 };
    g_value_init(&newValue, G_TYPE_BOOLEAN);
    g_value_set_boolean(&newValue, !g_value_get_boolean(&origValue));
    mkdg_backend_write(backend, &newValue, QUOTE_ME(PROJECT_SCHEMA_DIR),
		       "plain-zhuyin", NULL);

    GValue storedValue = { 0 };
    g_value_init(&storedValue, G_TYPE_BOOLEAN);
    gsettings_command_get_key_value("plain-zhuyin", &storedValue);

    g_assert(mkdg_g_value_is_equal(&newValue, &storedValue));

}


gint main(gint argc, gchar ** argv)
{
    g_test_init(&argc, &argv, NULL);
#ifdef GSETTINGS_SUPPORT
    backend = mkdg_g_settings_backend_new(QUOTE_ME(PROJECT_SCHEMA_ID),
					  QUOTE_ME(PROJECT_SCHEMA_DIR),
					  NULL);
#elif GCONF2_SUPPORT
    backend = gconf2_backend_new("/desktop/ibus/engine", NULL);
#else
    g_error("Flag GSETTINGS_SUPPORT or GCONF2_SUPPORT are required!");
    return 2;
#endif				/* GSETTINGS_SUPPORT */
    mkdg_log_set_level(DEBUG);

    g_test_add_func
	("/ibus-chewing/MakerDialogBackend/boolean_w_test",
	 boolean_w_test);

#if 0
    g_test_add_func
	("/ibus-chewing/MakerDialogBackend/int_w_test", int_w_test);

    g_test_add_func
	("/ibus-chewing/MakerDialogBackend/string_w_test", string_w_test);
#endif
    return g_test_run();
}
