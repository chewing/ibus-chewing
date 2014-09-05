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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _MAKER_DIALOG_UTIL_H_
#define _MAKER_DIALOG_UTIL_H_
#include <string.h>


typedef enum {
    ERROR,
    WARN,
    MSG,
    INFO,
    DEBUG
} MkdgLogLevel;

void mkdg_log_set_level(MkdgLogLevel level);

void mkdg_log_set_domain(const gchar * domain);

void mkdg_log(MkdgLogLevel level, const gchar *format, ...);

gchar *mkdg_g_value_to_string(GValue * value);

gboolean mkdg_g_value_from_string(GValue * value, gchar * str);

/**************************************
 * String Utility Macros
 */

#define QUOTE_ME_INNER(s) #s
#define QUOTE_ME(s) QUOTE_ME_INNER(s)

#define STRING_IS_EMPTY(str) \
    (!str  || \
     (str[0] == '\0')\
    )

#define STRING_EQUALS(str1, str2) \
    ((str1==NULL && str2==NULL) ||\
     ((str1!=NULL && str2!=NULL) && strcmp(str1, str2)==0) \
    )

#endif   /* _MAKER_DIALOG_UTIL_H_ */
