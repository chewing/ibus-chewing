/*
 * Copyright © 2011  Red Hat, Inc. All rights reserved.
 * Copyright © 2011  Ding-Yi Chen <dchen at redhat.com>
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

#pragma once

#include <chewing.h>
#include <glib.h>
#include <ibus.h>

#define CHEWING_CHECK_VERSION(major, minor, patch)                                                 \
    (CHEWING_VERSION_MAJOR > (major) ||                                                            \
     (CHEWING_VERSION_MAJOR == (major) && CHEWING_VERSION_MINOR > (minor)) ||                      \
     (CHEWING_VERSION_MAJOR == (major) && CHEWING_VERSION_MINOR == (minor) &&                      \
      CHEWING_VERSION_PATCH >= (patch)))

#define IBUS_CHEWING_LOG(level, msg, args...) mkdg_log_domain("ibus-chewing", level, msg, ##args)

typedef guint KSym;

typedef guint KeyModifiers;

typedef enum {
    CHEWING_KBTYPE_INVALID = -1,
    CHEWING_KBTYPE_DEFAULT,
    CHEWING_KBTYPE_HSU,
    CHEWING_KBTYPE_IBM,
    CHEWING_KBTYPE_GIN_YIEH,
    CHEWING_KBTYPE_ETEN,
    CHEWING_KBTYPE_ETEN26,
    CHEWING_KBTYPE_DVORAK,
    CHEWING_KBTYPE_DVORAK_HSU,
    CHEWING_KBTYPE_DACHEN_CP26,
    CHEWING_KBTYPE_HAN_YU,
    CHEWING_KBTYPE_LUOMA_PINYIN,
    CHEWING_KBTYPE_MSP2,
    CHEWING_KBTYPE_CARPALX,
    CHEWING_KBTYPE_COLEMAK_DH_ANSI,
    CHEWING_KBTYPE_COLEMAK_DH_ORTH,
    CHEWING_KBTYPE_WORKMAN,
    CHEWING_KBTYPE_COLEMAK,
} ChewingKbType;

KSym key_sym_KP_to_normal(KSym k);

const gchar *modifiers_to_string(guint modifier);

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

void mkdg_log(MkdgLogLevel level, const char *format, ...);

void mkdg_log_domain(const char *domain, MkdgLogLevel level, const char *format, ...);

/**************************************
 * String Utility Macros and Functions
 */

#define QUOTE_ME_INNER(s) #s
#define QUOTE_ME(s) QUOTE_ME_INNER(s)

#define STRING_IS_EMPTY(str) (!str || (str[0] == '\0'))

/**************************************
 * Flag Utility Macros
 */

#define mkdg_clear_flag(flagSet, flag) (flagSet &= ~(flag))
#define mkdg_has_flag(flagSet, flag) ((flagSet & flag) == flag)
#define mkdg_set_flag(flagSet, flag) (flagSet |= flag)
