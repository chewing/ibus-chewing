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

/*
 * ibus_chewing_pre_edit private function definitions
 * Put it here so they can be tested.
 */
#ifndef _IBUS_CHEWING_PRE_EDIT_PRIVATE_H_
#    define _IBUS_CHEWING_PRE_EDIT_PRIVATE_H_

/*== Frequent used shortcut ==*/
#    define cursor_current chewing_cursor_Current(self->context)
#    define total_choice chewing_cand_TotalChoice(self->context)
#    define default_english_case_short \
        (STRING_EQUALS(ibus_chewing_pre_edit_get_property_string(self, "default-english-case"), "lowercase")) ? 'l' : \
        (STRING_EQUALS(ibus_chewing_pre_edit_get_property_string(self, "default-english-case"), "uppercase") ? 'u' : 'n')

/*== Conditional Expression Shortcut ==*/
#    define is_plain_zhuyin ibus_chewing_pre_edit_get_property_boolean(self, "plain-zhuyin")
#    define bpmf_check ibus_chewing_bopomofo_check(self->context)
#    define table_is_showing ibus_chewing_pre_edit_has_flag(self, FLAG_TABLE_SHOW)

#    define is_capslock (umaskedMod == IBUS_LOCK_MASK)
#    define is_shift_only (maskedMod == IBUS_SHIFT_MASK)
#    define is_shift (unmaskedMod & IBUS_SHIFT_MASK)
#    define is_ctrl_only (maskedMod == IBUS_CONTROL_MASK)
#    define buffer_is_empty (ibus_chewing_pre_edit_is_empty(self))


/*== Event processing ==*/
/* We only recognize the combination of shift, control and alt */
#    define modifiers_mask(unmaskedMod) unmaskedMod & (IBUS_SHIFT_MASK | IBUS_CONTROL_MASK | IBUS_MOD1_MASK)

/**
 * filter_modifier:
 * @allowed Allowed modifler.
 *
 * allowed=0 means only the keysym is allowed, no shift, alt, control are allowed.
 * allowed=IBUS_SHIFT_MASK means both keysym without modifier or shift are allowed.
 */
#    define filter_modifiers(allowed) KeyModifiers maskedMod = modifiers_mask(unmaskedMod); \
							if ( (maskedMod) & (~(allowed))){ return EVENT_RESPONSE_IGNORE; }
#    define absorb_when_release if (event_is_released(unmaskedMod)) { return EVENT_RESPONSE_ABSORB; }
#    define ignore_when_release if (event_is_released(unmaskedMod)) { return EVENT_RESPONSE_IGNORE; }
#    define ignore_when_buffer_is_empty if (buffer_is_empty) { return EVENT_RESPONSE_IGNORE; }
#    define ignore_when_buffer_is_empty_and_table_not_showing if (buffer_is_empty && !table_is_showing) { return EVENT_RESPONSE_IGNORE; }

#    define event_is_released(unmaskedMod) ((unmaskedMod & IBUS_RELEASE_MASK) !=0 )
#    define event_process_or_ignore(cond) (cond) ? EVENT_RESPONSE_PROCESS: EVENT_RESPONSE_IGNORE

#    define handle_log(funcName) IBUS_CHEWING_LOG(INFO, "* self_handle_%s(-,%x(%s),%x(%s))", funcName, kSym, key_sym_get_name(kSym), unmaskedMod, modifiers_to_string(unmaskedMod));

KSym self_key_sym_fix(IBusChewingPreEdit * self,
                      KSym kSym, KeyModifiers unmaskedMod);

EventResponse self_handle_key_sym_default(IBusChewingPreEdit * self,
                                          KSym kSym, KeyModifiers unmaskedMod);

void ibus_chewing_pre_edit_update(IBusChewingPreEdit * self);
#endif                          /* _IBUS_CHEWING_PRE_EDIT_PRIVATE_H_ */
