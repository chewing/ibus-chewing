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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

/**
 * SECTION:MakerDialogPropertySpec
 * @short_description: A MakerDialog property spec defines the characteristics
 * of a property.
 * @title: MakerDialog Property
 * @stability: Stable
 * @include: MakerDailogPropertySpec.h
 *
 * A MakerDialog property spec defines the characteristics of a property,
 * including value type, display names, default value, and valid values.
 */

#ifndef _MAKER_DIALOG_PROPERTY_SPEC_H_
#define _MAKER_DIALOG_PROPERTY_SPEC_H_
#include <glib-object.h>
#include <glib.h>

/**
 * MkdgPropertyFlags:
 * @MKDG_PROPERTY_FLAG_INVISIBLE:       The property is not visible in UI.
 * @MKDG_PROPERTY_FLAG_INSENSITIVE:     The property is gray-out and not
 * editable.
 * @MKDG_PROPERTY_FLAG_NO_NEW:          The property does not accept new values.
 * @MKDG_PROPERTY_FLAG_HAS_TRANSLATION: The property should show translated
 * value.
 * @MKDG_PROPERTY_FLAG_NO_BACKEND:      The property has no backend, thus will
 * not be saved.
 *
 * MakerDialog flag controls how the property displayed in UI.
 */
typedef enum {
    MKDG_PROPERTY_FLAG_INVISIBLE = 1,
    MKDG_PROPERTY_FLAG_INSENSITIVE = 1 << 1,
    MKDG_PROPERTY_FLAG_NO_NEW = 1 << 2,
    MKDG_PROPERTY_FLAG_HAS_TRANSLATION = 1 << 3,
    MKDG_PROPERTY_FLAG_NO_BACKEND = 1 << 4
} MkdgPropertyFlags;

typedef struct _PropertyContext PropertyContext;

typedef gboolean (*MkdgApplyFunc)(PropertyContext *ctx, gpointer userData);

/**
 * MkdgPropertySpec:
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
 * @propertyFlags: Property Flags.
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

    MkdgApplyFunc applyFunc;

    MkdgPropertyFlags propertyFlags;
    const gchar *tooltip;
    gpointer auxData;
} MkdgPropertySpec;

#endif /* _MAKER_DIALOG_PROPERTY_SPEC_H_ */
