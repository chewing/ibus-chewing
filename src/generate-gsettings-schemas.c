#include <stdio.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <glib/gprintf.h>
#include <ibus.h>

/*
 * Callback should be skipped
 */
#ifndef MKDG_SPEC_ONLY
#define MKDG_SPEC_ONLY
#endif

#include "MakerDialogUtil.h"
#include "MakerDialogProperty.h"
#include "GSettingsBackend.h"
#include "IBusChewingProperties.h"
#define XML_BUFFER_SIZE 1000
#define DEFAULT_LOCALES "C;zh_TW"

static gint verbose = WARN;
static gchar *schemasFilename = NULL;

static const GOptionEntry entries[] = {
    {"verbose", 'v', 0, G_OPTION_ARG_INT, &verbose,
     "Verbose level. The higher the level, the more the debug messages.",
     "[integer]"},
    {NULL},
};

/**
 * write_gconf_schemas_file:
 * @filename: Filename for output.
 *
 * Output the parameters as GSettings schemes file.
 */
gboolean write_schemas_file(const gchar * filename)
{
    mkdg_log(INFO, "write_schemes_file(%s,)", filename);
    FILE *outF = fopen(filename, "w");
    if (outF == NULL) {
	mkdg_log(DEBUG,
		 "write_gconf_schemas_file(%s) file %s cannot be written!",
		 filename, filename);
	return FALSE;
    }

    gboolean result =
	mkdg_g_settings_write_schema_from_spec_array(
		QUOTE_ME(PROJECT_SCHEMA_ID),
		QUOTE_ME(PROJECT_SCHEMA_PATH),
		outF, propSpecs, QUOTE_ME(PROJECT_NAME));

    if (fclose(outF))
	return FALSE;
    mkdg_log(DEBUG, "write_gconf_schemas_file(%s) ... done.", filename);
    return result;
}

int main(gint argc, gchar * argv[])
{
    GError *error = NULL;
    GOptionContext *context;

    /* Init i18n messages */
    setlocale(LC_ALL, "");
    bindtextdomain(QUOTE_ME(PROJECT_NAME), QUOTE_ME(DATA_DIR) "/locale");
    textdomain(QUOTE_ME(PROJECT_NAME));

    context = g_option_context_new("schemasFile");

    g_option_context_add_main_entries(context, entries, "ibus-chewing");

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
	g_print("Option parsing failed: %s\n", error->message);
	return 1;
    }
    g_option_context_free(context);
    if (argc < 2) {
	fprintf(stderr, "Specify filename of outputing schemas file!\n");
	return 1;
    }
    mkdg_log_set_level(verbose);
    schemasFilename = argv[1];
    gboolean result = write_schemas_file(schemasFilename);
    if (!result) {
	return 2;
    }
    return 0;
}
