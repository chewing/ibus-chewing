/* ibus-setup-chewing-window.h
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

#include "ibus-setup-chewing-about.h"

#include <glib/gi18n.h>
#include <stdio.h>

#define QUOTE_ME_INNER(s) #s
#define QUOTE_ME(s) QUOTE_ME_INNER(s)

static char *generate_debug_info(void) {
    GString *string = g_string_new(NULL);

    g_string_append_printf(string, "ibus-chewing: %s\n\n", QUOTE_ME(PRJ_VER));
    g_string_append(string, "Compiled against:\n");
    g_string_append_printf(string, "- ibus: %s\n", QUOTE_ME(IBUS_VERSION));
    g_string_append_printf(string, "- libchewing: %s\n",
                           QUOTE_ME(CHEWING_VERSION));

    g_string_append(string, "\nRunning against:\n");
    g_string_append_printf(string, "- libchewing: n/a\n");

    g_string_append(string, "\n");
    {
        g_autoptr(GSettings) settings =
            g_settings_new("org.freedesktop.IBus.Chewing");
        gchar *kb_type = g_settings_get_string(settings, "kb-type");
        gchar *sel_keys = g_settings_get_string(settings, "sel-keys");
        gchar *chi_eng_mode_toggle =
            g_settings_get_string(settings, "chi-eng-mode-toggle");
        gchar *sync_caps_lock =
            g_settings_get_string(settings, "sync-caps-lock");
        gchar *default_english_case =
            g_settings_get_string(settings, "default-english-case");

        g_string_append(string, "Settings:\n");
        g_string_append_printf(string, "- kb-type: %s\n", kb_type);
        g_string_append_printf(string, "- sel-keys: %s\n", sel_keys);
        g_string_append_printf(
            string, "- plain-zhuyin: %d\n",
            g_settings_get_boolean(settings, "plain-zhuyin"));
        g_string_append_printf(
            string, "- conversion-engine: %d\n",
            g_settings_get_enum(settings, "conversion-engine"));
        g_string_append_printf(
            string, "- auto-shift-cur: %d\n",
            g_settings_get_boolean(settings, "auto-shift-cur"));
        g_string_append_printf(
            string, "- add-phrase-direction: %d\n",
            g_settings_get_boolean(settings, "add-phrase-direction"));
        g_string_append_printf(
            string, "- clean-buffer-focus-out: %d\n",
            g_settings_get_boolean(settings, "clean-buffer-focus-out"));
        g_string_append_printf(
            string, "- easy-symbol-input: %d\n",
            g_settings_get_boolean(settings, "easy-symbol-input"));
        g_string_append_printf(
            string, "- esc-clean-all-buf: %d\n",
            g_settings_get_boolean(settings, "esc-clean-all-buf"));
        g_string_append_printf(
            string, "- max-chi-symbol-len: %d\n",
            g_settings_get_int(settings, "max-chi-symbol-len"));
        g_string_append_printf(string, "- chi-eng-mode-toggle: %s\n",
                               chi_eng_mode_toggle);
        g_string_append_printf(string, "- sync-caps-lock: %s\n",
                               sync_caps_lock);
        g_string_append_printf(string, "- default-english-case: %s\n",
                               default_english_case);
        g_string_append_printf(string, "- cand-per-page: %d\n",
                               g_settings_get_uint(settings, "cand-per-page"));
        g_string_append_printf(
            string, "- show-page-number: %d\n",
            g_settings_get_boolean(settings, "show-page-number"));
        g_string_append_printf(
            string, "- phrase-choice-from-last: %d\n",
            g_settings_get_boolean(settings, "phrase-choice-from-last"));
        g_string_append_printf(
            string, "- space-as-selection: %d\n",
            g_settings_get_boolean(settings, "space-as-selection"));
        g_string_append_printf(
            string, "- vertical-lookup-table: %d\n",
            g_settings_get_boolean(settings, "vertical-lookup-table"));

        g_free(kb_type);
        g_free(sel_keys);
        g_free(chi_eng_mode_toggle);
        g_free(sync_caps_lock);
        g_free(default_english_case);
    }

    g_string_append(string, "\n");
    {
        const char *desktop = g_getenv("XDG_CURRENT_DESKTOP");
        const char *session_desktop = g_getenv("XDG_SESSION_DESKTOP");
        const char *session_type = g_getenv("XDG_SESSION_TYPE");
        const char *lang = g_getenv("LANG");

        g_string_append(string, "Environment:\n");
        g_string_append_printf(string, "- Desktop: %s\n", desktop);
        g_string_append_printf(string, "- Session: %s (%s)\n", session_desktop,
                               session_type);
        g_string_append_printf(string, "- Language: %s\n", lang);
    }

    return g_string_free_and_steal(string);
}

void show_about(GtkWidget *self) {
    AdwDialog *about;
    char *debug_info = generate_debug_info();

    // clang-format off
    const char *developers[] = {
        "Peng Huang",
        "Ding-Yi Chen",
        "Hiunn-hué",
        "Kan-Ru Chen",
        NULL
    };
    // clang-format on

    // clang-format off
    about = g_object_new(
        ADW_TYPE_ABOUT_DIALOG,
        "application-name", _("IBus Chewing"),
        "application-icon", "im.chewing.Chewing",
        "version", QUOTE_ME(PRJ_VER),
        "website", "https://chewing.im",
        "issue-url", "https://github.com/chewing/ibus-chewing/issues",
        "debug-info", debug_info,
        "debug-info-filename", "ibus-chewing-debug-info.txt",
        "copyright", "© 2024 libchewing Core Team",
        "license-type", GTK_LICENSE_GPL_2_0,
        "developers", developers,
        "translator-credits", _("translator-credits"),
        NULL
    );
    // clang-format on

    adw_about_dialog_add_link(ADW_ABOUT_DIALOG(about), _("_Mailing list"),
                              "https://groups.google.com/g/chewing");

    adw_about_dialog_add_link(ADW_ABOUT_DIALOG(about), _("_Chat"),
                              "https://matrix.to/#/#chewing-users:matrix.org");

    adw_dialog_present(about, GTK_WIDGET(self));

    g_free(debug_info);
}