/*
 * Copyright © 2009,2010  Red Hat, Inc. All rights reserved.
 * Copyright © 2009,2010  Ding-Yi Chen <dchen at redhat.com>
 *
 * This file is part of the ibus-chewing Project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that ill be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#pragma once

#include "GSettingsBackend.h"
#include "IBusChewingPreEdit.h"
#include "IBusChewingProperties.h"
#include "IBusChewingUtil.h"
#include <chewing.h>
#include <ctype.h>
#include <glib-object.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <ibus.h>
#include <libintl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

G_BEGIN_DECLS

typedef enum {
    ENGINE_FLAG_INITIALIZED = 0x1,
    ENGINE_FLAG_ENABLED = 0x2,
    ENGINE_FLAG_FOCUS_IN = 0x4,
    ENGINE_FLAG_IS_PASSWORD = 0x8,
    ENGINE_FLAG_PROPERTIES_REGISTERED = 0x10
} EngineFlag;
#define ENGINE_TYPE_FLAG engine_flag_get_type()
GType engine_flag_get_type(void) G_GNUC_CONST;

extern MkdgPropertySpec propSpecs[];
extern const gchar *page_labels[];
extern const gchar *button_labels[];
extern GtkResponseType button_responses[];

#define cursor_current chewing_cursor_Current(self->icPreEdit->context)

// XXX not defined by ibus
G_DEFINE_AUTOPTR_CLEANUP_FUNC(IBusEngine, g_object_unref)

#define IBUS_TYPE_CHEWING_ENGINE ibus_chewing_engine_get_type()
G_DECLARE_FINAL_TYPE(IBusChewingEngine, ibus_chewing_engine, IBUS,
                     CHEWING_ENGINE, IBusEngine)

void ibus_chewing_engine_refresh_property_list(IBusChewingEngine *self);
void ibus_chewing_engine_hide_property_list(IBusChewingEngine *self);

void ibus_chewing_engine_reset(IBusEngine *self);
void ibus_chewing_engine_enable(IBusEngine *self);
void ibus_chewing_engine_disable(IBusEngine *self);
void ibus_chewing_engine_focus_in(IBusEngine *self);
void ibus_chewing_engine_focus_out(IBusEngine *self);
gboolean ibus_chewing_engine_process_key_event(IBusEngine *self, guint key_sym,
                                               guint keycode, guint modifiers);

G_END_DECLS