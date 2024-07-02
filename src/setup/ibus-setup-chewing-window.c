/* ibus-setup-chewing-window.c
 *
 * Copyright 2024 Kan-Ru Chen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "ibus-setup-chewing-window.h"
#include "ibus-setup-chewing-about.h"

struct _IbusSetupChewingWindow {
    AdwPreferencesWindow parent_instance;

    /* Template widgets */
    AdwComboRow *kb_type;
    AdwComboRow *sel_keys;
    AdwSwitchRow *plain_zhuyin;
    AdwSwitchRow *auto_shift_cur;
    AdwSwitchRow *add_phrase_direction;
    AdwSwitchRow *clean_buffer_focus_out;
    AdwSwitchRow *easy_symbol_input;
    AdwSwitchRow *esc_clean_all_buf;
    AdwSpinRow *max_chi_symbol_len;
    AdwComboRow *chi_eng_mode_toggle;
    AdwComboRow *sync_caps_lock;
    AdwComboRow *default_english_case;
    AdwSpinRow *cand_per_page;
    AdwSwitchRow *show_page_number;
    AdwSwitchRow *phrase_choice_from_last;
    AdwSwitchRow *space_as_selection;
    AdwSwitchRow *vertical_lookup_table;
};

G_DEFINE_FINAL_TYPE(IbusSetupChewingWindow, ibus_setup_chewing_window,
                    ADW_TYPE_PREFERENCES_WINDOW)

#define bind_child(child_id)                                                   \
    gtk_widget_class_bind_template_child(widget_class, IbusSetupChewingWindow, \
                                         child_id)

static void action_adaptor_show_about(GtkWidget *widget,
                                      [[maybe_unused]] const char *action_name,
                                      [[maybe_unused]] GVariant *parameter) {
    show_about(widget);
}

static void
ibus_setup_chewing_window_class_init(IbusSetupChewingWindowClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(
        widget_class,
        "/org/freedesktop/IBus/Chewing/Setup/ibus-setup-chewing-window.ui");

    bind_child(kb_type);
    bind_child(sel_keys);
    bind_child(plain_zhuyin);
    bind_child(auto_shift_cur);
    bind_child(add_phrase_direction);
    bind_child(clean_buffer_focus_out);
    bind_child(easy_symbol_input);
    bind_child(esc_clean_all_buf);
    bind_child(max_chi_symbol_len);
    bind_child(chi_eng_mode_toggle);
    bind_child(sync_caps_lock);
    bind_child(default_english_case);
    bind_child(cand_per_page);
    bind_child(show_page_number);
    bind_child(phrase_choice_from_last);
    bind_child(space_as_selection);
    bind_child(vertical_lookup_table);

    gtk_widget_class_install_action(widget_class, "about", NULL,
                                    action_adaptor_show_about);
}

const gchar *kb_type_ids[] = {
    "default",
    "hsu",
    "ibm",
    "gin_yieh",
    "eten",
    "eten26",
    "dvorak",
    "dvorak_hsu",
    "dachen_26",
    "hanyu",
    "thl_pinying",
    "mps2_pinyin",
    "carpalx",
    "colemak_dh_ansi",
    "colemak_dh_orth",
    NULL,
};

const gchar *sel_key_ids[] = {
    "1234567890", "asdfghjkl;", "asdfzxcv89",
    "asdfjkl789", "aoeu;qjkix", /* Dvorak */
    "aoeuhtnsid",               /* Dvorak */
    "aoeuidhtns",               /* Dvorak */
    "1234qweras", NULL,
};

const gchar *chi_eng_mode_toggle_ids[] = {
    "caps_lock", "shift", "shift_l", "shift_r", NULL,
};

const gchar *sync_caps_lock_ids[] = {
    "disable",
    "keyboard",
    "input method",
    NULL,
};

const gchar *default_english_case_ids[] = {
    "no default",
    "lowercase",
    "uppercase",
    NULL,
};

static gboolean id_get_mapping(GValue *value, GVariant *variant,
                               gpointer user_data) {
    const gchar *saved;
    gchar **ids_list = (gchar **)user_data;

    g_return_val_if_fail(g_variant_is_of_type(variant, G_VARIANT_TYPE_STRING),
                         FALSE);

    saved = g_variant_get_string(variant, NULL);
    for (int i = 0; ids_list[i] != NULL; i++) {
        if (!g_strcmp0(ids_list[i], saved)) {
            g_value_set_uint(value, i);
            return TRUE;
        }
    }
    return FALSE;
}

static GVariant *id_set_mapping(const GValue *value,
                                const GVariantType *expected_type,
                                gpointer user_data) {
    gchar **ids_list = (gchar **)user_data;

    return g_variant_new_string(ids_list[g_value_get_uint(value)]);
}

static void ibus_setup_chewing_window_init(IbusSetupChewingWindow *self) {
    GSettings *settings;

    gtk_widget_init_template(GTK_WIDGET(self));

    settings = g_settings_new("org.freedesktop.IBus.Chewing");

    g_settings_bind_with_mapping(settings, "kb-type", self->kb_type, "selected",
                                 G_SETTINGS_BIND_DEFAULT, id_get_mapping,
                                 id_set_mapping, kb_type_ids, NULL);
    g_settings_bind_with_mapping(settings, "sel-keys", self->sel_keys,
                                 "selected", G_SETTINGS_BIND_DEFAULT,
                                 id_get_mapping, id_set_mapping, sel_key_ids,
                                 NULL);
    g_settings_bind(settings, "plain-zhuyin", self->plain_zhuyin, "active",
                    G_SETTINGS_BIND_INVERT_BOOLEAN);
    g_settings_bind(settings, "auto-shift-cur", self->auto_shift_cur, "active",
                    G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings, "add-phrase-direction",
                    self->add_phrase_direction, "active",
                    G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings, "clean-buffer-focus-out",
                    self->clean_buffer_focus_out, "active",
                    G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings, "easy-symbol-input", self->easy_symbol_input,
                    "active", G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings, "esc-clean-all-buf", self->esc_clean_all_buf,
                    "active", G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings, "max-chi-symbol-len", self->max_chi_symbol_len,
                    "value", G_SETTINGS_BIND_DEFAULT);
    g_settings_bind_with_mapping(settings, "chi-eng-mode-toggle",
                                 self->chi_eng_mode_toggle, "selected",
                                 G_SETTINGS_BIND_DEFAULT, id_get_mapping,
                                 id_set_mapping, chi_eng_mode_toggle_ids, NULL);
    g_settings_bind_with_mapping(settings, "sync-caps-lock",
                                 self->sync_caps_lock, "selected",
                                 G_SETTINGS_BIND_DEFAULT, id_get_mapping,
                                 id_set_mapping, sync_caps_lock_ids, NULL);
    g_settings_bind_with_mapping(
        settings, "default-english-case", self->default_english_case,
        "selected", G_SETTINGS_BIND_DEFAULT, id_get_mapping, id_set_mapping,
        default_english_case_ids, NULL);
    g_settings_bind(settings, "cand-per-page", self->cand_per_page, "value",
                    G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings, "show-page-number", self->show_page_number,
                    "active", G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings, "phrase-choice-from-last",
                    self->phrase_choice_from_last, "active",
                    G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings, "space-as-selection", self->space_as_selection,
                    "active", G_SETTINGS_BIND_DEFAULT);
    g_settings_bind(settings, "vertical-lookup-table",
                    self->vertical_lookup_table, "active",
                    G_SETTINGS_BIND_DEFAULT);
}
