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
 * SECTION:IBusChewingBuffer
 * @short_description: Pre-edit string buffer.
 * @title: IBusChewingBuffer
 * @stability: Stable
 * @include: IbusChewingBuffer
 *
 * IBusChewingBuffer holds a pre-edit string buffer, which stores incompleted
 * string.
 */

#ifndef _IBUS_CHEWING_BUFFER_H_
#define _IBUS_CHEWING_BUFFER_H_
#include <glib.h>
#include <chewing.h>

#define UTF8_MAX_BYTES 6
#ifndef IBUS_CHEWING_MAX_WORD
#define IBUS_CHEWING_MAX_WORD 100
#endif
#define IBUS_CHEWING_MAX_BYTES UTF8_MAX_BYTES * IBUS_CHEWING_MAX_WORD

/**
 * IBusChewingBuffer:
 * @buffer:  Buffer storage. Not meant to be use directly from outside.
 * @wordLen: Length of words.
 *
 * An IBusChewingBuffer.
 */
typedef struct{
    IBusChewingProperties *iProperties;
    /*< public >*/
    ChewingContext *context;
    GString *buffer;
    guint wordLen;
} IBusChewingBuffer;

IBusChewingBuffer *ibus_chewing_buffer_new(IBusChewingProperties *
					   iProperties);
void ibus_chewing_buffer_free(IBusChewingBuffer * self);

guint ibus_chewing_buffer_length(IBusChewingBuffer * self);

guint ibus_chewing_buffer_word_length(IBusChewingBuffer * self);

guint ibus_chewing_buffer_word_limit(IBusChewingBuffer * self);

#define ibus_chewing_buffer_is_empty(self) (ibus_chewing_buffer_length(self) ==0)

#define ibus_chewing_buffer_is_full(self) (self->wordLen >= ibus_chewing_buffer_word_limit(self))

/**
 * ibus_chewing_buffer_get_string:
 * @self: An IBusChewingBuffer.
 * @returns: Get the buffer content in string format.
 *
 * Get the pre-edit buffer content in conventional string format.
 * No need to free it.
 */
gchar *ibus_chewing_buffer_get_string(IBusChewingBuffer * self);

void ibus_chewing_buffer_clear(IBusChewingBuffer * self);

void ibus_chewing_buffer_add_word(IBusChewingBuffer *self, gunichar word);

#endif /* _IBUS_CHEWING_BUFFER_H_ */

