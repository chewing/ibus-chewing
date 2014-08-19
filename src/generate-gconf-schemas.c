#include "IBusChewingEngine-def.c"
#include "ibus-chewing-util.h"
#include "IBusChewingConfig.h"

static gint verbose = 0;
static gchar *schemasFilename = NULL;
static gchar *localeStr = NULL;
static const gchar localeDefault[] = "C";

static const GOptionEntry entries[] = {
    {"verbose", 'v', 0, G_OPTION_ARG_INT, &verbose,
     "Verbose level. The higher the level, the more the debug messages.",
     "[integer]"},
    {"locale", 'l', 0, G_OPTION_ARG_STRING, &localeStr,
     "Supported locale. Use ';' to separate locales.",
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
			       (!isEmptyString(tagName)) ? tagName : "",
			       (!isEmptyString(attribute)) ? " " : "",
			       (!isEmptyString(attribute)) ? attribute :
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
    IBUS_CHEWING_LOG(INFO, ",xml_tags_write:%s", strBuf->str);
    fprintf(outF, "%s\n", strBuf->str);

    if (type == XML_TAG_TYPE_BEGIN_ONLY)
	indentLevel++;
    g_string_free(strBuf, TRUE);
}

typedef struct {
    const gchar *schemasHome;
    const gchar *owner;
    const gchar *locales;
    FILE *outF;
} SchemasFileData;

static void ctx_write_locale(PropertyContext * ctx,
			     SchemasFileData * sData,
			     const gchar * localeStr)
{
    gchar buf[50];
    g_snprintf(buf, 50, "name=\"%s\"", localeStr);
    setlocale(LC_MESSAGES, localeStr);
    xml_tags_write(sData->outF, "locale", XML_TAG_TYPE_BEGIN_ONLY, buf,
		   NULL);
    xml_tags_write(sData->outF, "short", XML_TAG_TYPE_SHORT, NULL,
		   gettext(ctx->spec->label));
    xml_tags_write(sData->outF, "long", XML_TAG_TYPE_LONG, NULL,
		   gettext(ctx->spec->tooltip));
    xml_tags_write(sData->outF, "locale", XML_TAG_TYPE_END_ONLY, NULL,
		   NULL);
}

static void ctx_write_callback(gpointer data, gpointer user_data)
{
    PropertyContext *ctx = (PropertyContext *) data;
    SchemasFileData *sData = (SchemasFileData *) user_data;
    xml_tags_write(sData->outF, "schema", XML_TAG_TYPE_BEGIN_ONLY, NULL,
		   NULL);
    gchar buf[STRING_BUFFER_SIZE_DEFAULT];
    g_snprintf(buf, STRING_BUFFER_SIZE_DEFAULT, "/schemas%s/%s",
	       sData->schemasHome, ctx->spec->key);
    xml_tags_write(sData->outF, "key", XML_TAG_TYPE_SHORT, NULL, buf);
    xml_tags_write(sData->outF, "applyto", XML_TAG_TYPE_SHORT, NULL,
		   buf + strlen("/schemas"));
    xml_tags_write(sData->outF, "owner", XML_TAG_TYPE_SHORT, NULL,
		   sData->owner);
    switch (ctx->spec->valueType) {
    case G_TYPE_BOOLEAN:
	xml_tags_write(sData->outF, "type", XML_TAG_TYPE_SHORT, NULL,
		       "bool");
	break;
    case G_TYPE_INT:
    case G_TYPE_UINT:
	xml_tags_write(sData->outF, "type", XML_TAG_TYPE_SHORT, NULL,
		       "int");
	break;
    case G_TYPE_STRING:
	xml_tags_write(sData->outF, "type", XML_TAG_TYPE_SHORT, NULL,
		       "string");
	break;
    default:
	break;
    }
    if (ctx->spec->defaultValue) {
	xml_tags_write(sData->outF, "default", XML_TAG_TYPE_SHORT, NULL,
		       ctx->spec->defaultValue);
    }
    gchar **localeArray = g_strsplit_set(sData->locales, ":;", -1);
    int i;
    for (i = 0; localeArray[i] != NULL; i++) {
	ctx_write_locale(ctx, sData, localeArray[i]);
    }
    setlocale(LC_ALL, NULL);
    xml_tags_write(sData->outF, "schema", XML_TAG_TYPE_END_ONLY, NULL,
		   NULL);
}

/**
 * write_gconf_schemas_file:
 * @iConfig: An IBusChewingConfig.
 * @filename: Filename for output.
 * @owner: Owner of the schemas.
 * @schemasHome: The "home direcory" of schemas.
 * @locales: Supported locales, use ';' as delimiter.
 * @returns: TRUE if succeed; FALSE otherwise.
 *
 * Output the parameters as GConf schemes file.
 */
gboolean write_gconf_schemas_file(IBusChewingConfig * iConfig,
				  const gchar * filename,
				  const gchar * owner,
				  const gchar *
				  schemasHome, const gchar * locales)
{
    IBUS_CHEWING_LOG(INFO,
		     "IBusChewingConfig_write_gconf_schemes_file(%s)",
		     filename);
    FILE *outF = fopen(filename, "w");
    if (outF == NULL) {
	IBUS_CHEWING_LOG(DEBUG,
			 "write_gconf_schemas_file(%s) file %s cannot be written!",
			 filename, filename);
	return FALSE;
    }
    xml_tags_write(outF, "gconfschemafile",
		   XML_TAG_TYPE_BEGIN_ONLY, NULL, NULL);
    xml_tags_write(outF, "schemalist",
		   XML_TAG_TYPE_BEGIN_ONLY, NULL, NULL);
    SchemasFileData sData;
    sData.schemasHome = schemasHome;
    sData.owner = owner;
    sData.locales = locales;
    sData.outF = outF;
    g_ptr_array_foreach(iConfig->_priv->propList, ctx_write_callback,
			&sData);
    xml_tags_write(outF, "schemalist", XML_TAG_TYPE_END_ONLY, NULL, NULL);
    xml_tags_write(outF, "gconfschemafile",
		   XML_TAG_TYPE_END_ONLY, NULL, NULL);
    if (fclose(outF))
	return FALSE;
    IBUS_CHEWING_LOG(DEBUG,
		     "write_gconf_schemas_file(%s) ... done.", filename);
    return TRUE;
}

gboolean schemas_get_definition()
{
    MakerDialog *dialog = maker_dialog_new();
    maker_dialog_set_verbose_level(dialog, verbose);
    int i;
    for (i = 0; propSpecs[i].valueType != G_TYPE_INVALID; i++) {
	maker_dialog_add_property_no_gui(dialog, &propSpecs[i], NULL);
    }
    gboolean ret = write_gconf_schemas_file(dialog, schemasFilename,
					    "ibus-chewing",
					    "/desktop/ibus/" IBUS_CHEWING_CONFIG_SECTION,
					    localeStr);

    return ret;
}

int main(gint argc, gchar * argv[])
{
    GError *error = NULL;
    GOptionContext *context;

    g_type_init();
//    gtk_init(&argc,&argv);

    /* Init i18n messages */
    setlocale(LC_ALL, "");
    bindtextdomain(quote_me(PROJECT_NAME), quote_me(DATA_DIR) "/locale");
    textdomain(quote_me(PROJECT_NAME));

    context = g_option_context_new("schemasFile");

    g_option_context_add_main_entries(context, entries, "ibus-chewing");

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
	g_print("Option parsing failed: %s\n", error->message);
	exit(-1);
    }
    g_option_context_free(context);
    if (!localeStr)
	localeStr = (gchar *) localeDefault;
    if (argc < 2) {
	fprintf(stderr, "Specify filename of outputing schemas file!\n");
	exit(-1);
    }
    schemasFilename = argv[1];
    if (schemas_get_definition()) {
	exit(0);
    }
    exit(1);
    return 0;
}
