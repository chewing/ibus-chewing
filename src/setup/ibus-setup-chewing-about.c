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

void show_about(GtkWidget *self) {
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
    adw_show_about_dialog(
        GTK_WIDGET(self),
        "application-name", _("IBus Chewing"),
        "application-icon", "application-x-executable",
        "version", QUOTE_ME(PRJ_VER),
        "website", "https://chewing.im",
        "issue-url", "https://github.com/chewing/ibus-chewing",
        "support-url", "https://github.com/chewing/ibus-chewing/discussions",
        //"debug-info", TODO return settings,
        "copyright", "© 2024 libchewing Core Team",
        "license-type", GTK_LICENSE_GPL_2_0,
        "developers", developers,
        "translator-credits", _("translator-credits"),
        NULL
    );
    // clang-format on
}