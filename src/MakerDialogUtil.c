#include <stdarg.h>
#include <glib.h>
#include "MakerDialogUtil.h"

static MkdgLogLevel debugLevel = WARN;
#define MKDG_LOG_DOMAIN_LEN 20
static gchar mkdgLogDomain[MKDG_LOG_DOMAIN_LEN] = "MKDG";

static FILE *logFile = NULL;
void mkdg_log_set_level(MkdgLogLevel level)
{
    debugLevel = level;
}

void mkdg_log_set_domain(const gchar * domain)
{
    g_strlcpy(mkdgLogDomain, domain, MKDG_LOG_DOMAIN_LEN);
}

void mkdg_log_set_file(FILE * file)
{
    logFile = file;
}

void mkdg_logv_domain(const gchar * domain, MkdgLogLevel level,
		      const gchar * format, va_list argList)
{
    if (level > debugLevel)
	return;
    GLogLevelFlags flagSet;
    gchar *levelStr = NULL;
    switch (level) {
    case ERROR:
	flagSet = G_LOG_FLAG_FATAL | G_LOG_LEVEL_ERROR;
	levelStr = "ERROR";
	break;
    case WARN:
	flagSet = G_LOG_LEVEL_WARNING;
	levelStr = "WARN";
	break;
    case MSG:
	flagSet = G_LOG_LEVEL_MESSAGE;
	levelStr = "MSG";
	break;
    case INFO:
	flagSet = G_LOG_LEVEL_INFO;
	levelStr = "INFO";
	break;
    default:
	flagSet = G_LOG_LEVEL_DEBUG;
	levelStr = "DEBUG";
	break;
    }
    g_logv(domain, flagSet, format, argList);
    if (logFile != NULL) {
	fprintf(logFile, "%s-%s: ", domain, levelStr);
	vfprintf(logFile, format, argList);
	fprintf(logFile, "\n");
    }
}

void mkdg_log(MkdgLogLevel level, const gchar * format, ...)
{
    if (level > debugLevel)
	return;
    va_list argList;
    va_start(argList, format);
    mkdg_logv_domain(mkdgLogDomain, level, format, argList);
    va_end(argList);
}

void mkdg_log_domain(const gchar * domain, MkdgLogLevel level,
		     const gchar * format, ...)
{
    if (level > debugLevel)
	return;
    va_list argList;
    va_start(argList, format);
    mkdg_logv_domain(domain, level, format, argList);
    va_end(argList);
}

gboolean mkdg_g_value_reset(GValue * value, GType type, gboolean overwrite)
{
    if (!G_IS_VALUE(value)) {
	g_value_init(value, type);
    }
    if (G_VALUE_TYPE(value) != type) {
	if (!overwrite) {
	    mkdg_log(ERROR, "mkdg_g_value_reset(): type incompatible");
	    return FALSE;
	}
    }
    g_value_reset(value);
    return TRUE;
}


gchar *mkdg_g_value_to_string(GValue * value)
{
    static gchar result[MAKER_DIALOG_VALUE_LENGTH];
    result[0] = '\0';
    GType gType = G_VALUE_TYPE(value);
    guint uintValue;
    gint intValue;
    switch (gType) {
    case G_TYPE_BOOLEAN:
	if (g_value_get_boolean(value)) {
	    g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "1");
	} else {
	    g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "0");
	}
	break;
    case G_TYPE_UINT:
	uintValue = g_value_get_uint(value);
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%u", uintValue);
	break;
    case G_TYPE_INT:
	intValue = g_value_get_int(value);
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%d", intValue);
	break;
    case G_TYPE_STRING:
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%s",
		   g_value_get_string(value));
	break;
    default:
	break;
    }
    return result;
}

gboolean mkdg_g_value_from_string(GValue * value, const gchar * str)
{
    mkdg_log(DEBUG, "mkdg_g_value_from_string(-,%s)", str);
    if (!G_IS_VALUE(value)) {
	mkdg_log(ERROR, "mkdg_g_value_from_string(): Failed to get GType");
	return FALSE;
    }
    GType gType = G_VALUE_TYPE(value);
    mkdg_log(DEBUG, "mkdg_g_value_from_string() gType=%s",
    	     g_type_name(gType));
    if (!mkdg_g_value_reset(value, gType, FALSE)) {
	return FALSE;
    }

    guint uintValue;
    gint intValue;
    gchar *endPtr = NULL;
    switch (gType) {
    case G_TYPE_BOOLEAN:
	if (STRING_IS_EMPTY(str)) {
	    g_value_set_boolean(value, FALSE);
	} else if (STRING_EQUALS(str, "0")) {
	    g_value_set_boolean(value, FALSE);
	} else if (STRING_EQUALS(str, "F")) {
	    g_value_set_boolean(value, FALSE);
	} else if (STRING_EQUALS(str, "f")) {
	    g_value_set_boolean(value, FALSE);
	} else if (STRING_EQUALS(str, "FALSE")) {
	    g_value_set_boolean(value, FALSE);
	} else if (STRING_EQUALS(str, "false")) {
	    g_value_set_boolean(value, FALSE);
	} else {
	    g_value_set_boolean(value, TRUE);
	}
	return TRUE;
    case G_TYPE_UINT:
	uintValue = g_ascii_strtoull(str, &endPtr, 10);
	if (uintValue == 0 && endPtr == str) {
	    return FALSE;
	}
	g_value_set_uint(value, uintValue);
	return TRUE;
    case G_TYPE_INT:
	intValue = g_ascii_strtoll(str, &endPtr, 10);
	if (intValue == 0 && endPtr == str) {
	    return FALSE;
	}
	g_value_set_int(value, intValue);
	return TRUE;
    case G_TYPE_STRING:
	g_value_set_string(value, str);
	return TRUE;
    default:
	break;
    }
    return FALSE;
}

void mkdg_g_variant_to_g_value(GVariant * gVar, GValue * value)
{
    const GVariantType *gVType = g_variant_get_type(gVar);
    if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_BOOLEAN)) {
	g_value_set_boolean(value, g_variant_get_boolean(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT16)) {
	g_value_set_uint(value, g_variant_get_uint16(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT32)) {
	g_value_set_uint(value, g_variant_get_uint32(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT64)) {
	g_value_set_uint64(value, g_variant_get_uint64(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT16)) {
	g_value_set_int(value, g_variant_get_int16(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT32)) {
	g_value_set_int(value, g_variant_get_int32(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT64)) {
	g_value_set_int64(value, g_variant_get_int64(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_STRING)) {
	g_value_set_string(value, g_variant_get_string(gVar, NULL));
    }
}

GVariant *mkdg_g_value_to_g_variant(GValue * value)
{
    GType gType = G_VALUE_TYPE(value);
    GVariant *gVar = NULL;
    switch (gType) {
    case G_TYPE_BOOLEAN:
	gVar = g_variant_new_boolean(g_value_get_boolean(value));
	break;
    case G_TYPE_UINT:
	gVar = g_variant_new_uint32(g_value_get_uint(value));
	break;
    case G_TYPE_UINT64:
	gVar = g_variant_new_uint64(g_value_get_uint(value));
	break;
    case G_TYPE_INT:
	gVar = g_variant_new_int32(g_value_get_int(value));
	break;
    case G_TYPE_INT64:
	gVar = g_variant_new_int64(g_value_get_int(value));
	break;
    case G_TYPE_STRING:
	gVar = g_variant_new_string(g_value_get_string(value));
	break;
    default:
	break;
    }
    return gVar;
}

/*============================================
 * MKDG XML functions
 */

#define INDENT_SPACES 4

static void mkdg_xml_append_indent_space(GString * strBuf,
					 gint indentLevel)
{
    int i, indentLen = indentLevel * INDENT_SPACES;
    for (i = 0; i < indentLen; i++) {
	g_string_append_c(strBuf, ' ');
    }
}

static GString *mkdg_xml_tags_to_string(const gchar * tagName,
					MkdgXmlTagType type,
					const gchar * attribute,
					const gchar * value,
					gint indentLevel)
{
    GString *strBuf = g_string_new(NULL);
    mkdg_xml_append_indent_space(strBuf, indentLevel);

    if (type != MKDG_XML_TAG_TYPE_NO_TAG) {
	g_string_append_printf(strBuf, "<%s%s%s%s%s>",
			       (type ==
				MKDG_XML_TAG_TYPE_END_ONLY) ? "/" : "",
			       (!STRING_IS_EMPTY(tagName)) ? tagName : "",
			       (!STRING_IS_EMPTY(attribute)) ? " " : "",
			       (!STRING_IS_EMPTY(attribute)) ? attribute :
			       "",
			       (type ==
				MKDG_XML_TAG_TYPE_EMPTY) ? "/" : "");
    }
    if (type == MKDG_XML_TAG_TYPE_EMPTY)
	return strBuf;
    if (type == MKDG_XML_TAG_TYPE_BEGIN_ONLY)
	return strBuf;
    if (type == MKDG_XML_TAG_TYPE_END_ONLY)
	return strBuf;

    if (type == MKDG_XML_TAG_TYPE_LONG) {
	g_string_append_c(strBuf, '\n');
    }

    if (value) {
	if (type == MKDG_XML_TAG_TYPE_LONG
	    || type == MKDG_XML_TAG_TYPE_NO_TAG) {
	    mkdg_xml_append_indent_space(strBuf, indentLevel + 1);
	    int i, valueLen = strlen(value);
	    for (i = 0; i < valueLen; i++) {
		g_string_append_c(strBuf, value[i]);
		if (value[i] == '\n') {
		    mkdg_xml_append_indent_space(strBuf, indentLevel + 1);
		}
	    }
	    g_string_append_c(strBuf, '\n');
	    if (type == MKDG_XML_TAG_TYPE_LONG) {
		mkdg_xml_append_indent_space(strBuf, indentLevel);
	    }
	} else {
	    g_string_append(strBuf, value);
	}
    }

    if (type == MKDG_XML_TAG_TYPE_LONG || type == MKDG_XML_TAG_TYPE_SHORT) {
	g_string_append_printf(strBuf, "</%s>", tagName);
    }
    return strBuf;
}

gchar *mkdg_xml_attr_append(gchar * buf, gint bufferSize,
			    const gchar * attr, const gchar * value)
{
    if (STRING_IS_EMPTY(attr))
	return buf;
    if (!STRING_IS_EMPTY(buf))
	g_strlcat(buf, " ", bufferSize);

    g_strlcat(buf, attr, bufferSize);
    if (STRING_IS_EMPTY(value))
	return buf;

    g_strlcat(buf, "=\"", bufferSize);
    g_strlcat(buf, value, bufferSize);
    g_strlcat(buf, "\"", bufferSize);
    return buf;
}

gboolean mkdg_xml_tags_write(FILE * outF, const gchar * tagName,
			     MkdgXmlTagType type, const gchar * attribute,
			     const gchar * value)
{
    static int indentLevel = 0;
    if (type == MKDG_XML_TAG_TYPE_END_ONLY)
	indentLevel--;

    GString *strBuf =
	mkdg_xml_tags_to_string(tagName, type, attribute, value,
				indentLevel);
    mkdg_log(INFO, "xml_tags_write:%s", strBuf->str);
    fprintf(outF, "%s\n", strBuf->str);

    if (type == MKDG_XML_TAG_TYPE_BEGIN_ONLY)
	indentLevel++;
    g_string_free(strBuf, TRUE);
    return TRUE;
}
