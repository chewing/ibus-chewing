/*
 * Copyright © 2014  Red Hat, Inc. All rights reserved.
 * Copyright © 2014  Ding-Yi Chen <dchen@redhat.com>
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
 * SECTION:IBusChewingPreEdit
 * @short_description: Pre-edit string buffer.
 * @title: IBusChewingPreEdit
 * @stability: Stable
 * @include: IbusChewingPreedit
 *
 * IBusChewingPreEdit is core of ibus-chewing.
 * It processes incoming key events and manage pre-edit and outgoing buffer.
 *
 * IBusChewingEngine uses the pre-edit and outgoing buffer to show to the end user.
 */

#ifndef _IBUS_CHEWING_PRE_EDIT_H_
#define _IBUS_CHEWING_PRE_EDIT_H_
#include <glib.h>
#include <chewing.h>
#include <ibus.h>
#include "MakerDialogBackend.h"
#include "IBusChewingUtil.h"
#include "IBusChewingProperties.h"

#define UTF8_MAX_BYTES 6
#ifndef IBUS_CHEWING_MAX_WORD
#define IBUS_CHEWING_MAX_WORD 100
#endif
#define IBUS_CHEWING_MAX_BYTES UTF8_MAX_BYTES * IBUS_CHEWING_MAX_WORD

typedef enum {
    FLAG_SYNC_FROM_IM = 1,
    FLAG_SYNC_FROM_KEYBOARD = 1<<1,
} IBusChewingPreEditFlag;

/**
 * IBusChewingPreEdit:
 * @context:   chewing input context.
 * @preEdit:   String that are not ready to be committed to engine.
 * @outgoing:  String to be committed to engine. (Usually means completed string).
 * @flags:     Misc flags.
 * @keyLast:   Last effective key.
 * @bpmfLen:   Length of bopomofo chars in unicode characters.
 * @wordLen:   Length of preEdit in unicode characters.
 *
 * An IBusChewingPreEdit.
 */
typedef struct {
    IBusChewingProperties *iProperties;
    /*< public > */
    ChewingContext *context;
    GString *preEdit;
    GString *outgoing;
    IBusKeymap *keymap;
    IBusChewingPreEditFlag flags;
    KSym keyLast;
    gint bpmfLen;
    gint wordLen;
} IBusChewingPreEdit;

IBusChewingPreEdit *ibus_chewing_pre_edit_new(MkdgBackend * backend);

void ibus_chewing_pre_edit_free(IBusChewingPreEdit * self);

#define ibus_chewing_pre_edit_get_property(self,propertyKey) mkdg_properties_find_by_key(self->iProperties->properties, propertyKey)

#define ibus_chewing_pre_edit_get_property_boolean(self,propertyKey) mkdg_properties_get_boolean_by_key(self->iProperties->properties, propertyKey)

#define ibus_chewing_pre_edit_get_property_int(self,propertyKey) mkdg_properties_get_int_by_key(self->iProperties->properties, propertyKey)

#define ibus_chewing_pre_edit_get_property_string(self,propertyKey) mkdg_properties_get_string_by_key(self->iProperties->properties, propertyKey)

#define ibus_chewing_pre_edit_set_property_boolean(self,propertyKey,boolValue) mkdg_properties_set_boolean_by_key(self->iProperties->properties, propertyKey, boolValue)

#define ibus_chewing_pre_edit_set_property_int(self,propertyKey,intValue) mkdg_properties_set_int_by_key(self->iProperties->properties, propertyKey, intValue)

#define ibus_chewing_pre_edit_set_property_string(self,propertyKey,strValue) mkdg_properties_set_string_by_key(self->iProperties->properties,propertyKey,strValue)

#define ibus_chewing_pre_edit_is_system_keyboard_layout(self) ibus_chewing_properties_read_boolean_general(self->iProperties, "ibus/general", "use-system-keyboard-layout", NULL)

#define ibus_chewing_pre_edit_is_embed_preedit_text(self) ibus_chewing_properties_read_boolean_general(self->iProperties, "ibus/general", "embed-preedit-text", NULL)

#define ibus_chewing_pre_edit_apply_property(self,propertyKey) mkdg_properties_apply_by_key(self->iProperties->properties, propertyKey, NULL)

void ibus_chewing_pre_edit_use_all_configure(IBusChewingPreEdit * self);

guint ibus_chewing_pre_edit_length(IBusChewingPreEdit * self);

guint ibus_chewing_pre_edit_word_length(IBusChewingPreEdit * self);

guint ibus_chewing_pre_edit_word_limit(IBusChewingPreEdit * self);

#define ibus_chewing_pre_edit_is_empty(self) (ibus_chewing_pre_edit_length(self) ==0)

#define ibus_chewing_pre_edit_is_full(self) (self->wordLen >= ibus_chewing_pre_edit_word_limit(self))

#define ibus_chewing_pre_edit_is_outgoing_empty(self) (self->outgoing->len==0)

/**
 * ibus_chewing_pre_edit_get_pre_edit:
 * @self: An IBusChewingPreEdit.
 * @returns: Content of pre-edit buffer.
 *
 * Return the content of pre-edit buffer in UTF-8 encoded string format.
 * No need to free it.
 */
gchar *ibus_chewing_pre_edit_get_pre_edit(IBusChewingPreEdit * self);

/**
 * ibus_chewing_pre_edit_get_outgoing:
 * @self: An IBusChewingPreEdit.
 * @returns: Content of outgoing buffer.
 *
 * Return the content of outgoing buffer in UTF-8 encoded string format.
 * No need to free it.
 */
gchar *ibus_chewing_pre_edit_get_outgoing(IBusChewingPreEdit * self);

#define ibus_chewing_pre_edit_set_flag(self,f) self->flags |= (f)
#define ibus_chewing_pre_edit_clear_flag(self,f) self->flags &= ~(f)
#define ibus_chewing_pre_edit_has_flag(self,f) (self->flags & f)

void ibus_chewing_pre_edit_force_commit(IBusChewingPreEdit * self);
void ibus_chewing_pre_edit_clear(IBusChewingPreEdit * self);
void ibus_chewing_pre_edit_clear_pre_edit(IBusChewingPreEdit * self);
void ibus_chewing_pre_edit_clear_outgoing(IBusChewingPreEdit * self);

gboolean ibus_chewing_pre_edit_process_key(IBusChewingPreEdit * self,
					   KSym kSym,
					   KeyModifiers unmaskedMod);


/**
 * ibus_chewing_pre_edit_key_code_to_key_sym:
 *
 * Convert keycode to key_sym.
 */
KSym ibus_chewing_pre_edit_key_code_to_key_sym(IBusChewingPreEdit * self,
					       KSym keySym, guint keyCode,
					       KeyModifiers unmaskedMod);

#if CHEWING_CHECK_VERSION(0,4,0)
#define ibus_chewing_bopomofo_check chewing_bopomofo_Check
#else
#define ibus_chewing_bopomofo_check chewing_zuin_Check
#endif
#define ibus_chewing_bopomofo_string chewing_zuin_String

#endif				/* _IBUS_CHEWING_PRE_EDIT_H_ */
