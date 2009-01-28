# vim:set et sts=4 sw=4:
# -*- coding: utf-8 -*-
#
# ibus-chewing - The Chewing engine for IBus
#
# Copyright (c) 2007-2008 Huang Peng <shawn.p.huang@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

import gobject
import ibus
from ibus import keysyms, modifier

import sys
from ctypes import *
from libchewing import chewing

chewing._libchewing.chewing_cand_String.restype = c_char_p
chewing._libchewing.chewing_zuin_String.restype = c_char_p
chewing._libchewing.chewing_aux_String.restype = c_char_p
_ = lambda a: a

#definitions
IS_USER_PHRASE=1
IS_DICT_PHRASE=0
CHINESE_MODE=1
SYMBOL_MODE=0
FULLSHAPE_MODE=1
HALFSHAPE_MODE=0

DEFAULT_KBTYPE=0
HSU_KBTYPE=1
IBM_KBTYPE=2
GINYIEH_KBTYPE=3
ETEN_KBTYPE=4
ETEN26_KBTYPE=5
DVORAK_KBTYPE=6
DVORAKHSU_KBTYPE=7
HANYU_KBTYPE=8
FIRST_KBTYPE=DEFAULT_KBTYPE
LAST_KBTYPE=HANYU_KBTYPE



class Engine(ibus.EngineBase):
    def __init__(self, bus, object_path):
        super(Engine, self).__init__(bus, object_path)

        # create chewing context
        chewing.Init('/usr/share/chewing', '/tmp')
        self.__context = chewing.ChewingContext()
        self.__context.Configure (18, 16, 0, 1, 0);
        MultiIntegers = c_int * 10
        selKeys=MultiIntegers(49, 50, 51, 52, 53, 54, 55, 56, 57, 48)
        self.__context.set_selKey(selKeys,10)
        self.__context.set_ChiEngMode(1)


        self.__lookup_table = ibus.LookupTable(8)
        self.__lookup_table.show_cursor(False)

        # init properties
        self.__chieng_property = ibus.Property(u"chieng")
        self.__letter_property = ibus.Property(u"letter")
        self.__kbtype_property = ibus.Property(u"kbtype")
        self.__prop_list = ibus.PropList()
        self.__prop_list.append(self.__chieng_property)
        self.__prop_list.append(self.__letter_property)
        self.__prop_list.append(self.__kbtype_property)

        # use reset to init values
        self.__reset()

    def __commit(self):
        # commit string
        if self.__context.commit_Check():
            text = u"".join(unicode(self.__context.commit_String(),'utf-8'))
            self.commit_string(text)

        # update preedit
        
        chiSymbolBuf=unicode(self.__context.buffer_String(),'utf-8')
        chiSymbolCursor = self.__context.cursor_Current()

        zuinBuf = unicode(self.__context.zuin_String(None),'utf-8')
        preedit_string = u"".join(
                chiSymbolBuf[:chiSymbolCursor] +
                zuinBuf +
                chiSymbolBuf[chiSymbolCursor:])
        attrs = ibus.AttrList()
        attr = ibus.AttributeForeground(0xffffff,
                chiSymbolCursor, chiSymbolCursor + len(zuinBuf))
        attrs.append(attr)
        attr = ibus.AttributeBackground(0x0,
                chiSymbolCursor, chiSymbolCursor + len(zuinBuf))
        attrs.append(attr)
        attr = ibus.AttributeUnderline(ibus.ATTR_UNDERLINE_SINGLE,
                0, len(preedit_string))
        attrs.append(attr)
        self.update_preedit (preedit_string, attrs, chiSymbolCursor, len(preedit_string) > 0)

        # update lookup table
        self.__lookup_table.clean()
        choicePerPage=self.__context.cand_ChoicePerPage()
        cand_toSkip=self.__context.cand_CurrentPage()*choicePerPage

        if self.__context.cand_TotalChoice()>0:
            self.__context.cand_Enumerate()

            for i in range(cand_toSkip):
                candidate=self.__context.cand_String()

            for i in range(choicePerPage):
                candidate = self.__context.cand_String()
                if candidate:
                    self.__lookup_table.append_candidate(unicode(candidate,'utf-8'))
                else:
                    break
            self.update_lookup_table (self.__lookup_table, True)
        else:
            self.update_lookup_table (self.__lookup_table, False)
	

        # update aux string
        text = u"".join(unicode(self.__context.aux_String(),'utf-8'))
        if text:
            self.update_aux_string(text, None, True)
        else:
            self.update_aux_string(u"", None, False)


        if self.__context.keystroke_CheckAbsorb():
            return True
        if self.__context.keystroke_CheckIgnore():
            return False
        return True
    # reset values of engine
    def __reset(self):
        self.__lookup_table.clean()
        self.__context.Reset()
        self.__commit()

    def __refreash_chieng_property(self):
        if self.__context.get_ChiEngMode() == CHINESE_MODE:
            self.__chieng_property.label = _(u"Chi")
        else:
            self.__chieng_property.label = _(u"Eng")
        self.update_property(self.__chieng_property)

    def __refreash_letter_property(self):
        if self.__context.get_ShapeMode() == FULLSHAPE_MODE:
            self.__letter_property.label = _(u"Full")
        else:
            self.__letter_property.label = _(u"Half")
        self.update_property(self.__letter_property)

    def __refreash_kbtype_property(self):
        mode = self.__context.get_KBType()
        labels = {
            DEFAULT_KBTYPE: _(u"Default"),
            HSU_KBTYPE: _(u"Hsu's"),
            IBM_KBTYPE: _(u"IBM"),
            GINYIEH_KBTYPE: _(u"Gin-Yieh"),
            ETEN_KBTYPE: _(u"ETen"),
            ETEN26_KBTYPE: _(u"ETen 26-key"),
            DVORAK_KBTYPE: _(u"Dvorak"),
            DVORAKHSU_KBTYPE: _(u"Dvorak Hsu's"),
            HANYU_KBTYPE: _(u"Han-Yu"),
        }
        self.__kbtype_property.label = labels.get(mode, _(u"Default"))
        self.update_property(self.__kbtype_property)

    def __refreash_properties(self):
        self.__refreash_chieng_property()
        self.__refreash_letter_property()
        self.__refreash_kbtype_property()

    def page_up(self):
        self.__context.handle_PageUp()
        self.__commit()
        return True

    def page_down(self):
        self.__context.handle_PageDown()
        self.__commit()
        return True

    def cursor_up(self):
        self.__context.handle_Up()
        self.__commit()
        return True

    def cursor_down(self):
        self.__context.handle_Down()
        self.__commit()
        return True
	
    def is_ascii(self, keyval):
        if keyval >= 0x30 and keyval <= 0x39:
	    # 0-9
	    return True
	elif keyval >= 0x41 and keyval <= 0x5a:
	    return True
	elif keyval >= 0x61 and keyval <= 0x7a:
	    return True
	return False
	

    def process_key_event(self, keyval, is_press, state):
        # ignore key release events
        if not is_press:
            return False
        state = state & (modifier.SHIFT_MASK | modifier.CONTROL_MASK | modifier.MOD1_MASK)

        if state == 0:
            if keyval == keysyms.Return:
                self.__context.handle_Enter()
            elif keyval == keysyms.Escape:
                self.__context.handle_Esc()
            elif keyval == keysyms.BackSpace:
                self.__context.handle_Backspace()
            elif keyval == keysyms.Delete or keyval == keysyms.KP_Delete:
                self.__context.handle_Del()
            elif keyval == keysyms.space or keyval == keysyms.KP_Space:
                self.__context.handle_Space()
            elif keyval == keysyms.Page_Up or keyval == keysyms.KP_Page_Up:
                self.__context.handle_PageUp()
            elif keyval == keysyms.Page_Down or keyval == keysyms.KP_Page_Down:
                self.__context.handle_PageDown()
            elif keyval == keysyms.Up or keyval == keysyms.KP_Up:
                self.__context.handle_Up()
            elif keyval == keysyms.Down or keyval == keysyms.KP_Down:
                self.__context.handle_Down()
            elif keyval == keysyms.Left:
                self.__context.handle_Left()
            elif keyval == keysyms.Right:
                self.__context.handle_Right()
            elif keyval == keysyms.Home or keyval == keysyms.KP_Home:
                self.__context.handle_Home()
            elif keyval == keysyms.End or keyval == keysyms.KP_End:
                self.__context.handle_End()
            elif keyval == keysyms.Tab:
                self.__context.handle_Tab()
            elif keyval == keysyms.Tab:
                self.__context.handle_Tab()
            else:
                self.__context.handle_Default(chr(keyval))
        elif state == modifier.SHIFT_MASK:
            if keyval == keysyms.Shift_L:
                self.__context.handle_ShiftLeft()
            elif keyval == keysyms.Shift_R:
                self.__context.handle_ShiftRight()
            elif keyval == keysyms.space or keyval == keysyms.KP_Space:
                self.__context.handle_ShiftSpace()
                self.property_activate("letter")
            else:
                self.__context.handle_Default(chr(keyval))
        elif state == modifier.CONTROL_MASK:
            if keyval >= keysyms._0 and keyval <= keysyms._9:
                self.__context.handle_CtrlNum(keyval)
            else:
                return False
        else:
            return False

        return self.__commit()

    def focus_in(self):
        self.register_properties(self.__prop_list)
        self.__refreash_properties()

    def focus_out(self):
        pass

    def property_activate(self, prop_name, prop_state = ibus.PROP_STATE_UNCHECKED):
        if prop_name == "chieng":
            if self.__context.get_ChiEngMode() == CHINESE_MODE:
                self.__context.set_ChiEngMode(SYMBOL_MODE)
            else:
                self.__context.set_ChiEngMode(CHINESE_MODE)
        elif prop_name == "letter":
            if self.__context.get_ShapeMode() == FULLSHAPE_MODE:
                self.__context.set_ShapeMode(HALFSHAPE_MODE)
            else:
                self.__context.set_ShapeMode(FULLSHAPE_MODE)
        elif prop_name == "kbtype":
            _type = self.__context.get_KBType()
            if _type == LAST_KBTYPE:
                _type = FIRST_KBTYPE
            else:
                _type += 1
            self.__context.set_KBType(_type)
        self.__refreash_properties()

