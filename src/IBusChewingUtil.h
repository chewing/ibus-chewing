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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _IBUS_CHEWING_UTIL_H_
#    define _IBUS_CHEWING_UTIL_H_
#    include <ctype.h>
#    include <string.h>
#    include <glib.h>
#    include <ibus.h>
#    include <chewing.h>
#    include <X11/Xlib.h>
#    include <X11/XKBlib.h>

#    define ZHUYIN_BUFFER_SIZE 12

#    define CHEWING_CHECK_VERSION(major,minor,patch)	\
    (CHEWING_VERSION_MAJOR > (major) || \
     (CHEWING_VERSION_MAJOR == (major) && CHEWING_VERSION_MINOR > (minor)) || \
     (CHEWING_VERSION_MAJOR == (major) && CHEWING_VERSION_MINOR == (minor) && \
      CHEWING_VERSION_PATCH >= (patch) \
      )\
    )

#    define IBUS_CHEWING_LOG_DOMAIN "ibus-chewing"

#    define IBUS_CHEWING_LOG(level, msg, args...) \
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
#    if CHEWING_CHECK_VERSION(0,3,4)
        , CHEWING_KBTYPE_LUOMA_PINYIN,
    CHEWING_KBTYPE_MSP2
#    endif
} ChewingKbType;


gint get_tone(ChewingKbType kbType, KSym k);

void add_tone(char *str, gint tone);

KSym key_sym_KP_to_normal(KSym k);

const char *key_sym_get_name(KSym k);

gboolean is_caps_led_on(Display * pDisplay);

void set_caps_led(gboolean on, Display * pDisplay);

const gchar *modifier_get_string(guint modifier);

const gchar *modifiers_to_string(guint modifier);

gboolean ibus_chewing_property_get_state(IBusProperty * prop);

#endif                          /* _IBUS_CHEWING_UTIL_H_ */
