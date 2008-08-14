/* vim:set et ts=4: */
/*
 * ibus-chewing - The Chewing engine for IBus
 *
 * Copyright (c) 2007-2008 Huang Peng <shawn.p.huang@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

%module chewing
%{
 /* Put header files here or function declarations like below */
#include <stdio.h>
#include <stdlib.h>
#include <chewing.h>
%}

%init %{
    char *hash_path = NULL;
    asprintf (&hash_path, "%s/.chewing", getenv ("HOME"));
    chewing_Init (CHEWING_DATADIR, hash_path);
    free(hash_path);
%}

void chewing_Terminate ();
int chewing_KBStr2Num (char *);

/* ChewingContext */
typedef struct {} ChewingContext;
%extend ChewingContext {
    ChewingContext () {
        return chewing_new ();
    }

    ~ChewingContext () {
        chewing_free (self);
    }

    int Reset () {
        return chewing_Reset (self);
    }

    int set_KBType (int kbtype) {
        return chewing_set_KBType (self, kbtype);
    }

    int get_KBType () {
        return chewing_get_KBType (self);
    }

    void set_ChiEngMode (int mode) {
        chewing_set_ChiEngMode (self, mode);
    }

    int get_ChiEngMode () {
        return chewing_get_ChiEngMode (self);
    }

    void set_ShapeMode (int mode) {
        chewing_set_ShapeMode (self, mode);
    }

    int get_ShapeMode () {
        return chewing_get_ShapeMode (self);
    }

    /* define handle methods */

    int handle_Space () {
        return chewing_handle_Space (self);
    }
    int handle_Esc () {
        return chewing_handle_Esc (self);
    }
    int handle_Enter () {
        return chewing_handle_Enter (self);
    }
    int handle_Del () {
        return chewing_handle_Del (self);
    }
    int handle_Backspace () {
        return chewing_handle_Backspace (self);
    }
    int handle_Tab () {
        return chewing_handle_Tab (self);
    }
    int handle_ShiftLeft () {
        return chewing_handle_ShiftLeft (self);
    }
    int handle_ShiftRight () {
        return chewing_handle_ShiftRight (self);
    }
    int handle_Left () {
        return chewing_handle_Left (self);
    }
    int handle_Right () {
        return chewing_handle_Right (self);
    }
    int handle_Up () {
        return chewing_handle_Up (self);
    }
    int handle_Home () {
        return chewing_handle_Home (self);
    }
    int handle_End () {
        return chewing_handle_End (self);
    }
    int handle_PageUp () {
        return chewing_handle_PageUp (self);
    }
    int handle_PageDown () {
        return chewing_handle_PageDown (self);
    }
    int handle_Down () {
        return chewing_handle_Down (self);
    }
    int handle_Capslock () {
        return chewing_handle_Capslock (self);
    }
    int handle_Default (int key) {
        return chewing_handle_Default (self, key);
    }
    int handle_CtrlNum (int key) {
        return chewing_handle_CtrlNum (self, key);
    }
    int handle_CtrlOption (int key) {
        return chewing_handle_CtrlOption (self, key);
    }
    int handle_ShiftSpace () {
        return chewing_handle_ShiftSpace (self);
    }
    int handle_DblTab () {
        return chewing_handle_DblTab (self);
    }
    int handle_Numlock (int key) {
        return chewing_handle_Numlock (self, key);
    }
};

