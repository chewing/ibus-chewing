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
#include "GSettingsBackend.h"
#include "IBusChewingUtil.h"
#include "IBusConfigBackend.h"
#include "IBusChewingProperties.h"

/*============================================
 * Class methods
 */


#define KEY_BUFFER_SIZE 100
static gchar *to_real_section(gchar * confSection, MkdgBackend * backend,
			      const gchar * section)
{

    if (!STRING_IS_EMPTY(backend->basePath)) {
	g_strlcpy(confSection, backend->basePath, KEY_BUFFER_SIZE);
    } else {
	g_strlcpy(confSection, "", KEY_BUFFER_SIZE);
    }

    if (!STRING_IS_EMPTY(section)) {
	g_strlcat(confSection, section, KEY_BUFFER_SIZE);
	g_strlcat(confSection, "/", KEY_BUFFER_SIZE);
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
    printf("confSection=%s\n", confSection);
#if IBUS_CHECK_VERSION(1, 4, 0)
    GVariant *gVar = ibus_config_get_value(config, confSection,
					   key);
    if (gVar == NULL) {
	return NULL;
    }
    g_variant_ref_sink(gVar);
    mkdg_g_variant_to_g_value(gVar, value);
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
    GVariant *gVar = g_variant_ref_sink(mkdg_g_value_to_g_variant(value));
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
 * basePath is shorter that other backend, as ibus-config should provide ibus level base
 */
MkdgBackend *ibus_config_backend_new(IBusService * service,
				     const gchar * basePath,
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
	mkdg_backend_new(IBUS_CONFIG_BACKEND_ID, (gpointer) config,
			 basePath, auxData);
    result->readFunc = ibus_config_backend_read_value;
    result->writeFunc = ibus_config_backend_write_value;
    return result;
}
