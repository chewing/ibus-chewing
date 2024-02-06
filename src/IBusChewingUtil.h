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

#ifndef _IBUS_CHEWING_UTIL_H_
#define _IBUS_CHEWING_UTIL_H_
#include <chewing.h>
#include <ctype.h>
#include <glib.h>
#include <ibus.h>
#include <string.h>

#define ZHUYIN_BUFFER_SIZE 12

#define CHEWING_CHECK_VERSION(major, minor, patch)                             \
    (CHEWING_VERSION_MAJOR > (major) ||                                        \
     (CHEWING_VERSION_MAJOR == (major) && CHEWING_VERSION_MINOR > (minor)) ||  \
     (CHEWING_VERSION_MAJOR == (major) && CHEWING_VERSION_MINOR == (minor) &&  \
      CHEWING_VERSION_PATCH >= (patch)))

#define IBUS_CHEWING_LOG_DOMAIN "ibus-chewing"

#define IBUS_CHEWING_LOG(level, msg, args...)                                  \
    mkdg_log_domain(IBUS_CHEWING_LOG_DOMAIN, level, msg, ##args)

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
    CHEWING_KBTYPE_HAN_YU
#if CHEWING_CHECK_VERSION(0, 3, 4)
    ,
    CHEWING_KBTYPE_LUOMA_PINYIN,
    CHEWING_KBTYPE_MSP2
#endif
} ChewingKbType;

KSym key_sym_KP_to_normal(KSym k);

const char *key_sym_get_name(KSym k);

const gchar *modifier_get_string(guint modifier);

const gchar *modifiers_to_string(guint modifier);

#endif /* _IBUS_CHEWING_UTIL_H_ */
