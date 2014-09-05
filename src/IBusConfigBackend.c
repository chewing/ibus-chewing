/*
 * Copyright © 2014  Red Hat, Inc. All rights reserved.
 * Copyright © 2014  Ding-Yi Chen <dchen@redhat.com>
 *
 * This file is part of the ibus-chewing Project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <ibus.h>
#include "ibus-chewing-util.h"
#include "IBusConfigBackend.h"

/*============================================
 * Supporting functions
 */
#if IBUS_CHECK_VERSION(1, 4, 0)
void g_variant_to_g_value(GVariant * gVar, GValue * gValue)
{
    const GVariantType *gVType = g_variant_get_type(gVar);
    if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_BOOLEAN)) {
	g_value_init(gValue, G_TYPE_BOOLEAN);
	g_value_set_boolean(gValue, g_variant_get_boolean(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT16)) {
	g_value_init(gValue, G_TYPE_UINT);
	g_value_set_uint(gValue, g_variant_get_uint16(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT32)) {
	g_value_init(gValue, G_TYPE_UINT);
	g_value_set_uint(gValue, g_variant_get_uint32(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT64)) {
	g_value_init(gValue, G_TYPE_UINT64);
	g_value_set_uint64(gValue, g_variant_get_uint64(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT16)) {
	g_value_init(gValue, G_TYPE_INT);
	g_value_set_int(gValue, g_variant_get_int16(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT32)) {
	g_value_init(gValue, G_TYPE_INT);
	g_value_set_int(gValue, g_variant_get_int32(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT64)) {
	g_value_init(gValue, G_TYPE_INT);
	g_value_set_int64(gValue, g_variant_get_int64(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_STRING)) {
	g_value_init(gValue, G_TYPE_STRING);
	g_value_set_string(gValue, g_variant_get_string(gVar, NULL));
    }
}

GVariant *g_value_to_g_variant(GValue * gValue)
{
    GType gType = G_VALUE_TYPE(gValue);
    GVariant *gVar = NULL;
    switch (gType) {
    case G_TYPE_BOOLEAN:
	gVar = g_variant_new_boolean(g_value_get_boolean(gValue));
	break;
    case G_TYPE_UINT:
	gVar = g_variant_new_uint32(g_value_get_uint(gValue));
	break;
    case G_TYPE_UINT64:
	gVar = g_variant_new_uint64(g_value_get_uint(gValue));
	break;
    case G_TYPE_INT:
	gVar = g_variant_new_int32(g_value_get_int(gValue));
	break;
    case G_TYPE_INT64:
	gVar = g_variant_new_int64(g_value_get_int(gValue));
	break;
    case G_TYPE_STRING:
	gVar = g_variant_new_string(g_value_get_string(gValue));
	break;
    default:
	break;
    }
    return gVar;
}
#endif

/*============================================
 * Class methods
 */



GValue *ibus_config_backend_load_value(MkdgBackend * backend,
				       GValue * value,
				       const gchar * section,
				       const gchar * key,
				       gpointer userData)
{
    IBusConfig *config = (IBusConfig *) backend->config;
#if IBUS_CHECK_VERSION(1, 4, 0)
    GVariant *gVar = ibus_config_get_value(config, section,
					   key);
    if (gVar == NULL) {
	return NULL;
    }
    g_variant_ref_sink(gVar);
    g_variant_to_g_value(gVar, value);
    g_variant_unref(gVar);
    return value;
#else
    return ibus_config_get_value(config, section, key, value);
#endif
}


gboolean ibus_config_backend_save_value(MkdgBackend * backend,
					GValue * value,
					const gchar * section,
					const gchar * key,
					gpointer userData)
{
    gboolean result = FALSE;
#if IBUS_CHECK_VERSION(1, 4, 0)
    GVariant *gVar = g_variant_ref_sink(g_value_to_g_variant(value));
    if (gVar != NULL) {
	result = ibus_config_set_value(self->config,
				       IBUS_CHEWING_CONFIG_SECTION, key,
				       gVar);
    }
#else
    result =
	ibus_config_set_value(self->config, IBUS_CHEWING_CONFIG_SECTION,
			      value);
#endif

    if (result == FALSE) {
	mkdg_log(WARN,
		 "IBusChewingConfig_set_value(-, %s, -) %s %s",
		 key, _("Failed to set variable"), key);
	return FALSE;
    }

    if (!self->config) {
	mkdg_log(WARN,
		 "IBusChewingConfig_set_value(-, %s, -) %s",
		 key, _("Failed to connect to IBusService"));
	return FALSE;
    }

    PropertyContext ctx;
    ctx.spec = IBusChewingConfig_find_key(key);
    ctx.spec->setFunc(&ctx, value);
    return TRUE;
}

MkdgBackend *ibus_config_backend_new(IBusService * service,
				     gpointer auxData)
{
    IBusConfig *config = NULL;
#if IBUS_CHECK_VERSION(1, 4, 0)
    GError *error = NULL;
    GDBusConnection *connection = ibus_service_get_connection(service);
    config = g_object_ref_sink(ibus_config_new(connection, NULL, &error));
    g_assert(error == NULL);
#else
    GList *connections_list = ibus_service_get_connections(service);
    g_assert(connections_list);
    g_assert(connections_list->data);
    IBusConnection *iConnection =
	(IBusConnection *) connections_list->data;
    config = g_object_ref_sink(ibus_config_new(iConnection));
#endif
    if (config == NULL) {
	return NULL;
    }
    MkdgBackend *result = mkdg_backend_new((gpointer) config, auxData);
    result->loadFunc = ibus_config_backend_load_value;
    result->saveFunc = ibus_config_backend_save_value;
    return result;
}
