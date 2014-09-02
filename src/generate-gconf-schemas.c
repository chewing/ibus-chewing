#include <stdio.h>
#include <glib/gi18n.h>
#include <locale.h>
#include <glib/gprintf.h>
#include "IBusChewingEngine-def.c"
#include "MakerDialogUtil.h"
#include "IBusChewingConfig.h"
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
 * GConf Schemas methods
 */

typedef enum {
    XML_TAG_TYPE_NO_TAG,
    XML_TAG_TYPE_EMPTY,
    XML_TAG_TYPE_SHORT,
    XML_TAG_TYPE_LONG,
    XML_TAG_TYPE_BEGIN_ONLY,
    XML_TAG_TYPE_END_ONLY,
} XmlTagsType;
#define INDENT_SPACES 4

static void append_indent_space(GString * strBuf, gint indentLevel)
{
    int i, indentLen = indentLevel * INDENT_SPACES;
    for (i = 0; i < indentLen; i++) {
	g_string_append_c(strBuf, ' ');
    }
}

static GString *xml_tags_to_string(const gchar * tagName, XmlTagsType type,
				   const gchar * attribute,
				   const gchar * value, gint indentLevel)
{
    GString *strBuf = g_string_new(NULL);
    append_indent_space(strBuf, indentLevel);

    if (type != XML_TAG_TYPE_NO_TAG) {
	g_string_append_printf(strBuf, "<%s%s%s%s%s>",
			       (type == XML_TAG_TYPE_END_ONLY) ? "/" : "",
			       (!STRING_IS_EMPTY(tagName)) ? tagName : "",
			       (!STRING_IS_EMPTY(attribute)) ? " " : "",
			       (!STRING_IS_EMPTY(attribute)) ? attribute :
			       "",
			       (type == XML_TAG_TYPE_EMPTY) ? "/" : "");
    }
    if (type == XML_TAG_TYPE_EMPTY)
	return strBuf;
    if (type == XML_TAG_TYPE_BEGIN_ONLY)
	return strBuf;
    if (type == XML_TAG_TYPE_END_ONLY)
	return strBuf;

    if (type == XML_TAG_TYPE_LONG) {
	g_string_append_c(strBuf, '\n');
    }

    if (value) {
	if (type == XML_TAG_TYPE_LONG || type == XML_TAG_TYPE_NO_TAG) {
	    append_indent_space(strBuf, indentLevel + 1);
	    int i, valueLen = strlen(value);
	    for (i = 0; i < valueLen; i++) {
		g_string_append_c(strBuf, value[i]);
		if (value[i] == '\n') {
		    append_indent_space(strBuf, indentLevel + 1);
		}
	    }
	    g_string_append_c(strBuf, '\n');
	    if (type == XML_TAG_TYPE_LONG) {
		append_indent_space(strBuf, indentLevel);
	    }
	} else {
	    g_string_append(strBuf, value);
	}
    }

    if (type == XML_TAG_TYPE_LONG || type == XML_TAG_TYPE_SHORT) {
	g_string_append_printf(strBuf, "</%s>", tagName);
    }
    return strBuf;
}

static void xml_tags_write(FILE * outF, const gchar * tagName,
			   XmlTagsType type, const gchar * attribute,
			   const gchar * value)
{
    static int indentLevel = 0;
    if (type == XML_TAG_TYPE_END_ONLY)
	indentLevel--;

    GString *strBuf =
	xml_tags_to_string(tagName, type, attribute, value, indentLevel);
    mkdg_log(INFO, "xml_tags_write:%s", strBuf->str);
    fprintf(outF, "%s\n", strBuf->str);

    if (type == XML_TAG_TYPE_BEGIN_ONLY)
	indentLevel++;
    g_string_free(strBuf, TRUE);
}

static void ctx_write_locale(PropertyContext * ctx,
			     FILE * outF, const gchar * locale)
{
    gchar buf[XML_BUFFER_SIZE];
    mkdg_log(DEBUG,"ctx_write_locale(%s,-,%s)", ctx->spec->key, locale);
    g_snprintf(buf, 50, "name=\"%s\"", locale);
    setlocale(LC_MESSAGES, locale);
    xml_tags_write(outF, "locale", XML_TAG_TYPE_BEGIN_ONLY, buf, NULL);
    xml_tags_write(outF, "short", XML_TAG_TYPE_SHORT, NULL,
		   gettext(ctx->spec->label));
    xml_tags_write(outF, "long", XML_TAG_TYPE_LONG, NULL,
		   gettext(ctx->spec->tooltip));
    xml_tags_write(outF, "locale", XML_TAG_TYPE_END_ONLY, NULL, NULL);
}

gboolean ctx_write(PropertyContext * ctx, const gchar * schemasHome,
		   const gchar * owner,  FILE * outF)
{
    xml_tags_write(outF, "schema", XML_TAG_TYPE_BEGIN_ONLY, NULL, NULL);
    gchar buf[XML_BUFFER_SIZE];
    g_snprintf(buf, XML_BUFFER_SIZE, "/schemas%s/%s",
	       schemasHome, ctx->spec->key);
    xml_tags_write(outF, "key", XML_TAG_TYPE_SHORT, NULL, buf);
    xml_tags_write(outF, "applyto", XML_TAG_TYPE_SHORT, NULL,
		   buf + strlen("/schemas"));
    xml_tags_write(outF, "owner", XML_TAG_TYPE_SHORT, NULL, owner);
    switch (ctx->spec->valueType) {
    case G_TYPE_BOOLEAN:
	xml_tags_write(outF, "type", XML_TAG_TYPE_SHORT, NULL, "bool");
	break;
    case G_TYPE_INT:
    case G_TYPE_UINT:
	xml_tags_write(outF, "type", XML_TAG_TYPE_SHORT, NULL, "int");
	break;
    case G_TYPE_STRING:
	xml_tags_write(outF, "type", XML_TAG_TYPE_SHORT, NULL, "string");
	break;
    default:
	break;
    }
    if (ctx->spec->defaultValue) {
	xml_tags_write(outF, "default", XML_TAG_TYPE_SHORT, NULL,
		       ctx->spec->defaultValue);
    }
    int i;
    for (i = 0; localeArray[i] != NULL; i++) {
	ctx_write_locale(ctx, outF, localeArray[i]);
    }
    
    setlocale(LC_ALL, NULL);
    xml_tags_write(outF, "schema", XML_TAG_TYPE_END_ONLY, NULL, NULL);
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
    mkdg_log(INFO, "write_gconf_schemes_file(%s,%s,%s,%s)", filename, owner, schemasHome, localeStr);
    FILE *outF = fopen(filename, "w");
    if (outF == NULL) {
	mkdg_log(DEBUG,
		 "write_gconf_schemas_file(%s) file %s cannot be written!",
		 filename, filename);
	return FALSE;
    }
    localeArray = g_strsplit_set(localeStr, ":;", -1);
    gchar ** loc;
    for(loc=localeArray; *loc!=NULL ; loc++){
	mkdg_log(DEBUG,
		"write_gconf_schemas_file() locale=%s",*loc);
		}

    /* Header */
    xml_tags_write(outF, "gconfschemafile",
		   XML_TAG_TYPE_BEGIN_ONLY, NULL, NULL);
    xml_tags_write(outF, "schemalist",
		   XML_TAG_TYPE_BEGIN_ONLY, NULL, NULL);
    /* Body */
    PropertyContextArray *array =
	IBusChewingConfig_get_PropertyContextArray(NULL);
    guint i;
    for (i = 0; i < array->len; i++) {
	PropertyContext *ctx = PropertyContextArray_index(array, i);
	ctx_write(ctx, schemasHome, owner, outF);
    }

    /* Footer */
    xml_tags_write(outF, "schemalist", XML_TAG_TYPE_END_ONLY, NULL, NULL);
    xml_tags_write(outF, "gconfschemafile",
		   XML_TAG_TYPE_END_ONLY, NULL, NULL);
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
	return 1;
    }
    g_option_context_free(context);
    if (argc < 2) {
	fprintf(stderr, "Specify filename of outputing schemas file!\n");
	return 1;
    }
    if (localeStr==NULL){
	localeStr=DEFAULT_LOCALES;
    }else{
	isLocaleStrAllocated=TRUE;
    }
    mkdg_log_set_level(verbose);
    schemasFilename = argv[1];
    g_type_init();
    gboolean result =
	write_gconf_schemas_file(schemasFilename, "ibus-chewing",
				 "/desktop/ibus/"
				 IBUS_CHEWING_CONFIG_SECTION,
				 localeStr);
    if (isLocaleStrAllocated){
	g_free(localeStr);
    }
    if (!result) {
	return 2;
    }
    return 0;
}
