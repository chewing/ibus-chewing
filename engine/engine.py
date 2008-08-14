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
import chewing
from ibus import keysyms

_ = lambda a: a

class Engine(ibus.EngineBase):
    def __init__(self, bus, object_path):
        super(Engine, self).__init__(bus, object_path)

        # create anthy context
        self.__context = chewing.ChewingContext()
        self.__context.Configure(18, 16, "12345")

        # init state
        self.__lookup_table = ibus.LookupTable()
        self.__lookup_table.show_cursor(False)

        # use reset to init values
        self.__reset()

    # reset values of engine
    def __reset(self):
        self.__lookup_table.clean()
        self.__context.Reset()
        self.__commit()

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

    def process_key_event(self, keyval, is_press, state):
        # ignore key release events
        if not is_press:
            return False

        if keyval == keysyms.Return:
            self.__context.handle_Enter()
        elif keyval == keysyms.Escape:
            self.__context.handle_Esc()
        elif keyval == keysyms.BackSpace:
            self.__context.handle_Backspace()
        elif keyval == keysyms.Delete or keyval == keysyms.KP_Delete:
            self.__context.handle_Del()
        elif keyval == keysyms.space:
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
        else:
            self.__context.handle_Default(keyval)
        return self.__commit()

    def focus_in(self):
        # self.register_properties(self.__prop_list)
        pass

    def focus_out(self):
        pass

    def __commit(self):
        # commit string
        if self.__context.keystrokeRtn & chewing.KEYSTROKE_COMMIT:
            text = u"".join(self.__context.commitStr)
            self.commit_string(text)

        # update preedit
        chiSymbolBuf = self.__context.chiSymbolBuf
        chiSymbolCursor = self.__context.chiSymbolCursor
        preedit_string = u"".join(
            chiSymbolBuf[:chiSymbolCursor] +
            self.__context.zuinBuf +
            chiSymbolBuf[chiSymbolCursor:])
        self.update_preedit (preedit_string, None, chiSymbolCursor, True)

        # update lookup table
        self.__lookup_table.clean()
        ci = self.__context.ci
        if ci:
            for i in range(9):
                candidate = ci.get_candidate(i)
                if candidate:
                    self.__lookup_table.append_candidate(candidate)
            self.update_lookup_table (self.__lookup_table, True)
        else:
            self.update_lookup_table (self.__lookup_table, False)

        if self.__context.keystrokeRtn & chewing.KEYSTROKE_ABSORB:
            return True
        if self.__context.keystrokeRtn & chewing.KEYSTROKE_IGNORE:
            return False

        return True
