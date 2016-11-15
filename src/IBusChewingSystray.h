/*
 * Copyright © 2016  Red Hat, Inc. All rights reserved.
 * Copyright © 2016  Ding-Yi Chen <dchen@redhat.com>
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
 * SECTION:IBusChewingSystray
 * @short_description: Systray support
 * @title: IBusChewingSystray
 * @stability: Stable
 * @include: IbusChewingSystray
 *
 * Systray support that shows:
 *   Chinese/English state
 *
 */

#ifndef _IBUS_CHEWING_SYSTRAY_H_
#    define _IBUS_CHEWING_SYSTRAY_H_
#    include <stdarg.h>
#    include <glib.h>
#    include <gtk/gtk.h>
#    include <locale.h>
#    include <glib/gi18n.h>
#    include "MakerDialogUtil.h"
#    include "IBusChewingPreEdit.h"
#    define IBUS_CHEWING_SYSTRAY_ICON_COUNT_MAX 5
typedef struct _IBusChewingSystrayIcon IBusChewingSystrayIcon;

typedef void (*IBusChewingSystrayClickFunc) (IBusChewingSystrayIcon *
                                             iSystrayIcon, gpointer userData);

typedef enum {
    IBUS_CHEWING_SYSTRAY_CHINESE_FLAG = 1,
    IBUS_CHEWING_SYSTRAY_FULL_HALF_SHAPE_FLAG = 2,
} IBusChewingSystrayFlag;

/** 
 * IBusChewingSystrayIcon:
 * Systray Icon for ibus-chewin
 *
 * @icon: The actual systray object
 * @value: An unsigned integer that hold the state of the systray icon.
 * also the index (starting from 0) for icon file. Thus it should not exceed length
 * of iconFileArray
 * @iconFileArray: Array of icon files.
 * @leftClickFunc: Callback function handles left click. NULL means do nothing
 * @leftClickData: Data that passed to leftClickFunc
 * @rightClickFunc: Callback function handles right click. NULL means do nothing
 * @rightClickData: Data that passed to rightClickFunc
 */
struct _IBusChewingSystrayIcon {
    GtkStatusIcon *icon;
    guint value;
    GPtrArray *iconFileArray;
    GPtrArray *iconCacheArray;
    IBusChewingSystrayClickFunc leftClickFunc;
    gpointer leftClickData;
    IBusChewingSystrayClickFunc rightClickFunc;
    gpointer rightClickData;
};

void ibus_chewing_systray_icon_free(IBusChewingSystrayIcon * self);

/**
 * ibus_chewing_systray_icon_get_icon_file:
 * @self: An IBusChewingSystrayIcon
 * @index: File index that starts from 0
 */
#    define ibus_chewing_systray_icon_get_icon_file(self, index) (const gchar *) g_ptr_array_index(self->iconFileArray,index)

/**
 * ibus_chewing_systray_icon_set_value:
 * @self: An IBusChewingSystrayIcon
 * @value: The value to set.
 *
 * Sets value to IBusChewingSystrayIcon.
 * This does not update display icon, see ibus_chewing_systray_icon_update for that.
 */
void ibus_chewing_systray_icon_set_value(IBusChewingSystrayIcon *
                                         self, guint value);

/**
 * ibus_chewing_systray_icon_set_visible:
 * @self: An IBusChewingSystrayIcon
 * @visible: TRUE for visible, FALSE for not visible.
 *
 * Sets whether this SystrayIcon visible.
 */
void ibus_chewing_systray_icon_set_visible(IBusChewingSystrayIcon *
                                           self, gboolean visible);

/**
 * ibus_chewing_systray_icon_update:
 * @self: An IBusChewingSystrayIcon
 *
 * Updates display icon according to value.
 */
void ibus_chewing_systray_icon_update(IBusChewingSystrayIcon * self);


#    include "ibus-chewing-engine.h"

IBusChewingSystrayIcon
    * ibus_chewing_systray_chi_eng_icon_new(IBusChewingEngine * iEngine);

/**
 * ibus_chewing_systray_chi_eng_icon_create_or_hide:
 * @iEngine: An IBusChewingEngine
 *
 * Create or destroy systray icon depending on property 'show-systray'
 * @returns: TRUE if systray icon created; FALSE otherwise.
 */
gboolean
ibus_chewing_systray_chi_eng_icon_create_or_hide(IBusChewingEngine * iEngine);

void ibus_chewing_systray_chi_eng_icon_refresh_value(IBusChewingEngine *
                                                     iEngine);
#endif
