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
#include <glib.h>
#include <gconf/gconf.h>
#include <gconf/gconf-client.h>
#include "MakerDialogUtil.h"
#include "GConf2Backend.h"

static GHashTable *camalCasedKeyTable = NULL;

gchar *gconf2_backend_get_key(MkdgBackend * backend,
			      const gchar * section, const gchar * key,
			      gpointer userData);

static GValue *gconf_value_to_g_value(GConfValue * confValue,
				      GValue * value)
{
    GType gType = G_VALUE_TYPE(value);
    switch (confValue->type) {
    case GCONF_VALUE_BOOL:
	if (gType != G_TYPE_BOOLEAN) {
	    return NULL;
	}
	g_value_set_boolean(value, gconf_value_get_bool(confValue));
	break;
    case GCONF_VALUE_INT:
	if (gType != G_TYPE_INT && gType != G_TYPE_UINT) {
	    return NULL;
	} else if (gType == G_TYPE_INT) {
	    g_value_set_int(value, gconf_value_get_int(confValue));
	} else {
	    g_value_set_uint(value, gconf_value_get_int(confValue));
	}
	break;
    case GCONF_VALUE_STRING:
	if (gType != G_TYPE_STRING) {
	    return NULL;
	}
	g_value_set_string(value, gconf_value_get_string(confValue));
	break;
    default:
	return NULL;
    }
    return value;
}

static GConfValue *gconf_value_new_g_value(GValue * value)
{
    GConfValue *confValue;
    GType gType = G_VALUE_TYPE(value);
    switch (gType) {
    case G_TYPE_BOOLEAN:
	confValue = gconf_value_new(GCONF_VALUE_BOOL);
	gconf_value_set_bool(confValue, g_value_get_boolean(value));
	break;
    case G_TYPE_INT:
	confValue = gconf_value_new(GCONF_VALUE_INT);
	gconf_value_set_int(confValue, g_value_get_int(value));
	break;
    case G_TYPE_UINT:
	confValue = gconf_value_new(GCONF_VALUE_INT);
	gconf_value_set_int(confValue, g_value_get_uint(value));
	break;
    case G_TYPE_STRING:
	confValue = gconf_value_new(GCONF_VALUE_STRING);
	gconf_value_set_string(confValue, g_value_get_string(value));
	break;
    default:
	return NULL;
    }
    return confValue;
}

#define KEY_BUFFER_SIZE 100
static gchar *to_real_key(gchar * confKey, MkdgBackend * backend,
			  const gchar * section, const gchar * key)
{
    gchar *camalCasedKey = gconf2_backend_get_key(backend, section, key, NULL);

    if (!STRING_IS_EMPTY(backend->basePath)) {
	g_strlcpy(confKey, backend->basePath, KEY_BUFFER_SIZE);
	g_strlcat(confKey, "/", KEY_BUFFER_SIZE);
    } else {
	g_strlcpy(confKey, "/", KEY_BUFFER_SIZE);
    }

    if (!STRING_IS_EMPTY(section)) {
	g_strlcat(confKey, section, KEY_BUFFER_SIZE);
	g_strlcat(confKey, "/", KEY_BUFFER_SIZE);
    }
    g_strlcat(confKey, camalCasedKey, KEY_BUFFER_SIZE);
    return confKey;
}

/*============================================
 * Interface routines
 */
gchar *gconf2_backend_get_key(MkdgBackend * backend,
			      const gchar * section, const gchar * key,
			      gpointer userData)
{
    gchar *camalCasedKey =
	(gchar *) g_hash_table_lookup(camalCasedKeyTable, key);
    if (camalCasedKey == NULL) {
	camalCasedKey = mkdg_str_dash_to_camel(key);
	g_hash_table_insert(camalCasedKeyTable, (gpointer) key,
			    camalCasedKey);
    }
    return camalCasedKey;
}

GValue *gconf2_backend_read_value(MkdgBackend * backend,
				  GValue * value,
				  const gchar * section,
				  const gchar * key, gpointer userData)
{
    GConfClient *config = (GConfClient *) backend->config;
    GError *err = NULL;
    gchar confKey[KEY_BUFFER_SIZE];
    to_real_key(confKey, backend, section, key);
    GConfValue *confValue = gconf_client_get(config, confKey, &err);

    if (err != NULL) {
	mkdg_log(ERROR, "gconf2_backend_read_value(-,-,%s,%s,-): %s",
		 section, key, err->message);
	return NULL;
    }
    return gconf_value_to_g_value(confValue, value);
}

gboolean gconf2_backend_write_value(MkdgBackend * backend,
				    GValue * value,
				    const gchar * section,
				    const gchar * key, gpointer userData)
{
    GConfClient *config = (GConfClient *) backend->config;
    GError *err = NULL;
    gchar confKey[KEY_BUFFER_SIZE];
    to_real_key(confKey, backend, section, key);
    GConfValue *confValue = gconf_value_new_g_value(value);
    gconf_client_set(config, confKey, confValue, &err);
    gconf_value_free(confValue);
    if (err != NULL) {
	mkdg_log(ERROR, "gconf2_backend_write_value(-,-,%s,%s,-): %s",
		 section, key, err->message);
	return FALSE;
    }
    return TRUE;
}

MkdgBackend *gconf2_backend_new(const gchar * basePath, gpointer auxData)
{
    GConfClient *client = gconf_client_get_default();
    MkdgBackend *result = mkdg_backend_new(GCONF2_BACKEND_ID,
					   (gpointer) client, basePath,
					   auxData);
    camalCasedKeyTable = g_hash_table_new(g_str_hash, g_str_equal);
    result->getKeyFunc = gconf2_backend_get_key;
    result->readFunc = gconf2_backend_read_value;
    result->writeFunc = gconf2_backend_write_value;
    return result;
}
