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

#ifndef _MAKER_DIALOG_UTIL_H_
#define _MAKER_DIALOG_UTIL_H_
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <glib-object.h>

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
typedef enum {
    ERROR,
    WARN,
    MSG,
    INFO,
    DEBUG
} MkdgLogLevel;

void mkdg_log_set_level(MkdgLogLevel level);

void mkdg_log_set_domain(const gchar * domain);

void mkdg_log_set_file(FILE * file);

void mkdg_log(MkdgLogLevel level, const gchar * format, ...);

void mkdg_log_domain(const gchar * domain, MkdgLogLevel level,
		     const gchar * format, ...);

gchar *mkdg_g_value_to_string(GValue * value);

gboolean mkdg_g_value_reset(GValue * value, GType type,
			    gboolean overwrite);

gboolean mkdg_g_value_from_string(GValue * value, const gchar * str);

gint mkdg_g_ptr_array_find_string(GPtrArray * array, const gchar * str);

#define mkdg_g_value_is_boolean(value) (G_VALUE_TYPE(value)==G_TYPE_BOOLEAN)

#define mkdg_g_value_is_int(value) (G_VALUE_TYPE(value)==G_TYPE_INT)

#define mkdg_g_value_is_uint(value) (G_VALUE_TYPE(value)==G_TYPE_UINT)

#define mkdg_g_value_is_string(value) (G_VALUE_TYPE(value)==G_TYPE_STRING)

void mkdg_g_variant_to_g_value(GVariant * gVar, GValue * value);

GVariant *mkdg_g_value_to_g_variant(GValue * value);


/**************************************
 * String Utility Macros and Functions
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

gchar *mkdg_str_dash_to_camel(const gchar * argStr);

/**************************************
 * Flag Utility Macros
 */

#define mkdg_clear_flag(flagSet, flag) (flagSet &= ~(flag))
#define mkdg_has_flag(flagSet, flag) ((flagSet & flag) == flag)
#define mkdg_set_flag(flagSet, flag) (flagSet |= flag)

/**************************************
 * XML data structure and functions
 */

typedef enum {
    MKDG_XML_TAG_TYPE_NO_TAG,
    MKDG_XML_TAG_TYPE_EMPTY,
    MKDG_XML_TAG_TYPE_SHORT,
    MKDG_XML_TAG_TYPE_LONG,
    MKDG_XML_TAG_TYPE_BEGIN_ONLY,
    MKDG_XML_TAG_TYPE_END_ONLY,
} MkdgXmlTagType;

gchar *mkdg_xml_attr_append(gchar * buf, gint bufferSize,
			    const gchar * attr, const gchar * value);

gboolean mkdg_xml_tags_write(FILE * outF, const gchar * tagName,
			     MkdgXmlTagType type, const gchar * attribute,
			     const gchar * value);

#define MAKER_DIALOG_VALUE_LENGTH 200
#endif				/* _MAKER_DIALOG_UTIL_H_ */
