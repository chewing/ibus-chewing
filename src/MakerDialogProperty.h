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

/**
 * SECTION:MakerDialogProperty
 * @short_description: A property stores a value for a configuration
 * @title: MakerDialog Property
 * @stability: Stable
 * @include: MakerDailogProperty.h
 *
 * A MakerDialog property stores a value for a configuration options.
 */

#ifndef _MAKER_DIALOG_PROPERTY_H_
#define _MAKER_DIALOG_PROPERTY_H_
#include <glib.h>
#include <glib-object.h>
#include "MakerDialogUtil.h"
#include "MakerDialogPropertySpec.h"
#include "MakerDialogBackend.h"

struct _PropertyContext {
    MkdgPropertySpec *spec;
    GValue value;		//<! Property Value
    MkdgBackend *backend;	//<! Backend.
    gpointer parent;		//<! The object that this property belongs to. parent need to handle apply
    gpointer auxData;
};

typedef struct {
    GPtrArray *contexts;
    MkdgBackend *backend;
    gpointer auxData;
} MkdgProperties;


PropertyContext *property_context_new(MkdgPropertySpec * spec,
				      MkdgBackend * backend,
				      gpointer parent, gpointer auxData);

gchar *property_context_to_string(PropertyContext * ctx);

gboolean property_context_from_string(PropertyContext * ctx,
				      const gchar * str);

gboolean property_context_from_gvalue(PropertyContext * ctx,
				      GValue * value);

GValue *property_context_read(PropertyContext * ctx, gpointer userData);

gboolean property_context_write(PropertyContext * ctx, gpointer userData);

GValue *property_context_get(PropertyContext * ctx);

gboolean property_context_set(PropertyContext * ctx, GValue * value);

GValue *property_context_load(PropertyContext * ctx, gpointer userData);

gboolean property_context_save(PropertyContext * ctx, GValue * value,
			       gpointer userData);

gboolean property_context_apply(PropertyContext * ctx, gpointer userData);

gboolean property_context_use(PropertyContext * ctx, gpointer userData);

void property_context_free(PropertyContext * ctx);

MkdgProperties *mkdg_properties_from_spec_array(MkdgPropertySpec specs[],
						MkdgBackend * backend,
						gpointer parent,
						gpointer auxData);

PropertyContext *mkdg_properties_find_by_key(MkdgProperties * properties,
					     const gchar * key);


PropertyContext *mkdg_properties_index(MkdgProperties * array,
				       guint index);

PropertyContext *mkdg_properties_index(MkdgProperties * array,
				       guint index);

GValue *mkdg_properties_get_by_key(MkdgProperties * properties,
				   const gchar * key);

#define mkdg_properties_get_boolean_by_key(properties, key) g_value_get_boolean(mkdg_properties_get_by_key(properties, key))

#define mkdg_properties_get_int_by_key(properties, key) g_value_get_int(mkdg_properties_get_by_key(properties, key))

#define mkdg_properties_get_string_by_key(properties, key) g_value_get_string(mkdg_properties_get_by_key(properties, key))

gboolean mkdg_properties_set_by_key(MkdgProperties * properties,
				   const gchar * key, GValue *value);

gboolean mkdg_properties_set_boolean_by_key(MkdgProperties * properties,
				   const gchar * key, gboolean boolValue);

gboolean mkdg_properties_set_int_by_key(MkdgProperties * properties,
				   const gchar * key, gint intValue);

gboolean mkdg_properties_set_string_by_key(MkdgProperties * properties,
				   const gchar * key, const gchar *stringValue);

GValue *mkdg_properties_load_by_key(MkdgProperties * properties,
				    const gchar * key, gpointer userData);

gboolean mkdg_properties_apply_by_key(MkdgProperties * properties,
				   const gchar * key, gpointer userData);


gsize mkdg_properties_size(MkdgProperties * properties);

gboolean mkdg_properties_use_all(MkdgProperties * properties,
				 gpointer userData);

void mkdg_properties_free(MkdgProperties * properties);
#endif
