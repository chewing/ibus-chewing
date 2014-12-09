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

#include "IBusChewingEngine-def.c"
#include "MakerDialogUtil.h"
#include "MakerDialogProperty.h"
#include "IBusConfigBackend.h"
#include "IBusChewingProperties.h"
#define XML_BUFFER_SIZE 1000
#define DEFAULT_LOCALES "C;zh_TW"

static gint verbose = WARN;
static gchar *schemasFilename = NULL;
static gchar *localeStr = NULL;
static gboolean isLocaleStrAllocated = FALSE;
static gchar **localeArray = NULL;

static const GOptionEntry entries[] = {
    {"verbose", 'v', 0, G_OPTION_ARG_INT, &verbose,
     "Verbose level. The higher the level, the more the debug messages.",
     "[integer]"},
    {"locale", 'l', 0, G_OPTION_ARG_STRING, &localeStr,
     "Supported locales. Use ';' to separate locales.",
     "[str]"},
    {NULL},
};


/*============================================
 * GConf Schemas routines
 */

static void ctx_write_locale(PropertyContext * ctx,
			     FILE * outF, const gchar * locale)
{
    gchar buf[XML_BUFFER_SIZE];
    mkdg_log(DEBUG, "ctx_write_locale(%s,-,%s)", ctx->spec->key, locale);
    setlocale(LC_ALL, "locale");
    g_snprintf(buf, 50, "name=\"%s\"", locale);
    setlocale(LC_MESSAGES, locale);
    mkdg_xml_tags_write(outF, "locale", MKDG_XML_TAG_TYPE_BEGIN_ONLY, buf,
			NULL);
    mkdg_xml_tags_write(outF, "short", MKDG_XML_TAG_TYPE_SHORT, NULL,
			gettext(ctx->spec->label));
    mkdg_xml_tags_write(outF, "long", MKDG_XML_TAG_TYPE_LONG, NULL,
			gettext(ctx->spec->tooltip));
    mkdg_xml_tags_write(outF, "locale", MKDG_XML_TAG_TYPE_END_ONLY, NULL,
			NULL);
}

gboolean ctx_write(PropertyContext * ctx, const gchar * schemasHome,
		   const gchar * owner, FILE * outF)
{
    mkdg_xml_tags_write(outF, "schema", MKDG_XML_TAG_TYPE_BEGIN_ONLY, NULL,
			NULL);
    gchar buf[XML_BUFFER_SIZE];
    if (STRING_IS_EMPTY(ctx->spec->subSection)) {
	g_snprintf(buf, XML_BUFFER_SIZE, "/schemas%s/%s",
		   schemasHome, ctx->spec->key);
    } else {
	g_snprintf(buf, XML_BUFFER_SIZE, "/schemas%s/%s/%s",
		   schemasHome, ctx->spec->subSection, ctx->spec->key);
    }
    mkdg_xml_tags_write(outF, "key", MKDG_XML_TAG_TYPE_SHORT, NULL, buf);
    mkdg_xml_tags_write(outF, "applyto", MKDG_XML_TAG_TYPE_SHORT, NULL,
			buf + strlen("/schemas"));
    mkdg_xml_tags_write(outF, "owner", MKDG_XML_TAG_TYPE_SHORT, NULL,
			owner);
    switch (ctx->spec->valueType) {
    case G_TYPE_BOOLEAN:
	mkdg_xml_tags_write(outF, "type", MKDG_XML_TAG_TYPE_SHORT, NULL,
			    "bool");
	break;
    case G_TYPE_INT:
    case G_TYPE_UINT:
	mkdg_xml_tags_write(outF, "type", MKDG_XML_TAG_TYPE_SHORT, NULL,
			    "int");
	break;
    case G_TYPE_STRING:
	mkdg_xml_tags_write(outF, "type", MKDG_XML_TAG_TYPE_SHORT, NULL,
			    "string");
	break;
    default:
	break;
    }
    if (ctx->spec->defaultValue) {
	mkdg_xml_tags_write(outF, "default", MKDG_XML_TAG_TYPE_SHORT, NULL,
			    ctx->spec->defaultValue);
    }
    int i;
    for (i = 0; localeArray[i] != NULL; i++) {
	ctx_write_locale(ctx, outF, localeArray[i]);
    }

    setlocale(LC_ALL, NULL);
    mkdg_xml_tags_write(outF, "schema", MKDG_XML_TAG_TYPE_END_ONLY, NULL,
			NULL);
    return TRUE;
}

/**
 * write_gconf_schemas_file:
 * @filename: Filename for output.
 * @owner: Owner of the schemas.
 * @schemasHome: The "home direcory" of schemas.
 * @localeStr: Supported localeStr, use ';' as delimiter.
 * @returns: TRUE if succeed; FALSE otherwise.
 *
 * Output the parameters as GConf schemes file.
 */
gboolean write_gconf_schemas_file(const gchar * filename,
				  const gchar * owner,
				  const gchar * schemasHome,
				  const gchar * localeStr)
{
    mkdg_log(INFO, "write_gconf_schemes_file(%s,%s,%s,%s)", filename,
	     owner, schemasHome, localeStr);
    FILE *outF = fopen(filename, "w");
    if (outF == NULL) {
	mkdg_log(DEBUG,
		 "write_gconf_schemas_file(%s) file %s cannot be written!",
		 filename, filename);
	return FALSE;
    }
    localeArray = g_strsplit_set(localeStr, ":;", -1);
    gchar **loc;
    for (loc = localeArray; *loc != NULL; loc++) {
	mkdg_log(DEBUG, "write_gconf_schemas_file() locale=%s", *loc);
    }

    /* Header */
    mkdg_xml_tags_write(outF, "gconfschemafile",
			MKDG_XML_TAG_TYPE_BEGIN_ONLY, NULL, NULL);
    mkdg_xml_tags_write(outF, "schemalist",
			MKDG_XML_TAG_TYPE_BEGIN_ONLY, NULL, NULL);
    /* Body */
    /* Backend is not needed for schema generation */
    IBusChewingConfig *iConfig =
	ibus_chewing_properties_new(NULL, NULL, NULL, NULL);
    gsize i;
    for (i = 0; i < mkdg_properties_size(iConfig->properties); i++) {
	PropertyContext *ctx =
	    mkdg_properties_index(iConfig->properties, i);
	ctx_write(ctx, schemasHome, owner, outF);
    }

    /* Footer */
    mkdg_xml_tags_write(outF, "schemalist", MKDG_XML_TAG_TYPE_END_ONLY,
			NULL, NULL);
    mkdg_xml_tags_write(outF, "gconfschemafile",
			MKDG_XML_TAG_TYPE_END_ONLY, NULL, NULL);
    if (fclose(outF))
	return FALSE;
    mkdg_log(DEBUG, "write_gconf_schemas_file(%s) ... done.", filename);
    return TRUE;
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
	return 2;
    }
    g_option_context_free(context);
    if (argc < 2) {
	fprintf(stderr, "Specify output schemas file!\n");
	return 2;
    }
    if (localeStr == NULL) {
	localeStr = DEFAULT_LOCALES;
    } else {
	isLocaleStrAllocated = TRUE;
    }
    mkdg_log_set_level(verbose);
    schemasFilename = argv[1];
    g_type_init();
    gboolean result =
	write_gconf_schemas_file(schemasFilename, "ibus-chewing",
				 QUOTE_ME(PROJECT_SCHEMA_PATH), localeStr);
    if (isLocaleStrAllocated) {
	g_free(localeStr);
    }
    if (!result) {
	return 1;
    }
    return 0;
}
