# vim:set et sts=4 sw=4:
# -*- coding: utf-8 -*-
#
# ibus-anthy - The Anthy engine for IBus
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
import anthy
from tables import *
from ibus import keysyms

MODE_HIRAGANA, \
MODE_KATAKANA, \
MODE_HALF_WIDTH_KATAKANA, \
MODE_LATIN, \
MODE_WIDE_LATIN = range(0, 5)

_ = lambda a: a

class Engine(ibus.EngineBase):
    def __init__(self, conn, object_path):
        super(Engine, self).__init__(conn, object_path)

        # create anthy context
        self.__context = anthy.anthy_context()
        self.__context._set_encoding(anthy.ANTHY_UTF8_ENCODING)

        # init state
        self.__input_mode = MODE_HIRAGANA
        self.__prop_dict = {}

        self.__lookup_table = ibus.LookupTable()
        self.__prop_list = self.__init_props()

        # use reset to init values
        self.__reset()

    def __init_props(self):
        props = ibus.PropList()

        # init input mode properties
        mode_prop = ibus.Property(name = "InputMode",
                            type = ibus.PROP_TYPE_MENU,
                            label = "あ",
                            tooltip = "Switch input mode")
        self.__prop_dict["InputMode"] = mode_prop

        mode_props = ibus.PropList()
        mode_props.append(ibus.Property(name = "InputMode.Hiragana",
                                        type = ibus.PROP_TYPE_RADIO,
                                        label = "Hiragana"))
        mode_props.append(ibus.Property(name = "InputMode.Katakana",
                                        type = ibus.PROP_TYPE_RADIO,
                                        label = "Katakana"))
        mode_props.append(ibus.Property(name = "InputMode.HalfWidthKatakana",
                                        type = ibus.PROP_TYPE_RADIO,
                                        label = "Half width katakana"))
        mode_props.append(ibus.Property(name = "InputMode.Latin",
                                        type = ibus.PROP_TYPE_RADIO,
                                        label = "Latin"))
        mode_props.append(ibus.Property(name = "InputMode.WideLatin",
                                        type = ibus.PROP_TYPE_RADIO,
                                        label = "Wide Latin"))

        mode_props[self.__input_mode].set_state(ibus.PROP_STATE_CHECKED)

        for prop in mode_props:
            self.__prop_dict[prop.get_name()] = prop

        mode_prop.set_sub_props(mode_props)
        props.append(mode_prop)


        # init test property
        test_prop = ibus.Property(name = "TestProp",
                            type = ibus.PROP_TYPE_TOGGLE,
                            label = "あ",
                            tooltip = "test property")
        self.__prop_dict["TestProp"] = test_prop
        props.append(test_prop)


        return props

    # reset values of engine
    def __reset(self):
        self.__input_chars = u""
        self.__convert_chars = u""
        self.__cursor_pos = 0
        self.__need_update = False
        self.__convert_begined = False
        self.__segments = list()
        self.__lookup_table.clean()
        self.__lookup_table_visible = False

    def page_up(self):
        # only process cursor down in convert mode
        if not self.__convert_begined:
            return False

        if not self.__lookup_table.page_up():
            return False

        candidate = self.__lookup_table.get_current_candidate()[0]
        index = self.__lookup_table.get_cursor_pos()
        self.__segments[self.__cursor_pos] = index, candidate
        self.__invalidate()
        return True

    def page_down(self):
        # only process cursor down in convert mode
        if not self.__convert_begined:
            return False

        if not self.__lookup_table.page_down():
            return False

        candidate = self.__lookup_table.get_current_candidate()[0]
        index = self.__lookup_table.get_cursor_pos()
        self.__segments[self.__cursor_pos] = index, candidate
        self.__invalidate()
        return True

    def cursor_up(self):
        # only process cursor down in convert mode
        if not self.__convert_begined:
            return False

        if not self.__lookup_table.cursor_up():
            return False

        candidate = self.__lookup_table.get_current_candidate()[0]
        index = self.__lookup_table.get_cursor_pos()
        self.__segments[self.__cursor_pos] = index, candidate
        self.__invalidate()
        return True

    def cursor_down(self):
        # only process cursor down in convert mode
        if not self.__convert_begined:
            return False

        if not self.__lookup_table.cursor_down():
            return False

        candidate = self.__lookup_table.get_current_candidate()[0]
        index = self.__lookup_table.get_cursor_pos()
        self.__segments[self.__cursor_pos] = index, candidate
        self.__invalidate()
        return True

    def __commit_string(self, text):
        self.__reset()
        self.commit_string(text)
        self.__invalidate()

    def process_key_event(self, keyval, is_press, state):
        # ignore key release events
        if not is_press:
            return False

        if keyval == keysyms.Return:
            return self.__on_key_return()
        elif keyval == keysyms.Escape:
            return self.__on_key_escape()
        elif keyval == keysyms.BackSpace:
            return self.__on_key_back_space()
        elif keyval == keysyms.Delete or keyval == keysyms.KP_Delete:
            return self.__on_key_delete()
        elif keyval == keysyms.space:
            return self.__on_key_space()
        elif keyval >= keysyms._1 and keyval <= keysyms._9:
            index = keyval - keysyms._1
            return self.__on_key_number(index)
        elif keyval == keysyms.Page_Up or keyval == keysyms.KP_Page_Up:
            return self.__on_key_page_up()
        elif keyval == keysyms.Page_Down or keyval == keysyms.KP_Page_Down:
            return self.__on_key_page_down()
        elif keyval == keysyms.Up:
            return self.__on_key_up()
        elif keyval == keysyms.Down:
            return self.__on_key_down()
        elif keyval == keysyms.Left:
            return self.__on_key_left()
        elif keyval == keysyms.Right:
            return self.__on_key_right()
        elif keyval in xrange(keysyms.a, keysyms.z + 1) or \
            keyval in xrange(keysyms.A, keysyms.Z + 1):
            return self.__on_key_common(keyval)
        else:
            return True

        return False

    def property_activate(self, prop_name, state):
        prop = self.__prop_dict[prop_name]
        prop.set_state(state)

        if state == ibus.PROP_STATE_CHECKED:
            if prop_name == "InputMode.Hiragana":
                prop = self.__prop_dict["InputMode"]
                prop.set_label(_("あ"))
                self.__input_mode = MODE_HIRAGANA
                self.update_property(prop)
            elif prop_name == "InputMode.Katakana":
                prop = self.__prop_dict["InputMode"]
                prop.set_label(_("ア"))
                self.__input_mode = MODE_KATAKANA
                self.update_property(prop)
            elif prop_name == "InputMode.HalfWidthKatakana":
                prop = self.__prop_dict["InputMode"]
                prop.set_label(_("ｱ"))
                self.__input_mode = MODE_HALF_WIDTH_KATAKANA
                self.update_property(prop)
            elif prop_name == "InputMode.Latin":
                prop = self.__prop_dict["InputMode"]
                self.__input_mode = MODE_LATIN
                prop.set_label(_("A"))
                self.update_property(prop)
            elif prop_name == "InputMode.WideLatin":
                prop = self.__prop_dict["InputMode"]
                prop.set_label(_("Ａ"))
                self.__input_mode = MODE_WIDE_LATIN
                self.update_property(prop)

    def focus_in(self):
        self.register_properties(self.__prop_list)

    def focus_out(self):
        pass

    # begine convert
    def __begin_convert(self):
        if self.__convert_begined:
            return
        self.__convert_begined = True

        self.__context.set_string(self.__input_chars.encode("utf-8"))
        conv_stat = anthy.anthy_conv_stat()
        self.__context.get_stat(conv_stat)

        for i in xrange(0, conv_stat.nr_segment):
            buf = self.__context.get_segment(i, 0)
            text = unicode(buf, "utf-8")
            self.__segments.append((0, text))

        self.__cursor_pos = 0
        self.__fill_lookup_table()
        self.__lookup_table_visible = False

    def __fill_lookup_table(self):
        # get segment stat
        seg_stat = anthy.anthy_segment_stat()
        self.__context.get_segment_stat(self.__cursor_pos, seg_stat)

        # fill lookup_table
        self.__lookup_table.clean()
        for i in xrange(0, seg_stat.nr_candidate):
            buf = self.__context.get_segment(self.__cursor_pos, i)
            candidate = unicode(buf, "utf-8")
            self.__lookup_table.append_candidate(candidate)


    def __invalidate(self):
        if self.__need_update:
            return
        self.__need_update = True
        gobject.idle_add(self.__update, priority = gobject.PRIORITY_LOW)


    def __update_input_chars(self):
        begin, end  = max(self.__cursor_pos - 4, 0), self.__cursor_pos

        for i in range(begin, end):
            text = self.__input_chars[i:end]
            romja = romaji_typing_rule.get(text, None)
            if romja != None:
                self.__input_chars = u"".join((self.__input_chars[:i], romja, self.__input_chars[end:]))
                self.__cursor_pos -= len(text)
                self.__cursor_pos += len(romja)

        attrs = ibus.AttrList()
        attrs.append(ibus.AttributeUnderline(ibus.ATTR_UNDERLINE_SINGLE, 0, len(self.__input_chars.encode("utf-8"))))

        self.update_preedit(self.__input_chars, attrs, self.__cursor_pos, len(self.__input_chars) > 0)
        self.update_aux_string(u"", ibus.AttrList(), False)
        self.update_lookup_table(self.__lookup_table, self.__lookup_table_visible)

    def __update_convert_chars(self):
        self.__convert_chars = u""
        pos = 0
        i = 0
        for seg_index, text in self.__segments:
            self.__convert_chars += text
            if i <= self.__cursor_pos:
                pos += len(text)
            i += 1

        attrs = ibus.AttrList()
        attrs.append(ibus.AttributeUnderline(ibus.ATTR_UNDERLINE_SINGLE, 0, len(self.__convert_chars)))
        attrs.append(ibus.AttributeBackground(ibus.RGB(200, 200, 240),
                pos - len(self.__segments[self.__cursor_pos][1]),
                pos))
        self.update_preedit(self.__convert_chars, attrs, pos, True)
        aux_string = u"( %d / %d )" % (self.__lookup_table.get_cursor_pos() + 1, self.__lookup_table.get_number_of_candidates())
        self.update_aux_string(aux_string, ibus.AttrList(), self.__lookup_table_visible)
        self.update_lookup_table(self.__lookup_table, self.__lookup_table_visible)

    def __update(self):
        self.__need_update = False
        if self.__convert_begined == False:
            self.__update_input_chars()
        else:
            self.__update_convert_chars()

    def __on_key_return(self):
        if not self.__input_chars:
            return False
        if self.__convert_begined == False:
            self.__commit_string(self.__input_chars)
        else:
            i = 0
            for seg_index, text in self.__segments:
                self.__context.commit_segment(i, seg_index)
            self.__commit_string(self.__convert_chars)
        return True

    def __on_key_escape(self):
        if not self.__input_chars:
            return False
        self.__reset()
        self.__invalidate()
        return True

    def __on_key_back_space(self):
        if not self.__input_chars:
            return False

        if self.__convert_begined:
            self.__convert_begined = False
            self.__cursor_pos = len(self.__input_chars)
            self.__lookup_table.clean()
            self.__lookup_table_visible = False
        elif self.__cursor_pos > 0:
            self.__input_chars = self.__input_chars[:self.__cursor_pos - 1] + self.__input_chars [self.__cursor_pos:]
            self.__cursor_pos -= 1

        self.__invalidate()
        return True

    def __on_key_delete(self):
        if not self.__input_chars:
            return False

        if self.__convert_begined:
            self.__convert_begined = False
            self.__cursor_pos = len(self.__input_chars)
            self.__lookup_table.clean()
            self.__lookup_table_visible = False
        elif self.__cursor_pos < len(self.__input_chars):
            self.__input_chars = self.__input_chars[:self.__cursor_pos] + self.__input_chars [self.__cursor_pos + 1:]

        self.__invalidate()
        return True

    def __on_key_space(self):
        if not self.__input_chars:
            return False
        if self.__convert_begined == False:
            self.__begin_convert()
            self.__invalidate()
        else:
            self.__lookup_table_visible = True
            self.cursor_down()
        return True

    def __on_key_up(self):
        if not self.__input_chars:
            return False
        self.__lookup_table_visible = True
        self.cursor_up()
        return True

    def __on_key_down(self):
        if not self.__input_chars:
            return False
        self.__lookup_table_visible = True
        self.cursor_down()
        return True

    def __on_key_page_up(self):
        if not self.__input_chars:
            return False
        if self.__lookup_table_visible == True:
            self.page_up()
        return True

    def __on_key_page_down(self):
        if not self.__input_chars:
            return False
        if self.__lookup_table_visible == True:
            self.page_down()
        return True

    def __on_key_left(self):
        if not self.__input_chars:
            return False
        if self.__cursor_pos == 0:
            return True
        self.__cursor_pos -= 1
        self.__lookup_table_visible = False
        self.__fill_lookup_table()
        self.__invalidate()
        return True

    def __on_key_right(self):
        if not self.__input_chars:
            return False

        if self.__convert_begined:
            max_pos = len(self.__segments) - 1
        else:
            max_pos = len(self.__input_chars)
        if self.__cursor_pos == max_pos:
            return True
        self.__cursor_pos += 1
        self.__lookup_table_visible = False
        self.__fill_lookup_table()
        self.__invalidate()

        return True

    def __on_key_number(self, index):
        if not self.__input_chars:
            return False

        if self.__convert_begined and self.__lookup_table_visible:
            candidates = self.__lookup_table.get_canidates_in_current_page()
            if self.__lookup_table.set_cursor_pos_in_current_page(index):
                index = self.__lookup_table.get_cursor_pos()
                candidate = self.__lookup_table.get_current_candidate()[0]
                self.__segments[self.__cursor_pos] = index, candidate
                self.__lookup_table_visible = False
                self.__on_key_right()
                self.__invalidate()
        return True


    def __on_key_common(self, keyval):
        if self.__convert_begined:
            i = 0
            for seg_index, text in self.__segments:
                self.__context.commit_segment(i, seg_index)
            self.__commit_string(self.__convert_chars)
        self.__input_chars += unichr(keyval)
        self.__cursor_pos += 1
        self.__invalidate()
        return True


