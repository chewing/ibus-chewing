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
#include <ibus.h>

#include "ibus-chewing-engine.h"

G_BEGIN_DECLS

struct _IBusChewingEngine {
    IBusEngine __parent__;
    IBusChewingPreEdit *icPreEdit;
    IBusText *preEditText;
    IBusText *auxText;
    IBusText *outgoingText;
    IBusProperty *InputMode;
    IBusProperty *AlnumSize;
    IBusProperty *setup_prop;
    IBusPropList *prop_list;
    IBusKeymap *keymap_us;

    EngineFlag statusFlags;
    IBusCapabilite capabilite;

    char *prop_kb_type;
    char *prop_sel_keys;
    int prop_cand_per_page;
    gboolean prop_vertical_lookup_table;
    gboolean prop_auto_shift_cur;
    gboolean prop_add_phrase_direction;
    gboolean prop_clean_buffer_focus_out;
    gboolean prop_easy_symbol_input;
    gboolean prop_esc_clean_all_buf;
    gboolean prop_enable_fullwidth_toggle_key;
    int prop_max_chi_symbol_len;
    char *prop_default_english_case;
    char *prop_chi_eng_mode_toggle;
    gboolean prop_phrase_choice_from_last;
    gboolean prop_space_as_selection;
    char *prop_sync_caps_lock;
    gboolean prop_show_page_number;
    char *prop_conversion_engine;
    gboolean prop_ibus_use_system_layout;

    IBusText *InputMode_label_chi;
    IBusText *InputMode_label_eng;
    IBusText *InputMode_tooltip;
    IBusText *InputMode_symbol_chi;
    IBusText *InputMode_symbol_eng;
    IBusText *AlnumSize_label_full;
    IBusText *AlnumSize_label_half;
    IBusText *AlnumSize_tooltip;
    IBusText *AlnumSize_symbol_full;
    IBusText *AlnumSize_symbol_half;
    IBusText *setup_prop_label;
    IBusText *setup_prop_tooltip;
    IBusText *setup_prop_symbol;
    IBusText *emptyText;
};

void ibus_chewing_engine_handle_Default(IBusChewingEngine *self, guint keyval,
                                        gboolean shiftPressed);
#define self_handle_Default ibus_chewing_engine_handle_Default

void ibus_chewing_engine_candidate_clicked(IBusEngine *engine, guint index, guint button,
                                           guint state);
void ibus_chewing_engine_property_activate(IBusEngine *engine, const gchar *prop_name,
                                           guint prop_state);
void ibus_chewing_engine_set_content_type(IBusEngine *engine, guint purpose, guint hints);

void refresh_pre_edit_text(IBusChewingEngine *self);
void update_pre_edit_text(IBusChewingEngine *self);
void refresh_aux_text(IBusChewingEngine *self);
void update_aux_text(IBusChewingEngine *self);
void update_lookup_table(IBusChewingEngine *self);
void refresh_outgoing_text(IBusChewingEngine *self);
void commit_text(IBusChewingEngine *self);

void ibus_chewing_engine_restore_mode(IBusChewingEngine *self);
void ibus_chewing_engine_update(IBusChewingEngine *self);
void ibus_chewing_engine_refresh_property(IBusChewingEngine *self, const gchar *prop_name);

G_END_DECLS