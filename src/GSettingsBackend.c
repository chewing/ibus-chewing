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

const gchar *mkdg_g_variant_type_string(GType gType)
{
    switch (gType) {
    case G_TYPE_BOOLEAN:
	return (const gchar *) G_VARIANT_TYPE_BOOLEAN;
    case G_TYPE_UINT:
	return (const gchar *) G_VARIANT_TYPE_UINT32;
    case G_TYPE_UINT64:
	return (const gchar *) G_VARIANT_TYPE_UINT64;
    case G_TYPE_INT:
	return (const gchar *) G_VARIANT_TYPE_INT32;
    case G_TYPE_INT64:
	return (const gchar *) G_VARIANT_TYPE_INT64;
    case G_TYPE_STRING:
	return (const gchar *) G_VARIANT_TYPE_STRING;
    default:
	break;
    }
    return NULL;
}

gchar *mkdg_g_settings_attr_append(gchar * buf, gint bufferSize,
				   const gchar * attr, const gchar * value)
{
    if (STRING_IS_EMPTY(attr))
	return buf;
    if (!STRING_IS_EMPTY(buf))
	g_strlcat(buf, " ", bufferSize);

    return buf;
}


/*============================================
 * Class methods
 */

#define KEY_BUFFER_SIZE 300

/*=== Start Schema Writing ===*/
gboolean mkdg_g_settings_write_schema_spec(FILE * file,
					   MkdgPropertySpec * spec)
{
    gchar attrBuf[KEY_BUFFER_SIZE];
    const gchar *typeString = mkdg_g_variant_type_string(spec->valueType);
    g_strlcpy(attrBuf, "", KEY_BUFFER_SIZE);
    mkdg_xml_attr_append(attrBuf, KEY_BUFFER_SIZE, "name", spec->key);
    mkdg_xml_attr_append(attrBuf, KEY_BUFFER_SIZE, "type", typeString);
    mkdg_xml_tags_write(file, "key",
			MKDG_XML_TAG_TYPE_BEGIN_ONLY, attrBuf, NULL);
    /* tag default */
    g_strlcpy(attrBuf, "", KEY_BUFFER_SIZE);
    if (spec->propertyFlags & MKDG_PROPERTY_FLAG_HAS_TRANSLATION) {
	mkdg_xml_attr_append(attrBuf, KEY_BUFFER_SIZE, "l10n", "messages");
	if (!STRING_IS_EMPTY(spec->translationContext)) {
	    mkdg_xml_attr_append(attrBuf, KEY_BUFFER_SIZE, "context",
				 spec->translationContext);
	}
    }

    gchar *valueStr = NULL;
    switch (spec->valueType) {
    case G_TYPE_BOOLEAN:
	valueStr =
	    g_strdup_printf("%s",
			    (STRING_EQUALS(spec->defaultValue, "0")) ?
			    "false" : "true");
	break;
    case G_TYPE_STRING:
	valueStr = g_strdup_printf("\"%s\"", spec->defaultValue);
	break;
    default:
	valueStr = g_strdup_printf("%s", spec->defaultValue);
	break;
    }
    mkdg_xml_tags_write(file, "default",
			MKDG_XML_TAG_TYPE_SHORT, attrBuf, valueStr);
    g_free(valueStr);
    mkdg_xml_tags_write(file, "summary",
			MKDG_XML_TAG_TYPE_SHORT, NULL, spec->label);
    mkdg_xml_tags_write(file, "description",
			MKDG_XML_TAG_TYPE_LONG, NULL, spec->tooltip);
    mkdg_xml_tags_write(file, "key",
			MKDG_XML_TAG_TYPE_END_ONLY, NULL, NULL);
    return TRUE;
}

gboolean mkdg_g_settings_write_schema_from_spec_array(const gchar *
						      schemaId,
						      const gchar *
						      basePath,
						      FILE * file,
						      MkdgPropertySpec
						      specs[],
						      const gchar *
						      gettextDomain)
{
    mkdg_log(INFO,
	     "mkdg_g_settings_write_schema_from_spec_array(%s,%s,-,-,-,%s)",
	     schemaId, basePath, gettextDomain);
    gchar attrBuf[KEY_BUFFER_SIZE];
    gchar path[KEY_BUFFER_SIZE];
    g_snprintf(path, KEY_BUFFER_SIZE, "%s", basePath);

    /* Header */
    mkdg_xml_tags_write(file, "schemalist",
			MKDG_XML_TAG_TYPE_BEGIN_ONLY, NULL, NULL);
    g_strlcpy(attrBuf, "", KEY_BUFFER_SIZE);
    mkdg_xml_attr_append(attrBuf, KEY_BUFFER_SIZE, "id", schemaId);
    mkdg_xml_attr_append(attrBuf, KEY_BUFFER_SIZE, "path", path);
    mkdg_xml_attr_append(attrBuf, KEY_BUFFER_SIZE,
			 "gettext-domain", gettextDomain);
    mkdg_xml_tags_write(file, "schema",
			MKDG_XML_TAG_TYPE_BEGIN_ONLY, attrBuf, NULL);
    /* Body */
    gint i;
    for (i = 0; (&specs[i])->valueType != G_TYPE_INVALID; i++) {
	mkdg_g_settings_write_schema_spec(file, &specs[i]);
    }

    /* Footer */
    mkdg_xml_tags_write(file, "schema",
			MKDG_XML_TAG_TYPE_END_ONLY, NULL, NULL);
    mkdg_xml_tags_write(file, "schemalist",
			MKDG_XML_TAG_TYPE_END_ONLY, NULL, NULL);
    return TRUE;
}

/*=== End Schema Writing ===*/

GValue *mkdg_g_settings_read_value(GSettings * settings,
				   GValue * value, const gchar * key)
{
    GVariant *confValue = g_settings_get_value(settings, key);
    if (confValue == NULL) {
	mkdg_log(ERROR, "mkdg_g_settings_read_value(-,-,%s)", key);
	return NULL;
    }
    mkdg_g_variant_to_g_value(confValue, value);
    return value;
}

GValue *mkdg_g_settings_backend_read_value(MkdgBackend * backend,
					   GValue * value,
					   const gchar * section,
					   const gchar * key,
					   gpointer userData)
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
    GVariant *confValue = mkdg_g_value_to_g_variant(value);
    g_variant_ref_sink(confValue);
    gboolean result = g_settings_set_value(config, key, confValue);
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
    result->readFunc = mkdg_g_settings_backend_read_value;
    result->writeFunc = mkdg_g_settings_backend_write_value;
    return result;
}
