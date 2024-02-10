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
#include "GSettingsBackend.h"
#include "IBusChewingPreEdit.h"
#include "IBusChewingProperties.h"
#include "IBusChewingUtil.h"
#include <chewing.h>
#include <ctype.h>
#include <glib/gi18n.h>
#include <ibus.h>
#include <libintl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __IBUS_CHEWING_ENGINE_PRIVATE_H__
#define __IBUS_CHEWING_ENGINE_PRIVATE_H__

#include "ibus-chewing-engine.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void ibus_chewing_engine_handle_Default(IBusChewingEngine *self, guint keyval,
                                        gboolean shiftPressed);
#define self_handle_Default ibus_chewing_engine_handle_Default

void ibus_chewing_engine_candidate_clicked(IBusEngine *engine, guint index,
                                           guint button, guint state);
void ibus_chewing_engine_property_activate(IBusEngine *engine,
                                           const gchar *prop_name,
                                           guint prop_state);
void ibus_chewing_engine_set_content_type(IBusEngine *engine, guint purpose,
                                          guint hints);

void refresh_pre_edit_text(IBusChewingEngine *self);
void update_pre_edit_text(IBusChewingEngine *self);
void refresh_aux_text(IBusChewingEngine *self);
void update_aux_text(IBusChewingEngine *self);
void update_lookup_table(IBusChewingEngine *self);
void refresh_outgoing_text(IBusChewingEngine *self);
void commit_text(IBusChewingEngine *self);

struct _IBusChewingEnginePrivate {
    EngineFlag statusFlags;
    IBusCapabilite capabilite;
    ChewingInputStyle inputStyle;
};
void ibus_chewing_engine_use_setting(IBusChewingEngine *self);
void ibus_chewing_engine_restore_mode(IBusChewingEngine *self);
void ibus_chewing_engine_update(IBusChewingEngine *self);
void ibus_chewing_engine_refresh_property(IBusChewingEngine *self,
                                          const gchar *prop_name);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
