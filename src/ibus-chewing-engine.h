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

#include "IBusChewingPreEdit.h"
#include "IBusChewingUtil.h"
#include <glib-object.h>
#include <glib.h>
#include <ibus.h>

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

// XXX not defined by ibus
G_DEFINE_AUTOPTR_CLEANUP_FUNC(IBusEngine, g_object_unref)

#define IBUS_TYPE_CHEWING_ENGINE ibus_chewing_engine_get_type()
G_DECLARE_FINAL_TYPE(IBusChewingEngine, ibus_chewing_engine, IBUS, CHEWING_ENGINE, IBusEngine)

void ibus_chewing_engine_refresh_property_list(IBusChewingEngine *self);
void ibus_chewing_engine_hide_property_list(IBusChewingEngine *self);

void ibus_chewing_engine_reset(IBusEngine *self);
void ibus_chewing_engine_enable(IBusEngine *self);
void ibus_chewing_engine_disable(IBusEngine *self);
void ibus_chewing_engine_focus_in(IBusEngine *self);
void ibus_chewing_engine_focus_out(IBusEngine *self);
gboolean ibus_chewing_engine_process_key_event(IBusEngine *self, guint key_sym, guint keycode,
                                               guint modifiers);

char ibus_chewing_engine_get_default_english_case(IBusChewingEngine *self);
char ibus_chewing_engine_get_chinese_english_toggle_key(IBusChewingEngine *self);
gboolean ibus_chewing_engine_use_vertical_lookup_table(IBusChewingEngine *self);
gboolean ibus_chewing_engine_use_system_layout(IBusChewingEngine *self);
void ibus_chewing_engine_notify_chinese_english_mode_change(IBusChewingEngine *self);
void ibus_chewing_engine_notify_fullwidth_mode_change(IBusChewingEngine *self);

G_END_DECLS