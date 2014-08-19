#include <stdlib.h>
#include <glib/gprintf.h>
#include "ibus-chewing-util.h"
#include "MakerDialogProperty.h"

/*============================================
 * Supporting functions
 */
gchar *GValue_to_string(GValue * gValue)
{
    static gchar result[MAKER_DIALOG_VALUE_LENGTH];
    result[0] = '\0';
    GType gType = g_value_get_gtype(gValue);
    guint uintValue;
    int intValue;
    switch (gType) {
    case G_TYPE_BOOLEAN:
	if (g_value_get_boolean(gValue)) {
	    g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "1");
	} else {
	    g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "0");
	}
	break;
    case G_TYPE_UINT:
	uintValue = g_value_get_uint(gValue);
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%u", uintValue);
	break;
    case G_TYPE_INT:
	intValue = g_value_get_int(gValue);
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%d", intValue);
	break;
    case G_TYPE_STRING:
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH,
		   g_value_get_string(gValue));
	break;
    default:
	break;
    }
    return result;
}

gboolean GValue_from_string(GValue * gValue, gchar * str)
{
    GType gType = g_value_get_gtype(gValue);
    guint uintValue;
    int intValue;
    gchar *endPtr = NULL;
    switch (gType) {
    case G_TYPE_BOOLEAN:
	if (STRING_IS_EMPTY(str)) {
	    g_value_set_boolean(gValue, FALSE);
	} else if (STRING_EQUALS(str, "0")) {
	    g_value_set_boolean(gValue, FALSE);
	} else if (STRING_EQUALS(str, "F")) {
	    g_value_set_boolean(gValue, FALSE);
	} else if (STRING_EQUALS(str, "f")) {
	    g_value_set_boolean(gValue, FALSE);
	} else if (STRING_EQUALS(str, "FALSE")) {
	    g_value_set_boolean(gValue, FALSE);
	} else if (STRING_EQUALS(str, "false")) {
	    g_value_set_boolean(gValue, FALSE);
	} else {
	    g_value_set_boolean(gValue, TRUE);
	}
	return TRUE;
    case G_TYPE_UINT:
	uintValue = g_ascii_strtoull(str, &endPtr, 10);
	if (uintValue == 0 && endPtr == str) {
	    return FALSE;
	}
	g_value_set_uint(gValue, uintValue);
	return TRUE;
    case G_TYPE_INT:
	intValue = g_ascii_strtoll(str, &endPtr, 10);
	if (intValue == 0 && endPtr == str) {
	    return FALSE;
	}
	g_value_set_int(gValue, intValue);
	return TRUE;
    case G_TYPE_STRING:
	g_value_set_string(gValue, str);
	return TRUE;
    default:
	break;
    }
    return FALSE;
}

/*============================================
 * PropertyContext Methods
 */

PropertyContext *PropertyContext_new(PropertySpec * spec, GValue * value,
				     gpointer parent, gpointer userData)
{
    if (spec == NULL) {
	return NULL;
    }
    PropertyContext *result = g_new0(PropertyContext, 1);
    if (value == NULL) {
	g_value_init(&(result->value), spec->valueType);
	if (!STRING_IS_EMPTY(spec->defaultValue)) {
	    if (!PropertyContext_from_string(result, spec->defaultValue)) {
		return NULL;
	    }
	}
    } else if (G_IS_VALUE(value)) {
	g_value_copy(value, &(result->value));
    } else {
	return NULL;
    }
    result->spec = spec;
    result->parent = parent;
    result->userData = userData;
    return result;
}

gchar *PropertyContext_to_string(PropertyContext * ctx)
{
    if (ctx == NULL) {
	return NULL;
    }
    return GValue_to_string(&(ctx->value));
}

gboolean PropertyContext_from_string(PropertyContext * ctx, gchar * str)
{
    if (ctx == NULL) {
	return FALSE;
    }
    return GValue_from_string(&(ctx->value), str);
}

gboolean PropertyContext_from_GValue(PropertyContext * ctx, GValue * value)
{
    if (ctx == NULL) {
	return FALSE;
    }
    if (!G_IS_VALUE(value)) {
	return FALSE;
    }
    g_value_copy(value, &(ctx->value));
    return TRUE;
}


/*============================================
 * PropertyContextArray Methods
 */

PropertyContextArray *PropertyContextArray_from_spec_array(PropertySpec
							   specs[],
							   gpointer parent,
							   gpointer
							   userData)
{
    gsize arraySize = 0;
    gsize i;
    for (i = 0; specs[i].valueType != G_TYPE_INVALID; i++) {
	arraySize++;
    }
    PropertyContextArray *result = g_ptr_array_sized_new(arraySize);
    for (i = 0; i < arraySize; i++) {
	PropertyContext *ctx = PropertyContext_new(&specs[i], NULL, parent,
						   userData);
	g_ptr_array_add(result, (gpointer) ctx);
    }
    return result;
}

PropertyContext *PropertyContextArray_index(PropertyContextArray * array,
					    guint index)
{
    return (PropertyContext *) g_ptr_array_index(array, index);
}
