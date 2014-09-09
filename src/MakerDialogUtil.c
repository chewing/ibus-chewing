#include <stdarg.h>
#include <glib.h>
#include "MakerDialogUtil.h"

#define MAKER_DIALOG_VALUE_LENGTH 200

static MkdgLogLevel debugLevel = WARN;
#define MKDG_LOG_DOMAIN_LEN 20
static gchar mkdgLogDomain[MKDG_LOG_DOMAIN_LEN] = "MKDG";

void mkdg_log_set_level(MkdgLogLevel level)
{
    debugLevel = level;
}

void mkdg_log_set_domain(const gchar * domain)
{
    g_strlcpy(mkdgLogDomain, domain, MKDG_LOG_DOMAIN_LEN);
}

void mkdg_log(MkdgLogLevel level, const gchar * format, ...)
{
    if (level > debugLevel)
	return;
    va_list arglist;
    va_start(arglist, format);
    GLogLevelFlags flagSet;
    switch (level) {
    case ERROR:
	flagSet = G_LOG_FLAG_FATAL | G_LOG_LEVEL_ERROR;
	break;
    case WARN:
	flagSet = G_LOG_LEVEL_WARNING;
	break;
    case MSG:
	flagSet = G_LOG_LEVEL_MESSAGE;
	break;
    case INFO:
	flagSet = G_LOG_LEVEL_INFO;
	break;
    default:
	flagSet = G_LOG_LEVEL_DEBUG;
	break;
    }
    g_logv(mkdgLogDomain, flagSet, format, arglist);
    va_end(arglist);
}

gboolean mkdg_g_value_reset(GValue *value, GType type){
    if (!G_IS_VALUE(value)){
	g_value_init(value, type);
    }
    if (G_VALUE_TYPE(value)!=type){
	mkdg_log(ERROR, "mkdg_g_value_reset(): type incompatable");
	return FALSE;
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
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH,
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
    }
    GType gType = G_VALUE_TYPE(value);
    mkdg_log(DEBUG, "mkdg_g_value_from_string() gType=%s",
	     g_type_name(gType));
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
