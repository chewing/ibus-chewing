#include "MakerDialogUtil.h"
#include <glib.h>
#include <stdarg.h>

static MkdgLogLevel debugLevel = WARN;

#define MKDG_LOG_DOMAIN_LEN 20
static gchar mkdgLogDomain[MKDG_LOG_DOMAIN_LEN] = "MKDG";

static FILE *logFile = NULL;
void mkdg_log_set_level(MkdgLogLevel level) { debugLevel = level; }

void mkdg_log_set_file(FILE *file) { logFile = file; }

void mkdg_logv_domain(const gchar *domain, MkdgLogLevel level,
                      const gchar *format, va_list argList) {
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

void mkdg_log(MkdgLogLevel level, const gchar *format, ...) {
    if (level > debugLevel)
        return;
    va_list argList;

    va_start(argList, format);
    mkdg_logv_domain(mkdgLogDomain, level, format, argList);
    va_end(argList);
}

void mkdg_log_domain(const gchar *domain, MkdgLogLevel level,
                     const gchar *format, ...) {
    if (level > debugLevel)
        return;
    va_list argList;

    va_start(argList, format);
    mkdg_logv_domain(domain, level, format, argList);
    va_end(argList);
}

gboolean mkdg_g_value_reset(GValue *value, GType type, gboolean overwrite) {
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

gchar *mkdg_g_value_to_string(GValue *value) {
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

gboolean mkdg_g_value_from_string(GValue *value, const gchar *str) {
    mkdg_log(DEBUG, "mkdg_g_value_from_string(-,%s)", str);
    if (!G_IS_VALUE(value)) {
        mkdg_log(ERROR, "mkdg_g_value_from_string(): Failed to get GType");
        return FALSE;
    }
    GType gType = G_VALUE_TYPE(value);

    mkdg_log(DEBUG, "mkdg_g_value_from_string() gType=%s", g_type_name(gType));
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

void mkdg_g_variant_to_g_value(GVariant *gVar, GValue *value) {
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

GVariant *mkdg_g_value_to_g_variant(GValue *value) {
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
