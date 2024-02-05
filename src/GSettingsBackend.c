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
#include "MakerDialogUtil.h"
#include "GSettingsBackend.h"

gchar *mkdg_g_variant_to_string(GVariant * gVar)
{
    static gchar result[MAKER_DIALOG_VALUE_LENGTH];

    result[0] = '\0';
    const GVariantType *gVType = g_variant_get_type(gVar);

    if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_BOOLEAN)) {
        g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH,
                   g_variant_get_boolean(gVar) ? "1" : "0");
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT16)) {
        g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%" G_GUINT16_FORMAT,
                   g_variant_get_uint16(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT32)) {
        g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%" G_GUINT32_FORMAT,
                   g_variant_get_uint32(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_UINT64)) {
        g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%" G_GUINT64_FORMAT,
                   g_variant_get_uint64(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT16)) {
        g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%" G_GINT16_FORMAT,
                   g_variant_get_int16(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT32)) {
        g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%" G_GINT32_FORMAT,
                   g_variant_get_int32(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_INT64)) {
        g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%" G_GINT64_FORMAT,
                   g_variant_get_int64(gVar));
    } else if (g_variant_type_is_subtype_of(gVType, G_VARIANT_TYPE_STRING)) {
        g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%s",
                   g_variant_get_string(gVar, NULL));
    }
    return result;
}

/*============================================
 * Class routines
 */

#define KEY_BUFFER_SIZE 300

GValue *mkdg_g_settings_read_value(GSettings * settings,
                                   GValue * value, const gchar * key)
{
    GVariant *confValue = g_settings_get_value(settings, key);

    mkdg_log(DEBUG, "mkdg_g_settings_read_value(-,-,%s)", key);
    if (confValue == NULL) {
        mkdg_log(ERROR, "mkdg_g_settings_read_value(-,-,%s)", key);
        return NULL;
    }
    mkdg_g_variant_to_g_value(confValue, value);
    g_variant_unref(confValue);
    return value;
}


/*============================================
 * Interface routines
 */
gchar *mkdg_g_settings_backend_get_key(MkdgBackend * backend,
                                       const gchar * section,
                                       const gchar * key, gpointer userData)
{
    return (gchar *) key;
}

GValue *mkdg_g_settings_backend_read_value(MkdgBackend * backend,
                                           GValue * value,
                                           const gchar * section,
                                           const gchar * key, gpointer userData)
{
    GSettings *config = (GSettings *) backend->config;

    return mkdg_g_settings_read_value(config, value, key);
}


gboolean mkdg_g_settings_backend_write_value(MkdgBackend *
                                             backend,
                                             GValue *
                                             value,
                                             const gchar *
                                             section,
                                             const gchar *
                                             key, gpointer userData)
{
    GSettings *config = (GSettings *) backend->config;
    GVariant *confValue = g_variant_ref_sink(mkdg_g_value_to_g_variant(value));

    mkdg_log(DEBUG, "mkdg_g_settings_write_value(-,%s,%s) %s",
             mkdg_g_value_to_string(value), key,
             mkdg_g_variant_to_string(confValue));
    gboolean result = g_settings_set_value(config, key, confValue);

    g_settings_sync();
    if (!result) {
        mkdg_log(ERROR,
                 "mkdg_g_settings_backend_write_value(-,%s,%s,%s,-): Fail g_settings_set_value",
                 mkdg_g_value_to_string(value), section, key);
    }
    g_variant_unref(confValue);
    return result;
}

MkdgBackend *mkdg_g_settings_backend_new(const gchar *
                                         schemaId,
                                         const gchar *
                                         basePath, gpointer auxData)
{
    GSettings *client = g_settings_new(schemaId);
    MkdgBackend *result =
        mkdg_backend_new(GSETTINGS_BACKEND_ID, (gpointer) client, basePath,
                         auxData);

    result->getKeyFunc = mkdg_g_settings_backend_get_key;
    result->readFunc = mkdg_g_settings_backend_read_value;
    result->writeFunc = mkdg_g_settings_backend_write_value;
    return result;
}
