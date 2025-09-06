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
#include "ibus-chewing-engine.h"
#include "ibus-chewing-engine-private.h"
#include "ibus-chewing-lookup-table.h"
#include "ibus-chewing-preedit.h"
#include "ibus-chewing-util.h"
#include <chewing.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <ibus.h>

static ChewingKbType kb_type_get_index(const gchar *kb_type) {
    // The order of this list should match libchewing's KB enum
    //
    // clang-format off
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
        "workman",
        "colemak",
        NULL
    };
    // clang-format on
    ChewingKbType i = 0;

    for (i = 0; kb_type_ids[i] != NULL; i++) {
        if (strcmp(kb_type, kb_type_ids[i]) == 0) {
            return i;
        }
    }
    return CHEWING_KBTYPE_INVALID;
}

typedef enum {
    PROP_KB_TYPE = 1,
    PROP_SEL_KEYS,
    PROP_CAND_PER_PAGE,
    PROP_VERTICAL_LOOKUP_TABLE,
    PROP_AUTO_SHIFT_CUR,
    PROP_ADD_PHRASE_DIRECTION,
    PROP_CLEAN_BUFFER_FOCUS_OUT,
    PROP_EASY_SYMBOL_INPUT,
    PROP_ESC_CLEAN_ALL_BUF,
    PROP_ENABLE_FULLWIDTH_TOGGLE_KEY,
    PROP_MAX_CHI_SYMBOL_LEN,
    PROP_DEFAULT_ENGLISH_CASE,
    PROP_DEFAULT_USE_ENGLISH_MODE,
    PROP_CHI_ENG_MODE_TOGGLE,
    PROP_PHRASE_CHOICE_FROM_LAST,
    PROP_SPACE_AS_SELECTION,
    PROP_SYNC_CAPS_LOCK,
    PROP_SHOW_PAGE_NUMBER,
    PROP_CONVERSION_ENGINE,
    PROP_IBUS_USE_SYSTEM_LAYOUT,
    PROP_NOTIFY_MODE_CHANGE,
    N_PROPERTIES
} IBusChewingEngineProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = {};

G_DEFINE_TYPE(IBusChewingEngine, ibus_chewing_engine, IBUS_TYPE_ENGINE);

static IBusProperty *ibus_chewing_engine_get_ibus_property_by_name(IBusChewingEngine *self,
                                                                   const gchar *prop_name);
static void ibus_chewing_engine_page_up(IBusEngine *engine);
static void ibus_chewing_engine_page_down(IBusEngine *engine);
static void ibus_chewing_engine_cursor_up(IBusEngine *engine);
static void ibus_chewing_engine_cursor_down(IBusEngine *engine);
static void ibus_chewing_engine_set_capabilities(IBusEngine *engine, guint caps);
static void ibus_chewing_engine_property_show(IBusEngine *engine, const gchar *prop_name);
static void ibus_chewing_engine_property_hide(IBusEngine *engine, const gchar *prop_name);

#define ibus_chewing_engine_has_status_flag(self, f) mkdg_has_flag(self->statusFlags, f)
#define ibus_chewing_engine_set_status_flag(self, f) mkdg_set_flag(self->statusFlags, f)
#define ibus_chewing_engine_clear_status_flag(self, f) mkdg_clear_flag(self->statusFlags, f)

#define is_password(self) ibus_chewing_engine_has_status_flag(self, ENGINE_FLAG_IS_PASSWORD)
#define is_chinese_mode(self) ibus_chewing_pre_edit_get_chi_eng_mode(self->icPreEdit)
#define is_fullwidth_mode(self) ibus_chewing_pre_edit_get_full_half_mode(self->icPreEdit)

#define ibus_text_is_empty(iText) ((iText == NULL) || STRING_IS_EMPTY(iText->text))

static void ibus_chewing_engine_finalize(GObject *gobject) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(gobject);

    ibus_chewing_pre_edit_free(self->icPreEdit);
    g_clear_object(&self->preEditText);
    g_clear_object(&self->auxText);
    g_clear_object(&self->outgoingText);
    g_clear_object(&self->InputMode);
    g_clear_object(&self->AlnumSize);
    g_clear_object(&self->setup_prop);
    g_clear_object(&self->prop_list);
    g_free(self->prop_kb_type);
    self->prop_kb_type = NULL;
    g_free(self->prop_sel_keys);
    self->prop_sel_keys = NULL;
    g_free(self->prop_default_english_case);
    self->prop_default_english_case = NULL;
    g_free(self->prop_chi_eng_mode_toggle);
    self->prop_chi_eng_mode_toggle = NULL;
    g_free(self->prop_sync_caps_lock);
    self->prop_sync_caps_lock = NULL;
    g_free(self->prop_conversion_engine);
    self->prop_conversion_engine = NULL;
    G_OBJECT_CLASS(ibus_chewing_engine_parent_class)->finalize(gobject);
}

static void ibus_chewing_engine_set_property(GObject *object, guint property_id,
                                             const GValue *value, GParamSpec *pspec) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(object);
    ChewingContext *ctx = self->icPreEdit->context;

    switch ((IBusChewingEngineProperty)property_id) {
    case PROP_KB_TYPE:
        g_free(self->prop_kb_type);
        self->prop_kb_type = g_value_dup_string(value);
        chewing_set_KBType(self->icPreEdit->context, kb_type_get_index(self->prop_kb_type));
        break;
    case PROP_SEL_KEYS:
        g_free(self->prop_sel_keys);
        self->prop_sel_keys = g_value_dup_string(value);
        // FIXME
        ibus_chewing_lookup_table_resize(self->icPreEdit->iTable, ctx);
        break;
    case PROP_CAND_PER_PAGE:
        self->prop_cand_per_page = g_value_get_int(value);
        // FIXME
        ibus_chewing_lookup_table_resize(self->icPreEdit->iTable, ctx);
        break;
    case PROP_VERTICAL_LOOKUP_TABLE:
        self->prop_vertical_lookup_table = g_value_get_boolean(value);
        // FIXME
        ibus_chewing_lookup_table_resize(self->icPreEdit->iTable, ctx);
        break;
    case PROP_AUTO_SHIFT_CUR:
        self->prop_auto_shift_cur = g_value_get_boolean(value);
        chewing_set_autoShiftCur(ctx, self->prop_auto_shift_cur);
        break;
    case PROP_ADD_PHRASE_DIRECTION:
        self->prop_add_phrase_direction = g_value_get_boolean(value);
        chewing_set_addPhraseDirection(ctx, self->prop_add_phrase_direction);
        break;
    case PROP_CLEAN_BUFFER_FOCUS_OUT:
        self->prop_clean_buffer_focus_out = g_value_get_boolean(value);
        break;
    case PROP_EASY_SYMBOL_INPUT:
        self->prop_easy_symbol_input = g_value_get_boolean(value);
        chewing_set_easySymbolInput(ctx, self->prop_easy_symbol_input);
        break;
    case PROP_ESC_CLEAN_ALL_BUF:
        self->prop_esc_clean_all_buf = g_value_get_boolean(value);
        chewing_set_escCleanAllBuf(ctx, self->prop_esc_clean_all_buf);
        break;
    case PROP_ENABLE_FULLWIDTH_TOGGLE_KEY:
        self->prop_enable_fullwidth_toggle_key = g_value_get_boolean(value);
        chewing_config_set_int(ctx, "chewing.enable_fullwidth_toggle_key",
                               self->prop_enable_fullwidth_toggle_key);
        break;
    case PROP_MAX_CHI_SYMBOL_LEN:
        self->prop_max_chi_symbol_len = g_value_get_int(value);
        chewing_set_maxChiSymbolLen(ctx, self->prop_max_chi_symbol_len);
        break;
    case PROP_DEFAULT_ENGLISH_CASE:
        g_free(self->prop_default_english_case);
        self->prop_default_english_case = g_value_dup_string(value);
        break;
    case PROP_DEFAULT_USE_ENGLISH_MODE:
        self->prop_default_use_english_mode = g_value_get_boolean(value);
        break;
    case PROP_CHI_ENG_MODE_TOGGLE:
        g_free(self->prop_chi_eng_mode_toggle);
        self->prop_chi_eng_mode_toggle = g_value_dup_string(value);
        break;
    case PROP_PHRASE_CHOICE_FROM_LAST:
        self->prop_phrase_choice_from_last = g_value_get_boolean(value);
        chewing_set_phraseChoiceRearward(ctx, self->prop_phrase_choice_from_last);
        break;
    case PROP_SPACE_AS_SELECTION:
        self->prop_space_as_selection = g_value_get_boolean(value);
        chewing_set_spaceAsSelection(ctx, self->prop_space_as_selection);
        break;
    case PROP_SYNC_CAPS_LOCK:
        g_free(self->prop_sync_caps_lock);
        self->prop_sync_caps_lock = g_value_dup_string(value);
        if (strcmp(self->prop_sync_caps_lock, "keyboard") == 0) {
            ibus_chewing_pre_edit_set_flag(self->icPreEdit, FLAG_SYNC_FROM_KEYBOARD);
            ibus_chewing_pre_edit_clear_flag(self->icPreEdit, FLAG_SYNC_FROM_IM);
        } else if (strcmp(self->prop_sync_caps_lock, "input method") == 0) {
            ibus_chewing_pre_edit_set_flag(self->icPreEdit, FLAG_SYNC_FROM_IM);
            ibus_chewing_pre_edit_clear_flag(self->icPreEdit, FLAG_SYNC_FROM_KEYBOARD);
        } else {
            ibus_chewing_pre_edit_clear_flag(self->icPreEdit,
                                             FLAG_SYNC_FROM_IM | FLAG_SYNC_FROM_KEYBOARD);
        }
        break;
    case PROP_SHOW_PAGE_NUMBER:
        self->prop_show_page_number = g_value_get_boolean(value);
        break;
    case PROP_CONVERSION_ENGINE:
        g_free(self->prop_conversion_engine);
        self->prop_conversion_engine = g_value_dup_string(value);
        if (!g_strcmp0(self->prop_conversion_engine, "simple")) {
            chewing_config_set_int(ctx, "chewing.conversion_engine", 0);
        } else if (!g_strcmp0(self->prop_conversion_engine, "chewing")) {
            chewing_config_set_int(ctx, "chewing.conversion_engine", 1);
        } else if (!g_strcmp0(self->prop_conversion_engine, "fuzzy-chewing")) {
            chewing_config_set_int(ctx, "chewing.conversion_engine", 2);
        }
        break;
    case PROP_IBUS_USE_SYSTEM_LAYOUT:
        self->prop_ibus_use_system_layout = g_value_get_boolean(value);
        break;
    case PROP_NOTIFY_MODE_CHANGE:
        self->prop_notify_mode_change = g_value_get_boolean(value);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void ibus_chewing_engine_get_property(GObject *object, guint property_id, GValue *value,
                                             GParamSpec *pspec) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(object);
    switch ((IBusChewingEngineProperty)property_id) {
    case PROP_KB_TYPE:
        g_value_set_string(value, self->prop_kb_type);
        break;
    case PROP_SEL_KEYS:
        g_value_set_string(value, self->prop_sel_keys);
        break;
    case PROP_CAND_PER_PAGE:
        g_value_set_int(value, self->prop_cand_per_page);
        break;
    case PROP_VERTICAL_LOOKUP_TABLE:
        g_value_set_boolean(value, self->prop_vertical_lookup_table);
        break;
    case PROP_AUTO_SHIFT_CUR:
        g_value_set_boolean(value, self->prop_auto_shift_cur);
        break;
    case PROP_ADD_PHRASE_DIRECTION:
        g_value_set_boolean(value, self->prop_add_phrase_direction);
        break;
    case PROP_CLEAN_BUFFER_FOCUS_OUT:
        g_value_set_boolean(value, self->prop_clean_buffer_focus_out);
        break;
    case PROP_EASY_SYMBOL_INPUT:
        g_value_set_boolean(value, self->prop_easy_symbol_input);
        break;
    case PROP_ESC_CLEAN_ALL_BUF:
        g_value_set_boolean(value, self->prop_esc_clean_all_buf);
        break;
    case PROP_ENABLE_FULLWIDTH_TOGGLE_KEY:
        g_value_set_boolean(value, self->prop_enable_fullwidth_toggle_key);
        break;
    case PROP_MAX_CHI_SYMBOL_LEN:
        g_value_set_int(value, self->prop_max_chi_symbol_len);
        break;
    case PROP_DEFAULT_ENGLISH_CASE:
        g_value_set_string(value, self->prop_default_english_case);
        break;
    case PROP_DEFAULT_USE_ENGLISH_MODE:
        g_value_set_boolean(value, self->prop_default_use_english_mode);
        break;
    case PROP_CHI_ENG_MODE_TOGGLE:
        g_value_set_string(value, self->prop_chi_eng_mode_toggle);
        break;
    case PROP_PHRASE_CHOICE_FROM_LAST:
        g_value_set_boolean(value, self->prop_phrase_choice_from_last);
        break;
    case PROP_SPACE_AS_SELECTION:
        g_value_set_boolean(value, self->prop_space_as_selection);
        break;
    case PROP_SYNC_CAPS_LOCK:
        g_value_set_string(value, self->prop_sync_caps_lock);
        break;
    case PROP_SHOW_PAGE_NUMBER:
        g_value_set_boolean(value, self->prop_show_page_number);
        break;
    case PROP_CONVERSION_ENGINE:
        g_value_set_string(value, self->prop_conversion_engine);
        break;
    case PROP_IBUS_USE_SYSTEM_LAYOUT:
        g_value_set_boolean(value, self->prop_ibus_use_system_layout);
        break;
    case PROP_NOTIFY_MODE_CHANGE:
        g_value_set_boolean(value, self->prop_notify_mode_change);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void ibus_chewing_engine_class_init(IBusChewingEngineClass *klass) {
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    IBusEngineClass *ibus_engine_class = IBUS_ENGINE_CLASS(klass);

    object_class->finalize = ibus_chewing_engine_finalize;
    object_class->set_property = ibus_chewing_engine_set_property;
    object_class->get_property = ibus_chewing_engine_get_property;

    ibus_engine_class->reset = ibus_chewing_engine_reset;
    ibus_engine_class->page_up = ibus_chewing_engine_page_up;
    ibus_engine_class->page_down = ibus_chewing_engine_page_down;
    ibus_engine_class->cursor_up = ibus_chewing_engine_cursor_up;
    ibus_engine_class->cursor_down = ibus_chewing_engine_cursor_down;
    ibus_engine_class->enable = ibus_chewing_engine_enable;
    ibus_engine_class->disable = ibus_chewing_engine_disable;
    ibus_engine_class->focus_in = ibus_chewing_engine_focus_in;
    ibus_engine_class->focus_out = ibus_chewing_engine_focus_out;
    ibus_engine_class->set_capabilities = ibus_chewing_engine_set_capabilities;
    ibus_engine_class->property_show = ibus_chewing_engine_property_show;
    ibus_engine_class->property_hide = ibus_chewing_engine_property_hide;

    ibus_engine_class->property_activate = ibus_chewing_engine_property_activate;
    ibus_engine_class->process_key_event = ibus_chewing_engine_process_key_event;
    ibus_engine_class->candidate_clicked = ibus_chewing_engine_candidate_clicked;
#if IBUS_CHECK_VERSION(1, 5, 4)
    ibus_engine_class->set_content_type = ibus_chewing_engine_set_content_type;
#endif

    obj_properties[PROP_KB_TYPE] =
        g_param_spec_string("kb-type", NULL, NULL, NULL, G_PARAM_READWRITE);
    obj_properties[PROP_SEL_KEYS] =
        g_param_spec_string("sel-keys", NULL, NULL, NULL, G_PARAM_READWRITE);
    obj_properties[PROP_CAND_PER_PAGE] =
        g_param_spec_int("cand-per-page", NULL, NULL, 4, 10, 5, G_PARAM_READWRITE);
    obj_properties[PROP_VERTICAL_LOOKUP_TABLE] =
        g_param_spec_boolean("vertical-lookup-table", NULL, NULL, FALSE, G_PARAM_READWRITE);
    obj_properties[PROP_AUTO_SHIFT_CUR] =
        g_param_spec_boolean("auto-shift-cur", NULL, NULL, TRUE, G_PARAM_READWRITE);
    obj_properties[PROP_ADD_PHRASE_DIRECTION] =
        g_param_spec_boolean("add-phrase-direction", NULL, NULL, TRUE, G_PARAM_READWRITE);
    obj_properties[PROP_CLEAN_BUFFER_FOCUS_OUT] =
        g_param_spec_boolean("clean-buffer-focus-out", NULL, NULL, FALSE, G_PARAM_READWRITE);
    obj_properties[PROP_EASY_SYMBOL_INPUT] =
        g_param_spec_boolean("easy-symbol-input", NULL, NULL, TRUE, G_PARAM_READWRITE);
    obj_properties[PROP_ESC_CLEAN_ALL_BUF] =
        g_param_spec_boolean("esc-clean-all-buf", NULL, NULL, FALSE, G_PARAM_READWRITE);
    obj_properties[PROP_ENABLE_FULLWIDTH_TOGGLE_KEY] =
        g_param_spec_boolean("enable-fullwidth-toggle-key", NULL, NULL, TRUE, G_PARAM_READWRITE);
    obj_properties[PROP_MAX_CHI_SYMBOL_LEN] =
        g_param_spec_int("max-chi-symbol-len", NULL, NULL, 0, 39, 20, G_PARAM_READWRITE);
    obj_properties[PROP_DEFAULT_ENGLISH_CASE] =
        g_param_spec_string("default-english-case", NULL, NULL, NULL, G_PARAM_READWRITE);
    obj_properties[PROP_DEFAULT_USE_ENGLISH_MODE] =
        g_param_spec_boolean("default-use-english-mode", NULL, NULL, FALSE, G_PARAM_READWRITE);
    obj_properties[PROP_CHI_ENG_MODE_TOGGLE] =
        g_param_spec_string("chi-eng-mode-toggle", NULL, NULL, NULL, G_PARAM_READWRITE);
    obj_properties[PROP_PHRASE_CHOICE_FROM_LAST] =
        g_param_spec_boolean("phrase-choice-from-last", NULL, NULL, TRUE, G_PARAM_READWRITE);
    obj_properties[PROP_SPACE_AS_SELECTION] =
        g_param_spec_boolean("space-as-selection", NULL, NULL, FALSE, G_PARAM_READWRITE);
    obj_properties[PROP_SYNC_CAPS_LOCK] =
        g_param_spec_string("sync-caps-lock", NULL, NULL, NULL, G_PARAM_READWRITE);
    obj_properties[PROP_SHOW_PAGE_NUMBER] =
        g_param_spec_boolean("show-page-number", NULL, NULL, FALSE, G_PARAM_READWRITE);
    obj_properties[PROP_CONVERSION_ENGINE] =
        g_param_spec_string("conversion-engine", NULL, NULL, NULL, G_PARAM_READWRITE);
    obj_properties[PROP_IBUS_USE_SYSTEM_LAYOUT] =
        g_param_spec_boolean("use-system-keyboard-layout", NULL, NULL, FALSE, G_PARAM_READWRITE);
    obj_properties[PROP_NOTIFY_MODE_CHANGE] =
        g_param_spec_boolean("notify-mode-change", NULL, NULL, TRUE, G_PARAM_READWRITE);

    g_object_class_install_properties(object_class, N_PROPERTIES, obj_properties);
}

#define bind_settings(key) g_settings_bind(settings, key, self, key, G_SETTINGS_BIND_DEFAULT)

static void ibus_chewing_engine_init(IBusChewingEngine *self) {
    self->InputMode_label_chi =
        g_object_ref_sink(ibus_text_new_from_static_string(_("Switch to Alphanumeric Mode")));
    self->InputMode_label_eng =
        g_object_ref_sink(ibus_text_new_from_static_string(_("Switch to Chinese Mode")));
    self->InputMode_tooltip = g_object_ref_sink(
        ibus_text_new_from_static_string(_("Click to toggle Chinese/Alphanumeric Mode")));
    self->InputMode_symbol_chi = g_object_ref_sink(ibus_text_new_from_static_string("中"));
    self->InputMode_symbol_eng = g_object_ref_sink(ibus_text_new_from_static_string("英"));
    self->AlnumSize_label_full =
        g_object_ref_sink(ibus_text_new_from_static_string(_("Fullwidth Form")));
    self->AlnumSize_label_half =
        g_object_ref_sink(ibus_text_new_from_static_string(_("Halfwidth Form")));
    self->AlnumSize_tooltip = g_object_ref_sink(
        ibus_text_new_from_static_string(_("Click to toggle Halfwidth/Fullwidth Form")));
    self->AlnumSize_symbol_full = g_object_ref_sink(ibus_text_new_from_static_string("全"));
    self->AlnumSize_symbol_half = g_object_ref_sink(ibus_text_new_from_static_string("半"));
    self->setup_prop_label =
        g_object_ref_sink(ibus_text_new_from_static_string(_("IBus-Chewing Preferences")));
    self->setup_prop_tooltip =
        g_object_ref_sink(ibus_text_new_from_static_string(_("Click to configure IBus-Chewing")));
    self->setup_prop_symbol = g_object_ref_sink(ibus_text_new_from_static_string("訂"));
    self->emptyText = g_object_ref_sink(ibus_text_new_from_static_string(""));

    self->preEditText = NULL;
    self->auxText = NULL;
    self->outgoingText = NULL;
    self->statusFlags = 0;
    self->capabilite = 0;
    self->pending_notify_chinese_english_mode = FALSE;
    self->pending_notify_fullwidth_mode = FALSE;
    self->InputMode = g_object_ref_sink(
        ibus_property_new("InputMode", PROP_TYPE_NORMAL, self->InputMode_label_chi, NULL,
                          self->InputMode_tooltip, TRUE, TRUE, PROP_STATE_UNCHECKED, NULL));
    self->AlnumSize = g_object_ref_sink(
        ibus_property_new("AlnumSize", PROP_TYPE_NORMAL, self->AlnumSize_label_half, NULL,
                          self->AlnumSize_tooltip, TRUE, TRUE, PROP_STATE_UNCHECKED, NULL));
    self->setup_prop = g_object_ref_sink(
        ibus_property_new("setup_prop", PROP_TYPE_NORMAL, self->setup_prop_label, NULL,
                          self->setup_prop_tooltip, TRUE, TRUE, PROP_STATE_UNCHECKED, NULL));
    self->prop_list = g_object_ref_sink(ibus_prop_list_new());
    self->keymap_us = ibus_keymap_get("us");
    g_info("init() %sinitialized", (self->statusFlags & ENGINE_FLAG_INITIALIZED) ? "" : "un");
    if (self->statusFlags & ENGINE_FLAG_INITIALIZED) {
        return;
    }

    self->icPreEdit = ibus_chewing_pre_edit_new();

    g_assert(self->icPreEdit);

    self->icPreEdit->engine = IBUS_ENGINE(self);

    /* init properties */
    ibus_prop_list_append(self->prop_list, self->InputMode);
    ibus_prop_list_append(self->prop_list, self->AlnumSize);
    ibus_prop_list_append(self->prop_list, self->setup_prop);

    ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_INITIALIZED);

    g_autoptr(GSettings) settings = g_settings_new(QUOTE_ME(PROJECT_SCHEMA_ID));
    g_autoptr(GSettings) ibus_settings = g_settings_new("org.freedesktop.ibus.general");

    bind_settings("kb-type");
    bind_settings("sel-keys");
    bind_settings("cand-per-page");
    bind_settings("vertical-lookup-table");
    bind_settings("auto-shift-cur");
    bind_settings("add-phrase-direction");
    bind_settings("clean-buffer-focus-out");
    bind_settings("easy-symbol-input");
    bind_settings("esc-clean-all-buf");
    bind_settings("enable-fullwidth-toggle-key");
    bind_settings("max-chi-symbol-len");
    bind_settings("default-english-case");
    bind_settings("default-use-english-mode");
    bind_settings("chi-eng-mode-toggle");
    bind_settings("phrase-choice-from-last");
    bind_settings("space-as-selection");
    bind_settings("sync-caps-lock");
    bind_settings("show-page-number");
    bind_settings("conversion-engine");
    bind_settings("notify-mode-change");

    g_settings_bind(ibus_settings, "use-system-keyboard-layout", self, "use-system-keyboard-layout",
                    G_SETTINGS_BIND_DEFAULT);

    g_debug("init() Done");
}

void ibus_chewing_engine_restore_mode(IBusChewingEngine *self) {
    g_return_if_fail(self != NULL);
    g_return_if_fail(IBUS_IS_CHEWING_ENGINE(self));
    if (ibus_chewing_engine_get_chinese_english_toggle_key(self) == 'c') {
        g_debug("restore_mode() statusFlags=%x", self->statusFlags);
        GdkDisplay *display = gdk_display_get_default();

        if (display != NULL) {
            GdkSeat *seat = gdk_display_get_default_seat(display);

            if (seat != NULL) {
                GdkDevice *keyboard = gdk_seat_get_keyboard(seat);

                /* Restore Led Mode only make sense if
                 * display is available */
                if (ibus_chewing_pre_edit_has_flag(self->icPreEdit, FLAG_SYNC_FROM_IM)) {
                    g_debug("restore_mode() "
                            "FLAG_SYNC_FROM_IM (deprecated)");
                } else if (keyboard != NULL && ibus_chewing_pre_edit_has_flag(
                                                   self->icPreEdit, FLAG_SYNC_FROM_KEYBOARD)) {
                    g_debug("restore_mode() "
                            "FLAG_SYNC_FROM_KEYBOARD");
                    gboolean caps_lock_on = gdk_device_get_caps_lock_state(keyboard);
                    chewing_set_ChiEngMode(self->icPreEdit->context,
                                           caps_lock_on ? 0 : CHINESE_MODE);
                }
            }
            ibus_chewing_engine_refresh_property(self, "InputMode");
        }
    }
}

void ibus_chewing_engine_update(IBusChewingEngine *self) {
    g_return_if_fail(self != NULL);
    g_return_if_fail(IBUS_IS_CHEWING_ENGINE(self));
    {
        g_debug("update() statusFlags=%x", self->statusFlags);
        commit_text(self);
        update_pre_edit_text(self);
        update_aux_text(self);

        g_debug("update() nPhoneSeq=%d statusFlags=%x",
                chewing_get_phoneSeqLen(self->icPreEdit->context), self->statusFlags);
        update_lookup_table(self);
    }
}

void ibus_chewing_engine_refresh_property(IBusChewingEngine *self,
                                          [[maybe_unused]] const gchar *prop_name) {
    g_return_if_fail(self != NULL);
    g_return_if_fail(IBUS_IS_CHEWING_ENGINE(self));
    {
#ifndef UNIT_TEST
        g_debug("refresh_property(%s) status=%x", prop_name, self->statusFlags);

        if (g_strcmp0(prop_name, "InputMode") == 0) {

            ibus_property_set_label(self->InputMode,
                                    ibus_chewing_pre_edit_get_chi_eng_mode(self->icPreEdit)
                                        ? self->InputMode_label_chi
                                        : self->InputMode_label_eng);

#if IBUS_CHECK_VERSION(1, 5, 0)
            ibus_property_set_symbol(self->InputMode,
                                     ibus_chewing_pre_edit_get_chi_eng_mode(self->icPreEdit)
                                         ? self->InputMode_symbol_chi
                                         : self->InputMode_symbol_eng);
#endif

            ibus_engine_update_property(IBUS_ENGINE(self), self->InputMode);

        } else if (g_strcmp0(prop_name, "AlnumSize") == 0) {

            ibus_property_set_label(self->AlnumSize, chewing_get_ShapeMode(self->icPreEdit->context)
                                                         ? self->AlnumSize_label_full
                                                         : self->AlnumSize_label_half);

#if IBUS_CHECK_VERSION(1, 5, 0)
            ibus_property_set_symbol(self->AlnumSize,
                                     chewing_get_ShapeMode(self->icPreEdit->context)
                                         ? self->AlnumSize_symbol_full
                                         : self->AlnumSize_symbol_half);
#endif

            if (self->statusFlags & ENGINE_FLAG_PROPERTIES_REGISTERED)
                ibus_engine_update_property(IBUS_ENGINE(self), self->AlnumSize);

        } else if (g_strcmp0(prop_name, "setup_prop") == 0) {
#if IBUS_CHECK_VERSION(1, 5, 0)
            ibus_property_set_symbol(self->setup_prop, self->setup_prop_symbol);
#endif
            ibus_engine_update_property(IBUS_ENGINE(self), self->setup_prop);
        }
#endif
    }
}

/**
 * ibus_chewing_engine_refresh_property_list:
 * @self: this instances.
 *
 * Refresh the property list (language bar).
 **/
void ibus_chewing_engine_refresh_property_list(IBusChewingEngine *self) {
    g_return_if_fail(self != NULL);
    g_return_if_fail(IBUS_IS_CHEWING_ENGINE(self));
    {
#ifndef UNIT_TEST
        ibus_chewing_engine_refresh_property(self, "InputMode");
        ibus_chewing_engine_refresh_property(self, "AlnumSize");
        ibus_chewing_engine_refresh_property(self, "setup_prop");
#endif
    }
}

/**
 * ibus_chewing_engine_hide_property_list:
 * @self: this instances.
 *
 * Hide the property list (language bar).
 **/
void ibus_chewing_engine_hide_property_list(IBusChewingEngine *self) {
    g_return_if_fail(self != NULL);
    g_return_if_fail(IBUS_IS_CHEWING_ENGINE(self));
    {
#ifndef UNIT_TEST
        IBUS_ENGINE_GET_CLASS(self)->property_hide(IBUS_ENGINE(self), "AlnumSize");
#endif
    }
}

static IBusProperty *ibus_chewing_engine_get_ibus_property_by_name(IBusChewingEngine *self,
                                                                   const gchar *prop_name) {
    g_return_val_if_fail(self != NULL, (IBusProperty *)0);
    g_return_val_if_fail(IBUS_IS_CHEWING_ENGINE(self), (IBusProperty *)0);
    {
        if (g_strcmp0(prop_name, "InputMode") == 0) {
            return self->InputMode;
        } else if (g_strcmp0(prop_name, "AlnumSize") == 0) {
            return self->AlnumSize;
        } else if (g_strcmp0(prop_name, "setup_prop") == 0) {
            return self->setup_prop;
        }
        g_message("get_ibus_property_by_name(%s): NULL is returned", prop_name);
        return NULL;
    }
}

static void ibus_chewing_engine_page_up(IBusEngine *engine G_GNUC_UNUSED) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    if (is_password(self))
        return;
    ibus_chewing_pre_edit_process_key(self->icPreEdit, IBUS_KEY_Page_Up, 0);
    ibus_chewing_engine_update(self);
}

static void ibus_chewing_engine_page_down(IBusEngine *engine G_GNUC_UNUSED) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    if (is_password(self))
        return;
    ibus_chewing_pre_edit_process_key(self->icPreEdit, IBUS_KEY_Page_Down, 0);
    ibus_chewing_engine_update(self);
}

static void ibus_chewing_engine_cursor_up(IBusEngine *engine G_GNUC_UNUSED) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    if (is_password(self))
        return;
    ibus_chewing_pre_edit_process_key(self->icPreEdit, IBUS_KEY_Up, 0);
    ibus_chewing_engine_update(self);
}

static void ibus_chewing_engine_cursor_down(IBusEngine *engine G_GNUC_UNUSED) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    if (is_password(self))
        return;
    ibus_chewing_pre_edit_process_key(self->icPreEdit, IBUS_KEY_Down, 0);
    ibus_chewing_engine_update(self);
}

static void ibus_chewing_engine_set_capabilities(IBusEngine *engine G_GNUC_UNUSED, guint caps) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    self->capabilite = caps;
    g_message("***** set_capabilities(%x): statusFlags=%x", caps, self->statusFlags);
}

static void ibus_chewing_engine_property_show(IBusEngine *engine G_GNUC_UNUSED,
                                              const gchar *prop_name) {
    g_info("property_show(-, %s)", prop_name);
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);
    IBusProperty *prop = ibus_chewing_engine_get_ibus_property_by_name(self, prop_name);

    ibus_property_set_visible(prop, TRUE);
}

static void ibus_chewing_engine_property_hide(IBusEngine *engine G_GNUC_UNUSED,
                                              const gchar *prop_name) {
    g_info("property_hide(-, %s)", prop_name);
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);
    IBusProperty *prop = ibus_chewing_engine_get_ibus_property_by_name(self, prop_name);

    ibus_property_set_visible(prop, FALSE);
}

/**
 * ibus_chewing_engine_start:
 * @self: IBusChewingEngine instance.
 *
 * This is different with init. This will be called in the
 * beginning of reset, enable, and focus_in for setup.
 */
void ibus_chewing_engine_start(IBusChewingEngine *self) {
#ifndef UNIT_TEST
    if (!ibus_chewing_engine_has_status_flag(self, ENGINE_FLAG_PROPERTIES_REGISTERED)) {
        IBUS_ENGINE_GET_CLASS(self)->property_show(IBUS_ENGINE(self), "InputMode");
        IBUS_ENGINE_GET_CLASS(self)->property_show(IBUS_ENGINE(self), "AlnumSize");
        IBUS_ENGINE_GET_CLASS(self)->property_show(IBUS_ENGINE(self), "setup_prop");
        ibus_engine_register_properties(IBUS_ENGINE(self), self->prop_list);
        ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_PROPERTIES_REGISTERED);
    }
#endif
    ibus_chewing_engine_restore_mode(self);
    ibus_chewing_engine_refresh_property_list(self);
}

/**
 * ibus_chewing_engine_reset:
 * @self: IBusChewingEngine instance.
 *
 * Reset the outgoing and pre_edit buffer and cursor
 * chewing_reset will NOT called here, as it will chnage
 * the KBType and input mode.
 */
void ibus_chewing_engine_reset(IBusEngine *engine) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);
    g_message("* reset()");

    /* Always clean buffer */
    ibus_chewing_pre_edit_clear(self->icPreEdit);
#ifndef UNIT_TEST

    ibus_engine_hide_auxiliary_text(engine);
    ibus_engine_hide_lookup_table(engine);
    ibus_engine_update_preedit_text(engine, self->emptyText, 0, FALSE);

#endif
}

void ibus_chewing_engine_enable(IBusEngine *engine) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);
    g_message("* enable(): statusFlags=%x", self->statusFlags);
    ibus_chewing_engine_start(self);
    ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_ENABLED);
    if (self->prop_default_use_english_mode) {
        ibus_chewing_pre_edit_set_chi_eng_mode(self->icPreEdit, FALSE);
    }
}

void ibus_chewing_engine_disable(IBusEngine *engine) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);
    g_message("* disable(): statusFlags=%x", self->statusFlags);
    ibus_chewing_engine_clear_status_flag(self, ENGINE_FLAG_ENABLED);
}

void ibus_chewing_engine_focus_in(IBusEngine *engine) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);
    g_message("* focus_in(): statusFlags=%x", self->statusFlags);
    ibus_chewing_engine_start(self);
    /* Shouldn't have anything to commit when Focus-in */
    ibus_chewing_pre_edit_clear(self->icPreEdit);
    refresh_pre_edit_text(self);
    refresh_aux_text(self);
    refresh_outgoing_text(self);

    ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_FOCUS_IN);
    g_info("focus_in() statusFlags=%x: return", self->statusFlags);
}

void ibus_chewing_engine_focus_out(IBusEngine *engine) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);
    g_message("* focus_out(): statusFlags=%x", self->statusFlags);
    ibus_chewing_engine_clear_status_flag(self,
                                          ENGINE_FLAG_FOCUS_IN | ENGINE_FLAG_PROPERTIES_REGISTERED);
    ibus_chewing_engine_hide_property_list(self);

    if (self->prop_clean_buffer_focus_out) {
        /* Clean the buffer when focus out */
        ibus_chewing_pre_edit_clear(self->icPreEdit);
        refresh_pre_edit_text(self);
        refresh_aux_text(self);
    }

    g_debug("focus_out(): return");
}

#if IBUS_CHECK_VERSION(1, 5, 4)
void ibus_chewing_engine_set_content_type(IBusEngine *engine, guint purpose, guint hints) {
    g_debug("ibus_chewing_set_content_type(%d, %d)", purpose, hints);

    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    if (purpose == IBUS_INPUT_PURPOSE_PASSWORD || purpose == IBUS_INPUT_PURPOSE_PIN) {
        ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_IS_PASSWORD);
    } else {
        ibus_chewing_engine_clear_status_flag(self, ENGINE_FLAG_IS_PASSWORD);
    }
}
#endif

/*=================================================
 * Display text update routines
 *
 */

static void parent_commit_text(IBusEngine *iEngine) {
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(iEngine);

    g_message("* parent_commit_text(-): outgoingText=%s", self->outgoingText->text);
#ifdef UNIT_TEST
    printf("# * parent_commit_text(-, %s)\n", self->outgoingText->text);
#else
    ibus_engine_commit_text(iEngine, self->outgoingText);
#endif
}

static void parent_update_pre_edit_text_with_mode([[maybe_unused]] IBusEngine *iEngine,
                                                  IBusText *iText, guint cursor_pos,
                                                  gboolean visible, IBusPreeditFocusMode mode) {
#ifdef UNIT_TEST
    printf("# * parent_update_pre_edit_text_with_mode(-, %s, %u, %x, %x)\n", iText->text,
           cursor_pos, visible, mode);
#else
    ibus_engine_update_preedit_text_with_mode(iEngine, iText, cursor_pos, visible, mode);
#endif
}

static void parent_update_auxiliary_text([[maybe_unused]] IBusEngine *iEngine, IBusText *iText,
                                         gboolean visible) {
#ifdef UNIT_TEST
    printf("# * parent_update_auxiliary_text(-, %s, %x)\n", (iText) ? iText->text : "NULL",
           visible);
#else
    if (!visible || ibus_text_is_empty(iText)) {
        ibus_engine_hide_auxiliary_text(iEngine);
        return;
    }
    ibus_engine_update_auxiliary_text(iEngine, iText, visible);
    ibus_engine_show_auxiliary_text(iEngine);
#endif
}

static IBusText *decorate_pre_edit(IBusChewingPreEdit *icPreEdit,
                                   [[maybe_unused]] IBusCapabilite capabilite) {
    gchar *preEdit = ibus_chewing_pre_edit_get_pre_edit(icPreEdit);
    IBusText *iText = ibus_text_new_from_string(preEdit);
    gint chiSymbolCursor = chewing_cursor_Current(icPreEdit->context);
    gint charLen = (gint)g_utf8_strlen(preEdit, -1);

    g_debug("decorate_pre_edit() cursor=%d "
            "preEdit=%s charLen=%d",
            chiSymbolCursor, preEdit, charLen);

    /* Use single underline to mark whole pre-edit buffer
     */
    ibus_text_append_attribute(iText, IBUS_ATTR_TYPE_UNDERLINE, IBUS_ATTR_UNDERLINE_SINGLE, 0,
                               charLen);

    /* Use background color to show current cursor */
    if (chiSymbolCursor < charLen) {
        ibus_text_append_attribute(iText, IBUS_ATTR_TYPE_BACKGROUND, 0x00c8c8f0, chiSymbolCursor,
                                   chiSymbolCursor + 1);
        ibus_text_append_attribute(iText, IBUS_ATTR_TYPE_FOREGROUND, 0x00000000, chiSymbolCursor,
                                   chiSymbolCursor + 1);
    }

    return iText;
}

void refresh_pre_edit_text(IBusChewingEngine *self) {
    IBusText *iText = decorate_pre_edit(self->icPreEdit, self->capabilite);

    if (self->preEditText) {
        g_object_unref(self->preEditText);
    }
    self->preEditText = g_object_ref_sink(iText);
}

void update_pre_edit_text(IBusChewingEngine *self) {
    refresh_pre_edit_text(self);
    gboolean visible = TRUE;
    gint bpmfLen = self->icPreEdit->bpmfLen;

    IBusPreeditFocusMode mode;

    if (STRING_IS_EMPTY(self->preEditText->text)) {
        mode = IBUS_ENGINE_PREEDIT_CLEAR;
        visible = FALSE;
    } else {
        mode = IBUS_ENGINE_PREEDIT_COMMIT;
    }

    parent_update_pre_edit_text_with_mode(
        IBUS_ENGINE(self), self->preEditText,
        chewing_cursor_Current(self->icPreEdit->context) + bpmfLen, visible, mode);
}

void refresh_aux_text(IBusChewingEngine *self) {
    g_info("refresh_aux_text()");

    if (self->auxText != NULL) {
        g_object_unref(self->auxText);
    }

    /* Make auxText (text to be displayed in auxiliary
     * candidate window). Use auxText to show messages
     * from libchewing, such as "已有：".
     */

    gboolean showPageNumber = self->prop_show_page_number;

    if (chewing_aux_Length(self->icPreEdit->context) > 0) {
        g_info("update_aux_text() chewing_aux_Length=%x",
               chewing_aux_Length(self->icPreEdit->context));
        gchar *auxStr = chewing_aux_String(self->icPreEdit->context);

        g_info("update_aux_text() auxStr=%s", auxStr);
        self->auxText = g_object_ref_sink(ibus_text_new_from_string(auxStr));
        chewing_free(auxStr);
    } else if (self->prop_notify_mode_change && self->pending_notify_chinese_english_mode) {
        self->pending_notify_chinese_english_mode = FALSE;
        char *auxStr = is_chinese_mode(self) ? _("Chinese Mode") : _("English Mode");
        self->auxText = g_object_ref_sink(ibus_text_new_from_static_string(auxStr));
    } else if (self->prop_notify_mode_change && self->pending_notify_fullwidth_mode) {
        self->pending_notify_fullwidth_mode = FALSE;
        char *auxStr = is_fullwidth_mode(self) ? _("Fullwidth Mode") : _("Halfwidth Mode");
        self->auxText = g_object_ref_sink(ibus_text_new_from_static_string(auxStr));
    } else if (showPageNumber && (chewing_cand_TotalPage(self->icPreEdit->context) > 0)) {
        int TotalPage = chewing_cand_TotalPage(self->icPreEdit->context);
        int currentPage = chewing_cand_CurrentPage(self->icPreEdit->context) + 1;
        self->auxText =
            g_object_ref_sink(ibus_text_new_from_printf("(%i/%i)", currentPage, TotalPage));
    } else {
        /* clear out auxText, otherwise it will be
         * displayed continually. */
        self->auxText = g_object_ref_sink(ibus_text_new_from_static_string(""));
    }
}

void update_aux_text(IBusChewingEngine *self) {
    g_debug("update_aux_text()");
    refresh_aux_text(self);
    parent_update_auxiliary_text(IBUS_ENGINE(self), self->auxText, TRUE);
}

void update_lookup_table(IBusChewingEngine *self) {
    g_debug("update_lookup_table() CurrentPage=%d",
            chewing_cand_CurrentPage(self->icPreEdit->context));

    gboolean isShow = ibus_chewing_pre_edit_has_flag(self->icPreEdit, FLAG_TABLE_SHOW);

    if (isShow) {
#ifndef UNIT_TEST
        ibus_engine_update_lookup_table(IBUS_ENGINE(self), self->icPreEdit->iTable, isShow);
        ibus_engine_show_lookup_table(IBUS_ENGINE(self));
#endif
    } else {
#ifndef UNIT_TEST
        ibus_engine_update_lookup_table(IBUS_ENGINE(self), self->icPreEdit->iTable, isShow);
        ibus_engine_hide_lookup_table(IBUS_ENGINE(self));
#endif
    }
}

void refresh_outgoing_text(IBusChewingEngine *self) {
    gchar *outgoingStr = ibus_chewing_pre_edit_get_outgoing(self->icPreEdit);

    g_info("refresh_outgoing_text() outgoingStr=|%s|", outgoingStr);

    if (self->outgoingText) {
        g_object_unref(self->outgoingText);
    }
    self->outgoingText = g_object_ref_sink(ibus_text_new_from_string(outgoingStr));
    g_debug("refresh_outgoing_text() outgoingText=|%s|", self->outgoingText->text);
}

void commit_text(IBusChewingEngine *self) {
    refresh_outgoing_text(self);
    if (!ibus_text_is_empty(self->outgoingText) ||
        !ibus_chewing_engine_has_status_flag(self, ENGINE_FLAG_FOCUS_IN)) {
        parent_commit_text(IBUS_ENGINE(self));
    }

    ibus_chewing_pre_edit_clear_outgoing(self->icPreEdit);
}

gboolean ibus_chewing_engine_process_key_event(IBusEngine *engine, KSym keySym, guint keycode,
                                               KeyModifiers unmaskedMod) {
    g_message("******** process_key_event(-,%x(%s),%x,%x) %s", keySym, ibus_keyval_name(keySym),
              keycode, unmaskedMod, modifiers_to_string(unmaskedMod));

    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    if (unmaskedMod & IBUS_MOD4_MASK)
        return FALSE;

    if (is_password(self))
        return FALSE;

    KSym kSym =
        ibus_chewing_pre_edit_key_code_to_key_sym(self->icPreEdit, keySym, keycode, unmaskedMod);

    gboolean result = ibus_chewing_pre_edit_process_key(self->icPreEdit, kSym, unmaskedMod);

    g_message("process_key_event() result=%d", result);
    ibus_chewing_engine_update(self);

    if (kSym == IBUS_KEY_Shift_L || kSym == IBUS_KEY_Shift_R || kSym == IBUS_KEY_Caps_Lock) {
        /* Refresh property list (language bar) only when
         * users toggle Chi-Eng Mode or Shape Mode with
         * Shift or Caps Lock, otherwise the bar will
         * stick to the cursor and block the candidats
         * list.
         */
        ibus_chewing_engine_refresh_property_list(self);
    }

    return result;
}

/*===================================================
 * Mouse events
 */
void ibus_chewing_engine_candidate_clicked(IBusEngine *engine, guint index, guint button,
                                           guint state) {
    g_info("*** candidate_clicked(-, %x, %x, %x) ... proceed.", index, button, state);
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    if (is_password(self))
        return;
    if ((gint)index >= chewing_get_candPerPage(self->icPreEdit->context)) {
        g_debug("candidate_clicked() index out of ranged");
        return;
    }
    if (ibus_chewing_pre_edit_has_flag(self->icPreEdit, FLAG_TABLE_SHOW)) {
        gint *selKeys = chewing_get_selKey(self->icPreEdit->context);
        KSym k = (KSym)selKeys[index];

        ibus_chewing_pre_edit_process_key(self->icPreEdit, k, 0);
        chewing_free(selKeys);
        ibus_chewing_engine_update(self);
    } else {
        g_debug("candidate_clicked() ... candidates are not showing");
    }
}

void ibus_chewing_engine_property_activate(IBusEngine *engine, const gchar *prop_name,
                                           guint prop_state) {
    g_info("property_activate(-, %s, %u)", prop_name, prop_state);
    IBusChewingEngine *self = IBUS_CHEWING_ENGINE(engine);

    if (g_strcmp0(prop_name, "InputMode") == 0) {
        /* Toggle Chinese <-> English */
        ibus_chewing_pre_edit_toggle_chi_eng_mode(self->icPreEdit);
        g_info("property_activate chinese=%d", is_chinese_mode(self));
        ibus_chewing_engine_refresh_property(self, prop_name);
    } else if (g_strcmp0(prop_name, "AlnumSize") == 0) {
        /* Toggle Full <-> Half */
        ibus_chewing_pre_edit_toggle_full_half_mode(self->icPreEdit);
        g_info("property_activate fullwidth=%d", is_fullwidth_mode(self));
        ibus_chewing_engine_refresh_property(self, prop_name);
    } else if (g_strcmp0(prop_name, "setup_prop") == 0) {
        /* open preferences window */
        char *argv[] = {QUOTE_ME(LIBEXEC_DIR) "/ibus-setup-chewing", NULL};
        g_spawn_async(NULL, argv, NULL, G_SPAWN_DEFAULT, NULL, NULL, NULL, NULL);
    } else {
        g_debug("property_activate(-, %s, %u) not recognized", prop_name, prop_state);
    }
}

char ibus_chewing_engine_get_default_english_case(IBusChewingEngine *self) {
    char *prop = self->prop_default_english_case;
    return g_strcmp0(prop, "lowercase") == 0 ? 'l' : g_strcmp0(prop, "uppercase") == 0 ? 'u' : 'n';
}

char ibus_chewing_engine_get_chinese_english_toggle_key(IBusChewingEngine *self) {
    char *prop = self->prop_chi_eng_mode_toggle;
    return g_strcmp0(prop, "caps_lock") == 0 ? 'c'
           : g_strcmp0(prop, "shift") == 0   ? 's'
           : g_strcmp0(prop, "shift_l") == 0 ? 'l'
           : g_strcmp0(prop, "shift_r") == 0 ? 'r'
                                             : 'n';
    return 'n';
}

gboolean ibus_chewing_engine_use_vertical_lookup_table(IBusChewingEngine *self) {
    return self->prop_vertical_lookup_table;
}

gboolean ibus_chewing_engine_use_system_layout(IBusChewingEngine *self) {
    return self->prop_ibus_use_system_layout;
}

void ibus_chewing_engine_notify_chinese_english_mode_change(IBusChewingEngine *self) {
    self->pending_notify_chinese_english_mode = TRUE;
}
void ibus_chewing_engine_notify_fullwidth_mode_change(IBusChewingEngine *self) {
    self->pending_notify_fullwidth_mode = TRUE;
}
