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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
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
 * IBusChewingEngine uses the pre-edit and outgoing buffer to show to the end
 * user.
 */

#ifndef _IBUS_CHEWING_PRE_EDIT_H_
#define _IBUS_CHEWING_PRE_EDIT_H_
#include "IBusChewingLookupTable.h"
#include "IBusChewingUtil.h"
#include <chewing.h>
#include <glib.h>
#include <ibus.h>

#define UTF8_MAX_BYTES 4
#ifndef IBUS_CHEWING_MAX_WORD
#define IBUS_CHEWING_MAX_WORD 100
#endif
#define IBUS_CHEWING_MAX_BYTES UTF8_MAX_BYTES *IBUS_CHEWING_MAX_WORD

/**
 * IBusChewingPreEditFlag:
 * @FLAG_SYNC_FROM_IM: Sync the Chinese mode with input method
 * @FLAG_SYNC_FROM_KEYBOARD: Sync the Chinese mode with Caps Lock status
 * @FLAG_TABLE_SHOW: Lookup table is shown.
 */
typedef enum {
    FLAG_SYNC_FROM_IM = 1,
    FLAG_SYNC_FROM_KEYBOARD = 1 << 1,
    FLAG_TABLE_SHOW = 1 << 2,
} IBusChewingPreEditFlag;

/**
 * IBusChewingPreEdit:
 * @context:   chewing input context.
 * @preEdit:   String that are not ready to be committed to engine.
 * @outgoing:  String to be committed to engine. (Usually means completed
 * string).
 * @flags:     Misc flags.
 * @keyLast:   Last effective key.
 * @keyLastTs: Monotonic timestamp in microseconds of the last effective key press.
 * @bpmfLen:   Length of bopomofo chars in unicode characters.
 * @wordLen:   Length of preEdit in unicode characters.
 *
 * An IBusChewingPreEdit.
 */
typedef struct {
    // IBusChewingProperties *iProperties;
    /*< public > */
    ChewingContext *context;
    GString *preEdit;
    GString *outgoing;
    IBusKeymap *keymap;
    IBusLookupTable *iTable;
    IBusChewingPreEditFlag flags;
    KSym keyLast;
    gint64 keyLastTs;
    gint bpmfLen;
    gint wordLen;
    IBusEngine *engine;
} IBusChewingPreEdit;

IBusChewingPreEdit *ibus_chewing_pre_edit_new();

void ibus_chewing_pre_edit_free(IBusChewingPreEdit *self);

guint ibus_chewing_pre_edit_length(IBusChewingPreEdit *self);

guint ibus_chewing_pre_edit_word_length(IBusChewingPreEdit *self);

guint ibus_chewing_pre_edit_word_limit(IBusChewingPreEdit *self);

#define ibus_chewing_pre_edit_is_empty(self)                                   \
    (ibus_chewing_pre_edit_length(self) == 0)

#define ibus_chewing_pre_edit_is_full(self)                                    \
    (self->wordLen >= ibus_chewing_pre_edit_word_limit(self))

#define ibus_chewing_pre_edit_is_outgoing_empty(self) (self->outgoing->len == 0)

/**
 * ibus_chewing_pre_edit_get_pre_edit:
 * @self: An IBusChewingPreEdit.
 * @returns: Content of pre-edit buffer.
 *
 * Return the content of pre-edit buffer in UTF-8 encoded string format.
 * No need to free it.
 */
gchar *ibus_chewing_pre_edit_get_pre_edit(IBusChewingPreEdit *self);

/**
 * ibus_chewing_pre_edit_get_outgoing:
 * @self: An IBusChewingPreEdit.
 * @returns: Content of outgoing buffer.
 *
 * Return the content of outgoing buffer in UTF-8 encoded string format.
 * No need to free it.
 */
gchar *ibus_chewing_pre_edit_get_outgoing(IBusChewingPreEdit *self);

#define ibus_chewing_pre_edit_has_flag(self, f) mkdg_has_flag(self->flags, f)
#define ibus_chewing_pre_edit_set_flag(self, f) mkdg_set_flag(self->flags, f)
#define ibus_chewing_pre_edit_clear_flag(self, f)                              \
    mkdg_clear_flag(self->flags, f)

void ibus_chewing_pre_edit_force_commit(IBusChewingPreEdit *self);
void ibus_chewing_pre_edit_clear(IBusChewingPreEdit *self);
void ibus_chewing_pre_edit_clear_bopomofo(IBusChewingPreEdit *self);
void ibus_chewing_pre_edit_clear_pre_edit(IBusChewingPreEdit *self);
void ibus_chewing_pre_edit_clear_outgoing(IBusChewingPreEdit *self);

gboolean ibus_chewing_pre_edit_get_chi_eng_mode(IBusChewingPreEdit *self);
gboolean ibus_chewing_pre_edit_get_full_half_mode(IBusChewingPreEdit *self);

void ibus_chewing_pre_edit_set_chi_eng_mode(IBusChewingPreEdit *self,
                                            gboolean chineseMode);
void ibus_chewing_pre_edit_set_full_half_mode(IBusChewingPreEdit *self,
                                              gboolean fullShapeMode);

#define ibus_chewing_pre_edit_toggle_chi_eng_mode(self)                        \
    ibus_chewing_pre_edit_set_chi_eng_mode(                                    \
        self, !ibus_chewing_pre_edit_get_chi_eng_mode(self))
#define ibus_chewing_pre_edit_toggle_full_half_mode(self)                      \
    ibus_chewing_pre_edit_set_full_half_mode(                                  \
        self, !ibus_chewing_pre_edit_get_full_half_mode(self))

gboolean ibus_chewing_pre_edit_process_key(IBusChewingPreEdit *self, KSym kSym,
                                           KeyModifiers unmaskedMod);

/**
 * ibus_chewing_pre_edit_key_code_to_key_sym:
 *
 * Convert keycode to key_sym.
 */
KSym ibus_chewing_pre_edit_key_code_to_key_sym(IBusChewingPreEdit *self,
                                               KSym keySym, guint keyCode,
                                               KeyModifiers unmaskedMod);

/**
 * ibus_chewing_bopomofo_check(ChewingContext *context)
 * @returns: 1 if bopomofo buffer is non-empty; 0 if bopomofo buffer is empty.
 *
 */
#define ibus_chewing_bopomofo_check chewing_bopomofo_Check
gchar *ibus_chewing_pre_edit_get_bopomofo_string(IBusChewingPreEdit *self);

typedef enum {
    EVENT_RESPONSE_PROCESS = 0, /* Event process by IM */
    EVENT_RESPONSE_ABSORB, /* Event throw away by IM (e.g. Release event) */
    EVENT_RESPONSE_IGNORE, /* Event that should be passed to application, but
                              not process by IM */
    EVENT_RESPONSE_UNDECIDED,
} EventResponse;

typedef EventResponse (*KeyHandlingFunc)(IBusChewingPreEdit *self, KSym kSym,
                                         KeyModifiers unmaskedMod);

typedef struct {
    KSym kSymLower;
    KSym kSymUpper;
    KeyHandlingFunc keyFunc;
} KeyHandlingRule;

#endif /* _IBUS_CHEWING_PRE_EDIT_H_ */
