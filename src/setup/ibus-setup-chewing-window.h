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

#pragma once

#include <adwaita.h>

G_BEGIN_DECLS
#define IBUS_SETUP_CHEWING_TYPE_WINDOW (ibus_setup_chewing_window_get_type())
G_DECLARE_FINAL_TYPE(IbusSetupChewingWindow, ibus_setup_chewing_window,
                     IBUS_SETUP_CHEWING, WINDOW, AdwApplicationWindow)
G_END_DECLS
