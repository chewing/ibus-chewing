/*
 * Copyright © 2014  Red Hat, Inc. All rights reserved.
 * Copyright © 2014  Ding-Yi Chen <dchen at redhat.com>
 *
 * This file is part of the ibus-chewing Project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

/**
 * SECTION:MakerDialogUtil.h
 * @short_description: Utility functions for MakerDialog
 * @title: MakerDialog Util
 * @stability: Stable
 * @include: MakerDialogUtil.h
 *
 * Utility functions for MakerDialog.
 * The functions have <emphasis>mkdg_</emphasis> prefix,
 * while definitions have <emphasis>Mkdg</emphasis> prefix.
 */

#pragma once

#include <glib.h>
#include <string.h>

/**
 * MkdgLogLevel:
 * @ERROR: Show auses of failure, the program should stop
 * @WARN:  Show issues to be address, the program will still run.
 * @MSG:   Show normal message
 * @INFO:  Show information message
 * @DEBUG: Show debug message
 *
 * Message verbose level, from low to high. Similar to Log level of Gtk.
 *
 */
typedef enum { ERROR, WARN, MSG, INFO, DEBUG } MkdgLogLevel;

void mkdg_log_set_level(MkdgLogLevel level);

void mkdg_log(MkdgLogLevel level, const gchar *format, ...);

void mkdg_log_domain(const gchar *domain, MkdgLogLevel level,
                     const gchar *format, ...);

/**************************************
 * String Utility Macros and Functions
 */

#define QUOTE_ME_INNER(s) #s
#define QUOTE_ME(s) QUOTE_ME_INNER(s)

#define STRING_IS_EMPTY(str) (!str || (str[0] == '\0'))

#define STRING_EQUALS(str1, str2)                                              \
    ((str1 == NULL && str2 == NULL) ||                                         \
     ((str1 != NULL && str2 != NULL) && strcmp(str1, str2) == 0))

/**************************************
 * Flag Utility Macros
 */

#define mkdg_clear_flag(flagSet, flag) (flagSet &= ~(flag))
#define mkdg_has_flag(flagSet, flag) ((flagSet & flag) == flag)
#define mkdg_set_flag(flagSet, flag) (flagSet |= flag)
