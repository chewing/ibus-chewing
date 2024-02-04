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

#define GOB_VERSION_MAJOR 2
#define GOB_VERSION_MINOR 0
#define GOB_VERSION_PATCHLEVEL 20

#define selfp (self->_priv)

#include <string.h> /* memset() */

#include "ibus-chewing-engine.h"

#include "ibus-chewing-engine-private.h"

#ifdef G_LIKELY
#define ___GOB_LIKELY(expr) G_LIKELY(expr)
#define ___GOB_UNLIKELY(expr) G_UNLIKELY(expr)
#else /* ! G_LIKELY */
#define ___GOB_LIKELY(expr) (expr)
#define ___GOB_UNLIKELY(expr) (expr)
#endif /* G_LIKELY */
static const GEnumValue _chewing_input_style_values[] = {
	{ CHEWING_INPUT_STYLE_IN_APPLICATION, (char *)"CHEWING_INPUT_STYLE_IN_APPLICATION", (char *)"in-application" },
	{ CHEWING_INPUT_STYLE_IN_CANDIDATE, (char *)"CHEWING_INPUT_STYLE_IN_CANDIDATE", (char *)"in-candidate" },
	{ 0, NULL, NULL }
};

GType
chewing_input_style_get_type (void)
{
	static GType type = 0;
	if ___GOB_UNLIKELY(type == 0)
		type = g_enum_register_static ("ChewingInputStyle", _chewing_input_style_values);
	return type;
}

static const GEnumValue _engine_flag_values[] = {
	{ ENGINE_FLAG_INITIALIZED, (char *)"ENGINE_FLAG_INITIALIZED", (char *)"initialized" },
	{ ENGINE_FLAG_ENABLED, (char *)"ENGINE_FLAG_ENABLED", (char *)"enabled" },
	{ ENGINE_FLAG_FOCUS_IN, (char *)"ENGINE_FLAG_FOCUS_IN", (char *)"focus-in" },
	{ ENGINE_FLAG_IS_PASSWORD, (char *)"ENGINE_FLAG_IS_PASSWORD", (char *)"is-password" },
	{ ENGINE_FLAG_PROPERTIES_REGISTERED, (char *)"ENGINE_FLAG_PROPERTIES_REGISTERED", (char *)"properties-registered" },
	{ 0, NULL, NULL }
};

GType
engine_flag_get_type (void)
{
	static GType type = 0;
	if ___GOB_UNLIKELY(type == 0)
		type = g_enum_register_static ("EngineFlag", _engine_flag_values);
	return type;
}



extern gint ibus_chewing_verbose;

#define IBUS_CHEWING_MAIN
#include "IBusConfigBackend.h"

/* self casting macros */
#define SELF(x) IBUS_CHEWING_ENGINE(x)
#define SELF_CONST(x) IBUS_CHEWING_ENGINE_CONST(x)
#define IS_SELF(x) IBUS_IS_CHEWING_ENGINE(x)
#define TYPE_SELF IBUS_TYPE_CHEWING_ENGINE
#define SELF_CLASS(x) IBUS_CHEWING_ENGINE_CLASS(x)

#define SELF_GET_CLASS(x) IBUS_CHEWING_ENGINE_GET_CLASS(x)

/* self typedefs */
typedef IBusChewingEngine Self;
typedef IBusChewingEngineClass SelfClass;

/* here are local prototypes */
static void ibus_chewing_engine_init (IBusChewingEngine * self) G_GNUC_UNUSED;
static void ibus_chewing_engine_class_init (IBusChewingEngineClass * klass) G_GNUC_UNUSED;
static void ibus_chewing_engine_constructor (IBusChewingEngine * self) G_GNUC_UNUSED;
static IBusProperty * ibus_chewing_engine_get_ibus_property_by_name (IBusChewingEngine * self, const gchar * prop_name) G_GNUC_UNUSED;
static void ___b_ibus_chewing_engine_reset (IBusEngine * engine) G_GNUC_UNUSED;
static void ___c_ibus_chewing_engine_page_up (IBusEngine * engine) G_GNUC_UNUSED;
static void ___d_ibus_chewing_engine_page_down (IBusEngine * engine) G_GNUC_UNUSED;
static void ___e_ibus_chewing_engine_cursor_up (IBusEngine * engine) G_GNUC_UNUSED;
static void ___f_ibus_chewing_engine_cursor_down (IBusEngine * engine) G_GNUC_UNUSED;
static void ___10_ibus_chewing_engine_enable (IBusEngine * engine) G_GNUC_UNUSED;
static void ___11_ibus_chewing_engine_disable (IBusEngine * engine) G_GNUC_UNUSED;
static void ___12_ibus_chewing_engine_focus_in (IBusEngine * engine) G_GNUC_UNUSED;
static void ___13_ibus_chewing_engine_focus_out (IBusEngine * engine) G_GNUC_UNUSED;
static void ___14_ibus_chewing_engine_set_capabilities (IBusEngine * engine, guint caps) G_GNUC_UNUSED;
static void ___15_ibus_chewing_engine_property_show (IBusEngine * engine, const gchar * prop_name) G_GNUC_UNUSED;
static void ___16_ibus_chewing_engine_property_hide (IBusEngine * engine, const gchar * prop_name) G_GNUC_UNUSED;

/* pointer to the class of our parent */
static IBusEngineClass *parent_class = NULL;

/* Short form macros */
#define self_use_setting ibus_chewing_engine_use_setting
#define self_restore_mode ibus_chewing_engine_restore_mode
#define self_update ibus_chewing_engine_update
#define self_refresh_property ibus_chewing_engine_refresh_property
#define self_refresh_property_list ibus_chewing_engine_refresh_property_list
#define self_hide_property_list ibus_chewing_engine_hide_property_list
#define self_get_ibus_property_by_name ibus_chewing_engine_get_ibus_property_by_name
GType
ibus_chewing_engine_get_type (void)
{
	static GType type = 0;

	if ___GOB_UNLIKELY(type == 0) {
		static const GTypeInfo info = {
			sizeof (IBusChewingEngineClass),
			(GBaseInitFunc) NULL,
			(GBaseFinalizeFunc) NULL,
			(GClassInitFunc) ibus_chewing_engine_class_init,
			(GClassFinalizeFunc) NULL,
			NULL /* class_data */,
			sizeof (IBusChewingEngine),
			0 /* n_preallocs */,
			(GInstanceInitFunc) ibus_chewing_engine_init,
			NULL
		};

		type = g_type_register_static (IBUS_TYPE_ENGINE, "IBusChewingEngine", &info, (GTypeFlags)0);
	}

	return type;
}

/* a macro for creating a new object of our type */
#define GET_NEW ((IBusChewingEngine *)g_object_new(ibus_chewing_engine_get_type(), NULL))

/* a function for creating a new object of our type */
#include <stdarg.h>
static IBusChewingEngine * GET_NEW_VARG (const char *first, ...) G_GNUC_UNUSED;
static IBusChewingEngine *
GET_NEW_VARG (const char *first, ...)
{
	IBusChewingEngine *ret;
	va_list ap;
	va_start (ap, first);
	ret = (IBusChewingEngine *)g_object_new_valist (ibus_chewing_engine_get_type (), first, ap);
	va_end (ap);
	return ret;
}


static GObject *
___constructor (GType type, guint n_construct_properties, GObjectConstructParam *construct_properties)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::constructor"
	GObject *obj_self;
	IBusChewingEngine *self;
	obj_self = G_OBJECT_CLASS (parent_class)->constructor (type, n_construct_properties, construct_properties);
	self = IBUS_CHEWING_ENGINE (obj_self);
	ibus_chewing_engine_constructor (self);
	return obj_self;
}
#undef __GOB_FUNCTION__


static void
___finalize(GObject *obj_self)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::finalize"
	IBusChewingEngine *self G_GNUC_UNUSED = IBUS_CHEWING_ENGINE (obj_self);
	gpointer priv G_GNUC_UNUSED = self->_priv;
	if(self->icPreEdit) { ibus_chewing_pre_edit_free ((gpointer) self->icPreEdit); self->icPreEdit = NULL; }
	if(self->preEditText) { g_object_unref ((gpointer) self->preEditText); self->preEditText = NULL; }
	if(self->auxText) { g_object_unref ((gpointer) self->auxText); self->auxText = NULL; }
	if(self->outgoingText) { g_object_unref ((gpointer) self->outgoingText); self->outgoingText = NULL; }
	if(self->InputMode) { g_object_unref ((gpointer) self->InputMode); self->InputMode = NULL; }
	if(self->AlnumSize) { g_object_unref ((gpointer) self->AlnumSize); self->AlnumSize = NULL; }
	if(self->setup_prop) { g_object_unref ((gpointer) self->setup_prop); self->setup_prop = NULL; }
	if(self->prop_list) { g_object_unref ((gpointer) self->prop_list); self->prop_list = NULL; }
	if(self->_priv->pDisplay) { XCloseDisplay ((gpointer) self->_priv->pDisplay); self->_priv->pDisplay = NULL; }
	if(G_OBJECT_CLASS(parent_class)->finalize) \
		(* G_OBJECT_CLASS(parent_class)->finalize)(obj_self);
}
#undef __GOB_FUNCTION__

static void 
ibus_chewing_engine_init (IBusChewingEngine * self G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::init"
	self->_priv = G_TYPE_INSTANCE_GET_PRIVATE(self,IBUS_TYPE_CHEWING_ENGINE,IBusChewingEnginePrivate);
	self->icPreEdit = NULL;
	self->sDialog = NULL;
	self->preEditText = NULL;
	self->auxText = NULL;
	self->outgoingText = NULL;
	self->logFile = NULL;
	self->_priv->statusFlags = 0;
	self->_priv->capabilite = 0;
	self->InputMode = 
        g_object_ref_sink(ibus_property_new
            ("InputMode",
             PROP_TYPE_NORMAL,
             SELF_GET_CLASS(self)->InputMode_label_chi,
             NULL,
             SELF_GET_CLASS(self)->InputMode_tooltip,
             TRUE,
             TRUE,
             PROP_STATE_UNCHECKED,
             NULL))
    ;
	self->AlnumSize = 
        g_object_ref_sink(ibus_property_new
            ("AlnumSize",
             PROP_TYPE_NORMAL,
             SELF_GET_CLASS(self)->AlnumSize_label_half,
             NULL,
             SELF_GET_CLASS(self)->AlnumSize_tooltip,
             TRUE,
             TRUE,
             PROP_STATE_UNCHECKED,
             NULL))
    ;
	self->setup_prop = 
        g_object_ref_sink(ibus_property_new
            ("setup_prop",
             PROP_TYPE_NORMAL,
             SELF_GET_CLASS(self)->setup_prop_label,
             NULL,
             SELF_GET_CLASS(self)->setup_prop_tooltip,
             TRUE,
             TRUE,
             PROP_STATE_UNCHECKED,
             NULL))
    ;
	self->prop_list = 
        g_object_ref_sink(ibus_prop_list_new())
    ;
	self->keymap_us =  ibus_keymap_get("us") ;
	self->_priv->pDisplay = 
        XOpenDisplay(NULL)
    ;
 {

        /* initialize the object here */
        IBUS_CHEWING_LOG(INFO, "init() %sinitialized",
            (self->_priv->statusFlags & ENGINE_FLAG_INITIALIZED) ? "" : "un");
        if (self->_priv->statusFlags & ENGINE_FLAG_INITIALIZED) {
            return;
        }

        IBUS_CHEWING_LOG(INFO, "init() CHEWING_DATADIR_REAL=%s",
                         QUOTE_ME(CHEWING_DATADIR_REAL));

        gchar *logFilename = getenv("IBUS_CHEWING_LOGFILE");

        if (logFilename != NULL) {
            self->logFile = fopen(logFilename, "w+");
            if (self->logFile == NULL) {
                IBUS_CHEWING_LOG(WARN, "init() Cannot write to logfile %s, ignored",
                                 logFilename);
            } else {
                mkdg_log_set_file(self->logFile);
            }
        }

        if (self->icPreEdit == NULL) {

            MkdgBackend *backend =
                mkdg_g_settings_backend_new(QUOTE_ME(PROJECT_SCHEMA_ID),
                QUOTE_ME(PROJECT_SCHEMA_DIR), NULL);

            self->icPreEdit = ibus_chewing_pre_edit_new(backend);
        }

        g_assert(self->icPreEdit);

        self->icPreEdit->engine = IBUS_ENGINE(self);

        /* init properties */
        ibus_prop_list_append(self->prop_list, self->InputMode);
        ibus_prop_list_append(self->prop_list, self->AlnumSize);
        ibus_prop_list_append(self->prop_list, self->setup_prop);

        ibus_chewing_engine_set_status_flag(self, ENGINE_FLAG_INITIALIZED);

        /* In case we cannot open X display */
        if (self->_priv->pDisplay == NULL) {
            IBUS_CHEWING_LOG(WARN, "init() XOpenDisplay return NULL");
        }

        IBUS_CHEWING_LOG(DEBUG, "init() Done");
    
 }
}
#undef __GOB_FUNCTION__
static void 
ibus_chewing_engine_class_init (IBusChewingEngineClass * klass G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::class_init"
	GObjectClass *g_object_class G_GNUC_UNUSED = (GObjectClass*) klass;
	IBusEngineClass *ibus_engine_class = (IBusEngineClass *)klass;

	g_type_class_add_private(klass,sizeof(IBusChewingEnginePrivate));
	klass->InputMode_label_chi = 
        g_object_ref_sink(ibus_text_new_from_static_string(_("Chinese Mode")))
    ;
	klass->InputMode_label_eng = 
        g_object_ref_sink(ibus_text_new_from_static_string(_("Alphanumeric Mode")))
    ;
	klass->InputMode_tooltip = 
        g_object_ref_sink(ibus_text_new_from_static_string
                          (_("Click to toggle Chinese/Alphanumeric Mode")))
    ;
	klass->InputMode_symbol_chi = 
        g_object_ref_sink(ibus_text_new_from_static_string("中"))
    ;
	klass->InputMode_symbol_eng = 
        g_object_ref_sink(ibus_text_new_from_static_string("英"))
    ;
	klass->AlnumSize_label_full = 
        g_object_ref_sink(ibus_text_new_from_static_string(_("Fullwidth Form")))
    ;
	klass->AlnumSize_label_half = 
        g_object_ref_sink(ibus_text_new_from_static_string(_("Halfwidth Form")))
    ;
	klass->AlnumSize_tooltip = 
        g_object_ref_sink(ibus_text_new_from_static_string
                          (_("Click to toggle Halfwidth/Fullwidth Form")))
    ;
	klass->AlnumSize_symbol_full = 
        g_object_ref_sink(ibus_text_new_from_static_string("全"))
    ;
	klass->AlnumSize_symbol_half = 
        g_object_ref_sink(ibus_text_new_from_static_string("半"))
    ;
	klass->setup_prop_label = 
        g_object_ref_sink(ibus_text_new_from_static_string(_("IBus-Chewing Preferences")))
    ;
	klass->setup_prop_tooltip = 
        g_object_ref_sink(ibus_text_new_from_static_string
                          (_("Click to configure IBus-Chewing")))
    ;
	klass->setup_prop_symbol = 
        g_object_ref_sink(ibus_text_new_from_static_string("訂"))
    ;
	klass->emptyText = 
        g_object_ref_sink(ibus_text_new_from_static_string(""))
    ;

	parent_class = g_type_class_ref (IBUS_TYPE_ENGINE);

	ibus_engine_class->reset = ___b_ibus_chewing_engine_reset;
	ibus_engine_class->page_up = ___c_ibus_chewing_engine_page_up;
	ibus_engine_class->page_down = ___d_ibus_chewing_engine_page_down;
	ibus_engine_class->cursor_up = ___e_ibus_chewing_engine_cursor_up;
	ibus_engine_class->cursor_down = ___f_ibus_chewing_engine_cursor_down;
	ibus_engine_class->enable = ___10_ibus_chewing_engine_enable;
	ibus_engine_class->disable = ___11_ibus_chewing_engine_disable;
	ibus_engine_class->focus_in = ___12_ibus_chewing_engine_focus_in;
	ibus_engine_class->focus_out = ___13_ibus_chewing_engine_focus_out;
	ibus_engine_class->set_capabilities = ___14_ibus_chewing_engine_set_capabilities;
	ibus_engine_class->property_show = ___15_ibus_chewing_engine_property_show;
	ibus_engine_class->property_hide = ___16_ibus_chewing_engine_property_hide;
	g_object_class->constructor = ___constructor;
	g_object_class->finalize = ___finalize;
 {

        ibus_engine_class->property_activate =
            ibus_chewing_engine_property_activate;
        ibus_engine_class->process_key_event =
            ibus_chewing_engine_process_key_event;
        ibus_engine_class->candidate_clicked =
            ibus_chewing_engine_candidate_clicked;
#if IBUS_CHECK_VERSION(1, 5, 4)
        ibus_engine_class->set_content_type =
            ibus_chewing_engine_set_content_type;
#endif
    
 }
}
#undef __GOB_FUNCTION__



static void 
ibus_chewing_engine_constructor (IBusChewingEngine * self G_GNUC_UNUSED)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::constructor"
{
	
        /* GOB need this for generating constructor */
    }}
#undef __GOB_FUNCTION__

void 
ibus_chewing_engine_use_setting (IBusChewingEngine * self)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::use_setting"
	g_return_if_fail (self != NULL);
	g_return_if_fail (IBUS_IS_CHEWING_ENGINE (self));
{
	
        IBUS_CHEWING_LOG(INFO, "use_setting()");

        ibus_chewing_pre_edit_use_all_configure(self->icPreEdit);

        /* Input style */
        if (ibus_chewing_properties_read_boolean_general(self->icPreEdit->iProperties,
            "ibus/general", "embed-preedit-text", NULL)) {
            self->_priv->inputStyle = CHEWING_INPUT_STYLE_IN_APPLICATION;
        } else {
            self->_priv->inputStyle = CHEWING_INPUT_STYLE_IN_CANDIDATE;
        }
    }}
#undef __GOB_FUNCTION__

void 
ibus_chewing_engine_restore_mode (IBusChewingEngine * self)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::restore_mode"
	g_return_if_fail (self != NULL);
	g_return_if_fail (IBUS_IS_CHEWING_ENGINE (self));
{
	
        IBUS_CHEWING_LOG(DEBUG, "restore_mode() statusFlags=%x", self->_priv->statusFlags);
        if (self->_priv->pDisplay != NULL) {
            /* Restore Led Mode only make sense if pDisplay is available */
            if (ibus_chewing_pre_edit_has_flag(self->icPreEdit, FLAG_SYNC_FROM_IM)) {
                IBUS_CHEWING_LOG(DEBUG, "restore_mode() FLAG_SYNC_FROM_IM");
                if (ibus_chewing_engine_is_chinese_mode(self) == is_caps_lock(self)) {
                    /* ChiEng mode does not agree each other */
                    set_caps_led(!ibus_chewing_engine_is_chinese_mode(self), self->_priv->pDisplay);
                }
            } else if (ibus_chewing_pre_edit_has_flag(self->icPreEdit, FLAG_SYNC_FROM_KEYBOARD)) {
                IBUS_CHEWING_LOG(DEBUG, "restore_mode() FLAG_SYNC_FROM_KEYBOARD");
                chewing_set_ChiEngMode(self->icPreEdit->context, (is_caps_lock(self)) ? 0 : CHINESE_MODE);
            }
            self_refresh_property(self, "InputMode");
        }
    }}
#undef __GOB_FUNCTION__

void 
ibus_chewing_engine_update (IBusChewingEngine * self)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::update"
	g_return_if_fail (self != NULL);
	g_return_if_fail (IBUS_IS_CHEWING_ENGINE (self));
{
	
        IBUS_CHEWING_LOG(DEBUG, "update() statusFlags=%x", self->_priv->statusFlags);
        commit_text(self);
        update_pre_edit_text(self);
        update_aux_text(self);

        IBUS_CHEWING_LOG(DEBUG,
                         "update() nPhoneSeq=%d statusFlags=%x",
                         chewing_get_phoneSeqLen(self->icPreEdit->context),
                         self->_priv->statusFlags);
        update_lookup_table(self);
    }}
#undef __GOB_FUNCTION__

void 
ibus_chewing_engine_refresh_property (IBusChewingEngine * self, const gchar * prop_name)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::refresh_property"
	g_return_if_fail (self != NULL);
	g_return_if_fail (IBUS_IS_CHEWING_ENGINE (self));
{
	
#ifndef UNIT_TEST
        IBUS_CHEWING_LOG(DEBUG, "refresh_property(%s) status=%x",
                         prop_name, self->_priv->statusFlags);

        if (STRING_EQUALS(prop_name, "InputMode")) {

            ibus_property_set_label(self->InputMode,
                ibus_chewing_pre_edit_get_chi_eng_mode(self->icPreEdit) ?
                SELF_GET_CLASS(self)->InputMode_label_chi :
                SELF_GET_CLASS(self)->InputMode_label_eng);

#if IBUS_CHECK_VERSION(1, 5, 0)
            ibus_property_set_symbol(self->InputMode,
                ibus_chewing_pre_edit_get_chi_eng_mode(self->icPreEdit) ?
                SELF_GET_CLASS(self)->InputMode_symbol_chi :
                SELF_GET_CLASS(self)->InputMode_symbol_eng);
#endif

            ibus_engine_update_property(IBUS_ENGINE(self), self->InputMode);

        } else if (STRING_EQUALS(prop_name, "AlnumSize")) {

            ibus_property_set_label(self->AlnumSize,
                chewing_get_ShapeMode(self->icPreEdit->context) ?
                SELF_GET_CLASS(self)->AlnumSize_label_full :
                SELF_GET_CLASS(self)->AlnumSize_label_half);

#if IBUS_CHECK_VERSION(1, 5, 0)
            ibus_property_set_symbol(self->AlnumSize,
                chewing_get_ShapeMode(self->icPreEdit->context) ?
                SELF_GET_CLASS(self)->AlnumSize_symbol_full :
                SELF_GET_CLASS(self)->AlnumSize_symbol_half);
#endif

            if (self->_priv->statusFlags & ENGINE_FLAG_PROPERTIES_REGISTERED)
                ibus_engine_update_property(IBUS_ENGINE(self), self->AlnumSize);

        } else if (STRING_EQUALS(prop_name, "setup_prop")) {
#if IBUS_CHECK_VERSION(1, 5, 0)
            ibus_property_set_symbol(self->setup_prop,
                                      SELF_GET_CLASS(self)->setup_prop_symbol);
#endif
            ibus_engine_update_property(IBUS_ENGINE(self), self->setup_prop);
        }
#endif
    }}
#undef __GOB_FUNCTION__

/**
 * ibus_chewing_engine_refresh_property_list:
 * @self: this instances.
 *
 * Refresh the property list (language bar).
 **/
void 
ibus_chewing_engine_refresh_property_list (IBusChewingEngine * self)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::refresh_property_list"
	g_return_if_fail (self != NULL);
	g_return_if_fail (IBUS_IS_CHEWING_ENGINE (self));
{
	
#ifndef UNIT_TEST
        self_refresh_property(self, "InputMode");
        self_refresh_property(self, "AlnumSize");
        self_refresh_property(self, "setup_prop");
#endif
    }}
#undef __GOB_FUNCTION__

/**
 * ibus_chewing_engine_hide_property_list:
 * @self: this instances.
 *
 * Hide the property list (language bar).
 **/
void 
ibus_chewing_engine_hide_property_list (IBusChewingEngine * self)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::hide_property_list"
	g_return_if_fail (self != NULL);
	g_return_if_fail (IBUS_IS_CHEWING_ENGINE (self));
{
	
#ifndef UNIT_TEST
        IBUS_ENGINE_GET_CLASS(self)->property_hide(IBUS_ENGINE(self),
                                                   "AlnumSize");
#endif
    }}
#undef __GOB_FUNCTION__

static IBusProperty * 
ibus_chewing_engine_get_ibus_property_by_name (IBusChewingEngine * self, const gchar * prop_name)
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::get_ibus_property_by_name"
	g_return_val_if_fail (self != NULL, (IBusProperty * )0);
	g_return_val_if_fail (IBUS_IS_CHEWING_ENGINE (self), (IBusProperty * )0);
{
	
        if (STRING_EQUALS(prop_name, "InputMode")) {
            return self->InputMode;
        } else if (STRING_EQUALS(prop_name, "AlnumSize")) {
            return self->AlnumSize;
        } else if (STRING_EQUALS(prop_name, "setup_prop")) {
            return self->setup_prop;
        }
        IBUS_CHEWING_LOG(MSG, "get_ibus_property_by_name(%s): NULL is returned", prop_name);
        return NULL;
    }}
#undef __GOB_FUNCTION__

static void 
___b_ibus_chewing_engine_reset (IBusEngine * engine G_GNUC_UNUSED)
#define PARENT_HANDLER(___engine) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->reset) \
		(* IBUS_ENGINE_CLASS(parent_class)->reset)(___engine); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::reset"
{
	
        Self *self = SELF(engine);
        ibus_chewing_engine_reset(self);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___c_ibus_chewing_engine_page_up (IBusEngine * engine G_GNUC_UNUSED)
#define PARENT_HANDLER(___engine) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->page_up) \
		(* IBUS_ENGINE_CLASS(parent_class)->page_up)(___engine); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::page_up"
{
	
        Self *self = SELF(engine);
        if (is_password(self))
            return;
        ibus_chewing_pre_edit_process_key(self->icPreEdit, IBUS_KEY_Page_Up, 0);
        self_update(self);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___d_ibus_chewing_engine_page_down (IBusEngine * engine G_GNUC_UNUSED)
#define PARENT_HANDLER(___engine) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->page_down) \
		(* IBUS_ENGINE_CLASS(parent_class)->page_down)(___engine); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::page_down"
{
	
        Self *self = SELF(engine);
        if (is_password(self))
            return;
        ibus_chewing_pre_edit_process_key(self->icPreEdit, IBUS_KEY_Page_Down, 0);
        self_update(self);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___e_ibus_chewing_engine_cursor_up (IBusEngine * engine G_GNUC_UNUSED)
#define PARENT_HANDLER(___engine) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->cursor_up) \
		(* IBUS_ENGINE_CLASS(parent_class)->cursor_up)(___engine); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::cursor_up"
{
	
        Self *self = SELF(engine);
        if (is_password(self))
            return;
        ibus_chewing_pre_edit_process_key(self->icPreEdit, IBUS_KEY_Up, 0);
        self_update(self);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___f_ibus_chewing_engine_cursor_down (IBusEngine * engine G_GNUC_UNUSED)
#define PARENT_HANDLER(___engine) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->cursor_down) \
		(* IBUS_ENGINE_CLASS(parent_class)->cursor_down)(___engine); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::cursor_down"
{
	
        Self *self = SELF(engine);
        if (is_password(self))
            return;
        ibus_chewing_pre_edit_process_key(self->icPreEdit, IBUS_KEY_Down, 0);
        self_update(self);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___10_ibus_chewing_engine_enable (IBusEngine * engine G_GNUC_UNUSED)
#define PARENT_HANDLER(___engine) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->enable) \
		(* IBUS_ENGINE_CLASS(parent_class)->enable)(___engine); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::enable"
{
	
        Self *self = SELF(engine);
        ibus_chewing_engine_enable(self);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___11_ibus_chewing_engine_disable (IBusEngine * engine G_GNUC_UNUSED)
#define PARENT_HANDLER(___engine) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->disable) \
		(* IBUS_ENGINE_CLASS(parent_class)->disable)(___engine); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::disable"
{
	
        Self *self = SELF(engine);
        ibus_chewing_engine_disable(self);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___12_ibus_chewing_engine_focus_in (IBusEngine * engine G_GNUC_UNUSED)
#define PARENT_HANDLER(___engine) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->focus_in) \
		(* IBUS_ENGINE_CLASS(parent_class)->focus_in)(___engine); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::focus_in"
{
	
        Self *self = SELF(engine);
        ibus_chewing_engine_focus_in(self);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___13_ibus_chewing_engine_focus_out (IBusEngine * engine G_GNUC_UNUSED)
#define PARENT_HANDLER(___engine) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->focus_out) \
		(* IBUS_ENGINE_CLASS(parent_class)->focus_out)(___engine); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::focus_out"
{
	
        Self *self = SELF(engine);
        ibus_chewing_engine_focus_out(self);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___14_ibus_chewing_engine_set_capabilities (IBusEngine * engine G_GNUC_UNUSED, guint caps)
#define PARENT_HANDLER(___engine,___caps) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->set_capabilities) \
		(* IBUS_ENGINE_CLASS(parent_class)->set_capabilities)(___engine,___caps); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::set_capabilities"
{
	
        Self *self = SELF(engine);
        self->_priv->capabilite = caps;
        IBUS_CHEWING_LOG(MSG, "***** set_capabilities(%x): statusFlags=%x",
                         caps, self->_priv->statusFlags);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___15_ibus_chewing_engine_property_show (IBusEngine * engine G_GNUC_UNUSED, const gchar * prop_name)
#define PARENT_HANDLER(___engine,___prop_name) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->property_show) \
		(* IBUS_ENGINE_CLASS(parent_class)->property_show)(___engine,___prop_name); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::property_show"
{
	
        IBUS_CHEWING_LOG(INFO, "property_show(-, %s)", prop_name);
        Self *self = SELF(engine);
        IBusProperty *prop = self_get_ibus_property_by_name(self, prop_name);
        ibus_property_set_visible(prop, TRUE);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER

static void 
___16_ibus_chewing_engine_property_hide (IBusEngine * engine G_GNUC_UNUSED, const gchar * prop_name)
#define PARENT_HANDLER(___engine,___prop_name) \
	{ if(IBUS_ENGINE_CLASS(parent_class)->property_hide) \
		(* IBUS_ENGINE_CLASS(parent_class)->property_hide)(___engine,___prop_name); }
{
#define __GOB_FUNCTION__ "IBus:Chewing:Engine::property_hide"
{
	
        IBUS_CHEWING_LOG(INFO, "property_hide(-, %s)", prop_name);
        Self *self = SELF(engine);
        IBusProperty *prop = self_get_ibus_property_by_name(self, prop_name);
        ibus_property_set_visible(prop, FALSE);
    }}
#undef __GOB_FUNCTION__
#undef PARENT_HANDLER
