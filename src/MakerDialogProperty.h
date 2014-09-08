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

#define MAKER_DIALOG_VALUE_LENGTH 200
/**
 * MakerDialogPropertyFlags:
 * MAKER_DIALOG_PROPERTY_FLAG_INVISIBLE
 */
typedef enum {
    MAKER_DIALOG_PROPERTY_FLAG_INVISIBLE = 1,
    MAKER_DIALOG_PROPERTY_FLAG_INSENSITIVE = 1 << 1,
    MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE = 1 << 2,
    MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION = 1 << 3,
    MAKER_DIALOG_PROPERTY_FLAG_TRANSLATION_WITH_CONTEXT = 1 << 4,
} MakerDialogPropertyFlags;

typedef struct _PropertyContext PropertyContext;

typedef GValue *(*MkdgGetFunc) (PropertyContext * ctx);
typedef gboolean(*MkdgApplyFunc) (PropertyContext * ctx, GValue * value);
typedef gboolean(*MkdgBoolFunc) (PropertyContext * ctx, gpointer userData);

/**
 * PropertySpec:
 * @valueType:    Type of the value.
 * @key:          A unique Property ID.
 * @pageName:     The name of the tab that contain this property (Translatable).
 * @label:        Label in UI (Translatable).
 * @subSection:   Sub-section in backend.
 * @defaultValue: String represented string value.
 * @validValues:  NULL terminated valid values.
 * @min:          Minimum valid value for numeric types.
 * @max:          Maximum valid value for numeric types.
 * @applyFunc:    Callback function for apply the value.
 * @propertyFlag: Property Flags.
 * @tooltip:      Tooltip of this property (Translatable).
 * @auxData:      Auxiliary data that might be needed somewhere. 
 *
 * A Property Spec describe the characteristic of a property.
 */

typedef struct {
    GType valueType;
    gchar key[30];
    gchar pageName[50];
    gchar label[200];
    gchar subSection[200];
    gchar defaultValue[100];
    const gchar **validValues;
    gchar *translationContext;

    gint min;
    gint max;

    MkdgSetFunc applyFunc;

    MakerDialogPropertyFlags propertyFlags;
    const gchar *tooltip;
    gpointer auxData;
} PropertySpec;

typedef struct {
    GPtrArray *contexts;
    MkdgBackend *backend;
    gpointer auxData;
} MkdgProperties;

struct _PropertyContext {
    PropertySpec *spec;
    GValue value;		//<! Property Value
    MkdgBackend *backend;	//<! Backend.
    gpointer parent;		//<! The object that this property belongs to. parent need to handle apply
    gpointer auxData;
};


PropertyContext *property_context_new(PropertySpec * spec,
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

gboolean propety_context_apply(PropertyContext * ctx, gpointer userData);

gboolean property_context_use(PropertyContext * ctx, GValue * value,
			      gpointer userData);

MkdgProperties *mkdg_properties_from_spec_array(PropertySpec specs[],
						MkdgBackend * backend,
						gpointer parent,
						gpointer auxData);

PropertyContext *mkdg_properties_find_by_key(MkdgProperties * properties,
					     const gchar * key);

PropertyContext *mkdg_properties_index(MkdgProperties * array,
				       guint index);

gsize mkdg_properties_size(MkdgProperties * properties);
#endif
