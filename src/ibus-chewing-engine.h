/*
 * Copyright © 2009,2010  Red Hat, Inc. All rights reserved.
 * Copyright © 2009,2010  Ding-Yi Chen <dchen at redhat.com>
 *
 * This file is part of the ibus-chewing Project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that ill be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <stdlib.h>
#include <libintl.h>
#include <ibus.h>
#include <chewing.h>
#include <string.h>
#include <stdio.h>
#define GETTEXT_PACKAGE "gtk30"
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <ctype.h>
#include "IBusChewingUtil.h"
#include "IBusChewingProperties.h"
#include "IBusChewingPreEdit.h"
#include "GSettingsBackend.h"

#include <glib.h>
#include <glib-object.h>
#ifndef __IBUS_CHEWING_ENGINE_H__
#define __IBUS_CHEWING_ENGINE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef enum {
	CHEWING_INPUT_STYLE_IN_APPLICATION,
	CHEWING_INPUT_STYLE_IN_CANDIDATE
} ChewingInputStyle;
#define CHEWING_TYPE_INPUT_STYLE chewing_input_style_get_type()
GType chewing_input_style_get_type (void) G_GNUC_CONST;


typedef enum {
	ENGINE_FLAG_INITIALIZED = 0x1,
	ENGINE_FLAG_ENABLED = 0x2,
	ENGINE_FLAG_FOCUS_IN = 0x4,
	ENGINE_FLAG_IS_PASSWORD = 0x8,
	ENGINE_FLAG_PROPERTIES_REGISTERED = 0x10
} EngineFlag;
#define ENGINE_TYPE_FLAG engine_flag_get_type()
GType engine_flag_get_type (void) G_GNUC_CONST;



extern MkdgPropertySpec propSpecs[];
extern const gchar *page_labels[];
extern const gchar *button_labels[];
extern GtkResponseType button_responses[];

#define cursor_current chewing_cursor_Current(self->icPreEdit->context)


/*
 * Type checking and casting macros
 */
#define IBUS_TYPE_CHEWING_ENGINE	(ibus_chewing_engine_get_type())
#define IBUS_CHEWING_ENGINE(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ibus_chewing_engine_get_type(), IBusChewingEngine)
#define IBUS_CHEWING_ENGINE_CONST(obj)	G_TYPE_CHECK_INSTANCE_CAST((obj), ibus_chewing_engine_get_type(), IBusChewingEngine const)
#define IBUS_CHEWING_ENGINE_CLASS(klass)	G_TYPE_CHECK_CLASS_CAST((klass), ibus_chewing_engine_get_type(), IBusChewingEngineClass)
#define IBUS_IS_CHEWING_ENGINE(obj)	G_TYPE_CHECK_INSTANCE_TYPE((obj), ibus_chewing_engine_get_type ())

#define IBUS_CHEWING_ENGINE_GET_CLASS(obj)	G_TYPE_INSTANCE_GET_CLASS((obj), ibus_chewing_engine_get_type(), IBusChewingEngineClass)

/* Private structure type */
typedef struct _IBusChewingEnginePrivate IBusChewingEnginePrivate;

/*
 * Main object structure
 */
#ifndef __TYPEDEF_IBUS_CHEWING_ENGINE__
#define __TYPEDEF_IBUS_CHEWING_ENGINE__
typedef struct _IBusChewingEngine IBusChewingEngine;
#endif
struct _IBusChewingEngine {
	IBusEngine __parent__;
	/*< public >*/
	IBusChewingPreEdit * icPreEdit;
	GtkWidget * sDialog;
	IBusText * preEditText;
	IBusText * auxText;
	IBusText * outgoingText;
	IBusProperty * InputMode;
	IBusProperty * AlnumSize;
	IBusProperty * setup_prop;
	IBusPropList * prop_list;
	/*< private >*/
	FILE * logFile; /* protected */
	IBusKeymap * keymap_us; /* protected */
};

/*
 * Class definition
 */
typedef struct _IBusChewingEngineClass IBusChewingEngineClass;
struct _IBusChewingEngineClass {
	IBusEngineClass __parent__;
	IBusText * InputMode_label_chi;
	IBusText * InputMode_label_eng;
	IBusText * InputMode_tooltip;
	IBusText * InputMode_symbol_chi;
	IBusText * InputMode_symbol_eng;
	IBusText * AlnumSize_label_full;
	IBusText * AlnumSize_label_half;
	IBusText * AlnumSize_tooltip;
	IBusText * AlnumSize_symbol_full;
	IBusText * AlnumSize_symbol_half;
	IBusText * setup_prop_label;
	IBusText * setup_prop_tooltip;
	IBusText * setup_prop_symbol;
	IBusText * emptyText;
};


/*
 * Public methods
 */
GType	ibus_chewing_engine_get_type	(void) G_GNUC_CONST;
void 	ibus_chewing_engine_refresh_property_list	(IBusChewingEngine * self);
void 	ibus_chewing_engine_hide_property_list	(IBusChewingEngine * self);


void ibus_chewing_engine_reset(IBusChewingEngine *self);
void ibus_chewing_engine_enable(IBusChewingEngine *self);
void ibus_chewing_engine_disable(IBusChewingEngine *self);
void ibus_chewing_engine_focus_in(IBusChewingEngine *self);
void ibus_chewing_engine_focus_out(IBusChewingEngine *self);
gboolean ibus_chewing_engine_process_key_event(IBusEngine *self,
                                               guint key_sym,
                                               guint keycode,
                                               guint modifiers);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
