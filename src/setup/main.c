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

int main(int argc, char *argv[]) {
    g_autoptr(IbusSetupChewingApplication) app = NULL;
    int ret;

    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    app = ibus_setup_chewing_application_new(
        "org.freedesktop.IBus.Chewing.Setup", G_APPLICATION_DEFAULT_FLAGS);
#ifdef UNIT_TEST
    g_idle_add(G_SOURCE_FUNC(g_application_quit), G_APPLICATION(app));
#endif
    ret = g_application_run(G_APPLICATION(app), argc, argv);

    return ret;
}
