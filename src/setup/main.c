/* main.c
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

#include <glib/gi18n.h>

#include "ibus-setup-chewing-application.h"

static gboolean quit = FALSE;

static GOptionEntry entries[] = {
    {"quit", 'q', 0, G_OPTION_ARG_NONE, &quit,
     "Cause the application to quit immediately after launch", NULL},
    G_OPTION_ENTRY_NULL};

static void application_quit(gpointer user_data) {
    IbusSetupChewingApplication *self = user_data;
    GAction *quit_action;

    quit_action = g_action_map_lookup_action(G_ACTION_MAP(self), "quit");
    g_action_activate(G_ACTION(quit_action), NULL);
}

int main(int argc, char *argv[]) {
    GError *error = NULL;
    GOptionContext *context;
    g_autoptr(IbusSetupChewingApplication) app = NULL;
    int ret;

    context = g_option_context_new("- chewing settings");
    g_option_context_add_main_entries(context, entries, GETTEXT_PACKAGE);

    if (!g_option_context_parse(context, &argc, &argv, &error)) {
        g_print("option parsing failed: %s\n", error->message);
        exit(1);
    }

    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    app = ibus_setup_chewing_application_new(
        "org.freedesktop.IBus.Chewing.Setup", G_APPLICATION_DEFAULT_FLAGS);
    if (quit) {
        g_idle_add(G_SOURCE_FUNC(application_quit), app);
    }
    ret = g_application_run(G_APPLICATION(app), argc, argv);

    return ret;
}
