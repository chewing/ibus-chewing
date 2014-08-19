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

#ifndef _MAKER_DIALOG_PROPERTY_H_
#define _MAKER_DIALOG_PROPERTY_H_
#include <glib.h>
#include <glib-object.h>

#define MAKER_DIALOG_VALUE_LENGTH 200
typedef enum {
    MAKER_DIALOG_PROPERTY_FLAG_INVISIBLE = 0x1,
    MAKER_DIALOG_PROPERTY_FLAG_INSENSITIVE = 0x2,
    MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE = 0x4,
    MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION = 0x8,
    MAKER_DIALOG_PROPERTY_FLAG_TRANSLATION_WITH_CONTEXT = 0x10,
} MakerDialogPropertyFlags;

typedef struct _PropertyContext PropertyContext;

typedef GValue *(*CallbackGetFunc) (PropertyContext * ctx);
typedef void (*CallbackSetFunc) (PropertyContext * ctx, GValue * value);
typedef gboolean(*CallbackBoolFunc) (PropertyContext * ctx,
				     gpointer userData);

typedef struct {
    GType valueType;
    gchar key[30];
    gchar pageName[50];
    gchar label[200];
    gchar defaultValue[100];
    const gchar **validValues;
    gchar *translationContext;

    gint min;
    gint max;

    CallbackGetFunc getFunc;
    CallbackSetFunc setFunc;

    MakerDialogPropertyFlags propertyFlags;
    const gchar *tooltip;
    gpointer userData;
} PropertySpec;

struct _PropertyContext {
    PropertySpec *spec;
    GValue value;		//<! Property Value
    gpointer parent;		//<! Main object that this property.
    gpointer userData;		//<! User data to be used in callback.
};

typedef GPtrArray PropertyContextArray;

PropertyContext *PropertyContext_new(PropertySpec * spec, GValue * value,
	gpointer parent, gpointer userData);


gchar *PropertyContext_to_string(PropertyContext * ctx);

gboolean PropertyContext_from_string(PropertyContext * ctx, gchar * str);

gboolean PropertyContext_from_GValue(PropertyContext * ctx, GValue * value);

PropertyContextArray *PropertyContextArray_from_spec_array(PropertySpec
	specs[],
	gpointer parent,
	gpointer userData);

PropertyContext * PropertyContextArray_index(PropertyContextArray * array, guint index);


#endif
