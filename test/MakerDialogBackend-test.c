#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "test-util.h"
#include "MakerDialogUtil.h"
#include "MakerDialogBackend.h"
#ifdef USE_GSETTINGS
#include "GSettingsBackend.h"
#elif defined USE_GCONF2
#include "GConf2Backend.h"
#endif
#define TEST_RUN_THIS(f) add_test_case("MakerDialogBackend", f)
#define COMMAND_BUFFER_SIZE 200
#define FILE_BUFFER_SIZE 1024

MkdgBackend *backend = NULL;

gchar *command_run_obtain_output(const gchar * cmd)
{
    mkdg_log(DEBUG, "### command_run_obtain_output(%s)\n",cmd);
    static gchar readBuffer[FILE_BUFFER_SIZE];
    FILE *stream = popen(cmd, "r");
    if (stream == NULL) {
	g_error("Cannot pipe from command %s", cmd);
	exit(3);
    }
    gchar *line = fgets(readBuffer, FILE_BUFFER_SIZE, stream);
    pclose(stream);
    if (line == NULL) {
	printf("### line=NULL\n");
    } else {
	line[strlen(line) - 1] = '\0';
	printf("### line=%s\n", line);
    }
    return line;
}

GValue *backend_command_get_key_value(const gchar * key, GValue * value)
{
    gchar cmdBuf[COMMAND_BUFFER_SIZE];
    gchar *cKey=mkdg_backend_get_key(backend, NULL, key, NULL);
#ifdef USE_GSETTINGS
    g_snprintf(cmdBuf, COMMAND_BUFFER_SIZE, "gsettings get %s %s",
	       QUOTE_ME(PROJECT_SCHEMA_ID), cKey);
#else
    g_snprintf(cmdBuf, COMMAND_BUFFER_SIZE, "gconftool-2 --get  %s/%s",
	       QUOTE_ME(PROJECT_GCONF2_SCHEMA_DIR), cKey);
#endif
    gchar *retStr = command_run_obtain_output(cmdBuf);

#ifdef USE_GSETTINGS
    /* gsettings prepend 'uint32 ' before actual value */
    if (G_VALUE_TYPE(value) == G_TYPE_UINT) {
	gint offset = strlen("uint32 ");
	retStr += offset;
    }
#endif
    mkdg_g_value_from_string(value, retStr);
    return value;
}

void backend_command_set_key_value(const gchar * key, GValue * value)
{
    gchar *valueStr = mkdg_g_value_to_string(value);
    if (mkdg_g_value_is_boolean(value)) {
	valueStr = (g_value_get_boolean(value)) ? "true" : "false";
    }
    gchar cmdBuf[COMMAND_BUFFER_SIZE];
    gchar *cKey=mkdg_backend_get_key(backend, NULL, key, NULL);
#ifdef USE_GSETTINGS
    g_snprintf(cmdBuf, COMMAND_BUFFER_SIZE,
	       "gsettings set %s %s %s",
	       QUOTE_ME(PROJECT_SCHEMA_ID), cKey, valueStr);
#else
    gchar *typeStr;
    GType gType = G_VALUE_TYPE(value);
    switch (gType) {
    case G_TYPE_BOOLEAN:
	typeStr = "bool";
	break;
    case G_TYPE_FLOAT:
    case G_TYPE_DOUBLE:
	typeStr = "float";
	break;
    case G_TYPE_INT:
    case G_TYPE_UINT:
    case G_TYPE_LONG:
    case G_TYPE_ULONG:
    case G_TYPE_INT64:
    case G_TYPE_UINT64:
	typeStr = "int";
	break;
    case G_TYPE_STRING:
	typeStr = "string";
	break;
    }
    g_snprintf(cmdBuf, COMMAND_BUFFER_SIZE,
	       "gconftool-2 --set %s/%s --type %s '%s'",
	       QUOTE_ME(PROJECT_SCHEMA_DIR), cKey, typeStr, valueStr);
#endif

    command_run_obtain_output(cmdBuf);
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

void backup_key_to_g_value(const gchar * key, GType gType, GValue * value)
{
    g_value_init(value, gType);
    backend_command_get_key_value(key, value);
}

void change_new_value_from_orig_value(GValue * newValue,
				      GValue * origValue)
{
    g_value_init(newValue, G_VALUE_TYPE(origValue));
    gchar *tempStr = NULL;
    switch (G_VALUE_TYPE(origValue)) {
    case G_TYPE_BOOLEAN:
	g_value_set_boolean(newValue, !g_value_get_boolean(origValue));
	break;
    case G_TYPE_INT:
	g_value_set_int(newValue,
			(g_value_get_int(origValue) >
			 0) ? g_value_get_int(origValue) -
			1 : g_value_get_int(origValue) + 1);
	break;
    case G_TYPE_UINT:
	g_value_set_uint(newValue,
			 (g_value_get_uint(origValue) >
			  0) ? g_value_get_uint(origValue) -
			 1 : g_value_get_uint(origValue) + 1);
	break;
    case G_TYPE_STRING:
	tempStr = g_strdup_printf("%sx", g_value_get_string(origValue));
	g_value_set_string(newValue, tempStr);
	break;
    default:
	break;
    }
}

void write_key_with_g_value(const gchar * key, GValue * value)
{
    mkdg_backend_write(backend, value, QUOTE_ME(PROJECT_SCHEMA_SECTION), key,
		       NULL);
}

void assert_new_value_is_written(const gchar * key, GValue * newValue)
{
    GValue storedGValue = { 0 };
    g_value_init(&storedGValue, G_VALUE_TYPE(newValue));
    backend_command_get_key_value(key, &storedGValue);
    g_assert(mkdg_g_value_is_equal(newValue, &storedGValue));
    g_value_unset(&storedGValue);
}

void mkdg_g_value_from_string_test(const gchar * key, GType gType)
{
    GValue origGValue = { 0 };
    backup_key_to_g_value(key, gType, &origGValue);

    GValue newGValue = { 0 };
    change_new_value_from_orig_value(&newGValue, &origGValue);

    write_key_with_g_value(key, &newGValue);
    assert_new_value_is_written(key, &newGValue);

    /*
     * Restore the origValue 
     */
    write_key_with_g_value(key, &origGValue);

    g_value_unset(&origGValue);
    g_value_unset(&newGValue);
}

void mkdg_g_value_from_string_boolean_test()
{
    mkdg_g_value_from_string_test("plain-zhuyin", G_TYPE_BOOLEAN);
}

void mkdg_g_value_from_string_int_test()
{
    mkdg_g_value_from_string_test("max-chi-symbol-len", G_TYPE_INT);
}

void mkdg_g_value_from_string_uint_test()
{
    mkdg_g_value_from_string_test("cand-per-page", G_TYPE_UINT);
}

void mkdg_g_value_from_string_string_test()
{
    mkdg_g_value_from_string_test("max-chi-symbol-len", G_TYPE_INT);
}

void int_w_test()
{
#define GCONF_KEY "max-chi-symbol-len"
    GValue origValue = { 0 };
    g_value_init(&origValue, G_TYPE_BOOLEAN);
    backend_command_get_key_value(GCONF_KEY, &origValue);

    GValue newValue = { 0 };
    g_value_init(&newValue, G_TYPE_BOOLEAN);
    g_value_set_int(&newValue, !g_value_get_int(&origValue));
    mkdg_backend_write(backend, &newValue, QUOTE_ME(PROJECT_SCHEMA_SECTION),
		       GCONF_KEY, NULL);

    GValue storedValue = { 0 };
    g_value_init(&storedValue, G_TYPE_BOOLEAN);
    backend_command_get_key_value(GCONF_KEY, &storedValue);
    g_assert(mkdg_g_value_is_equal(&newValue, &storedValue));

    /*
     * Restore the original value 
     */
    backend_command_set_key_value(GCONF_KEY, &origValue);
#undef GCONF_KEY
}

gint main(gint argc, gchar ** argv)
{
    g_test_init(&argc, &argv, NULL);
#ifdef USE_GSETTINGS
    backend = mkdg_g_settings_backend_new(QUOTE_ME(PROJECT_SCHEMA_ID),
					  QUOTE_ME(PROJECT_SCHEMA_DIR),
					  NULL);
#elif defined USE_GCONF2
    backend = gconf2_backend_new(QUOTE_ME(PROJECT_SCHEMA_BASE), NULL);
#else
    g_error("Flag GSETTINGS_SUPPORT or GCONF2_SUPPORT are required!");
    return 2;
#endif				/* USE_GSETTINGS */
    mkdg_log_set_level(DEBUG);

    TEST_RUN_THIS(mkdg_g_value_from_string_boolean_test);
    TEST_RUN_THIS(mkdg_g_value_from_string_int_test);
    TEST_RUN_THIS(mkdg_g_value_from_string_uint_test);
    TEST_RUN_THIS(mkdg_g_value_from_string_string_test);
    return g_test_run();
}
