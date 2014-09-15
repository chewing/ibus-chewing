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
#include "MakerDialogUtil.h"
#include "ibus-chewing-util.h"
#include "IBusConfigBackend.h"
#include "IBusChewingConfig.h"

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


#define KEY_BUFFER_SIZE 100
static gchar *to_real_section(gchar * confSection, MkdgBackend * backend,
			  const gchar * section)
{

    if (!STRING_IS_EMPTY(backend->baseDir)){
	g_strlcpy(confSection, backend->baseDir, KEY_BUFFER_SIZE);
    }else{
	g_strlcpy(confSection, "", KEY_BUFFER_SIZE);
    }

    if (!STRING_IS_EMPTY(section)) {
	g_strlcat(confSection, "/", KEY_BUFFER_SIZE);
	g_strlcat(confSection, section, KEY_BUFFER_SIZE);
    }
    return confSection;
}

GValue *ibus_config_backend_read_value(MkdgBackend * backend,
				       GValue * value,
				       const gchar * section,
				       const gchar * key,
				       gpointer userData)
{
    IBusConfig *config = (IBusConfig *) backend->config;
    gchar confSection[KEY_BUFFER_SIZE];
    to_real_section(confSection, backend, section);
#if IBUS_CHECK_VERSION(1, 4, 0)
    GVariant *gVar = ibus_config_get_value(config, confSection,
					   key);
    if (gVar == NULL) {
	return NULL;
    }
    g_variant_ref_sink(gVar);
    g_variant_to_g_value(gVar, value);
    g_variant_unref(gVar);
    return value;
#else
    return ibus_config_get_value(config, confSection, key, value);
#endif
}


gboolean ibus_config_backend_write_value(MkdgBackend * backend,
					 GValue * value,
					 const gchar * section,
					 const gchar * key,
					 gpointer userData)
{
    gboolean result = FALSE;
    IBusConfig *config = (IBusConfig *) backend->config;
    gchar confSection[KEY_BUFFER_SIZE];
    to_real_section(confSection, backend, section);
#if IBUS_CHECK_VERSION(1, 4, 0)
    GVariant *gVar = g_variant_ref_sink(g_value_to_g_variant(value));
    if (gVar != NULL) {
	result = ibus_config_set_value(config, confSection, key, gVar);
    }
#else
    result = ibus_config_set_value(config, confSection, key, value);
#endif

    if (result == FALSE) {
	mkdg_log(WARN,
		 "ibus_config_backend_write_value(-, %s, -) %s %s",
		 key, "Failed to set variable", key);
	return FALSE;
    }

    if (!config) {
	mkdg_log(WARN,
		 "ibus_config_backend_write_value(-, %s, -) %s",
		 key, "Failed to connect to IBusService");
	return FALSE;
    }
    return TRUE;
}

/*
 * baseDir is shorter that other backend, as ibus-config should provide ibus level base
 */
MkdgBackend *ibus_config_backend_new(IBusService * service,
				     const gchar * baseDir,
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
    MkdgBackend *result =
	mkdg_backend_new((gpointer) config, baseDir, auxData);
    result->readFunc = ibus_config_backend_read_value;
    result->writeFunc = ibus_config_backend_write_value;
    return result;
}
