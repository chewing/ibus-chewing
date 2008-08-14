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
    asprintf (&hash_path, "%s/.chewing/", getenv ("HOME"));
    chewing_Init (CHEWING_DATADIR, hash_path);
    free(hash_path);
%}

/* define typemap PyObject * */
%typemap (out) PyObject * {
    $result = $1;
}

int chewing_Init (const char *, const char *);
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

    int Configure (int selectAreaLen = 18,
            int maxChiSymbolLen = 16,
            char *selKey = NULL,
            int addPhraseForward = 0,
            int spaceAsSelection = 1,
            int escCleanAllBuf = 0,
            int hsuSelKeyType = HSU_SELKEY_TYPE1) {
        ChewingConfigData config;
        config.selectAreaLen = selectAreaLen;
        config.maxChiSymbolLen = maxChiSymbolLen;
        config.bAddPhraseForward = addPhraseForward;
        config.bSpaceAsSelection = spaceAsSelection;
        config.bEscCleanAllBuf = escCleanAllBuf;
        config.hsuSelKeyType = hsuSelKeyType;
        if (selKey == NULL)
            selKey = "1234567890";
        int i;
        for (i = 0; selKey[i] != '\0' && i <= 9; i++) {
            config.selKey[i] = selKey[i];
        }
        config.selKey[i] = '\0';
        chewing_Configure (self, &config);
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

/* define properties */
%immutable;
    PyObject *chiSymbolBuf;
    long chiSymbolCursor;
    PyObject *zuinBuf;
    PyObject *dispInterval;
    PyObject *commitStr;
    int keystrokeRtn;
    PyObject *showMsg;
    /*
    PyObject *preedit_string;
    PyObject *message;
    */
%mutable;

%{
    PyObject *
    ChewingContext_chiSymbolBuf_get (ChewingContext *self) {
        ChewingOutput *output = self->output;

        if (output == NULL || output->chiSymbolBufLen <= 0) {
            return PyList_New (0);
        }

        PyObject *retval = PyList_New (output->chiSymbolBufLen);
        int i;
        for (i = 0; i < output->chiSymbolBufLen; i++) {
            PyObject *o = PyUnicode_DecodeUTF8 (
                            output->chiSymbolBuf[i].s,
                            MAX_UTF8_SIZE,
                            NULL);
            PyList_SetItem (retval, i, o);
        }

        return retval;
    }

    long
    ChewingContext_chiSymbolCursor_get (ChewingContext *self) {
        if (!self->output)
            return 0;
        return self->output->chiSymbolCursor;
    }

    PyObject *
    ChewingContext_zuinBuf_get (ChewingContext *self) {
        ChewingOutput *output = self->output;

        if (output == NULL) {
            return PyList_New (0);
        }

        PyObject *retval = PyList_New (0);
        int i;
        for (i = 0; output->zuinBuf[i].s[0] != '\0'; i++) {
            PyObject *o = PyUnicode_DecodeUTF8 (
                            output->zuinBuf[i].s,
                            MAX_UTF8_SIZE,
                            NULL);
            PyList_SetItem (retval, i, o);
        }

        return retval;
    }

    PyObject *
    ChewingContext_dispInterval_get (ChewingContext *self) {
        ChewingOutput *output = self->output;

        if (output == NULL || output->nDispInterval <= 0) {
            return PyList_New (0);
        }

        PyObject *retval = PyList_New (output->nDispInterval);
        int i;
        for (i = 0; i < output->nDispInterval; i++) {
            PyObject *o = PyTuple_New (2);
            PyTuple_SetItem (o, 0, 
                PyInt_FromLong (output->dispInterval[i].from));
            PyTuple_SetItem (o, 1, 
                PyInt_FromLong (output->dispInterval[i].to));
            PyList_SetItem (retval, i, o);
        }

        return retval;
    }


    PyObject *
    ChewingContext_commitStr_get (ChewingContext *self) {
        ChewingOutput *output = self->output;

        if (output == NULL || output->nCommitStr <= 0) {
            return PyList_New (0);
        }

        PyObject *retval = PyList_New (output->nCommitStr);
        int i;
        for (i = 0; i < output->nCommitStr; i++) {
            PyObject *o = PyUnicode_DecodeUTF8 (
                            output->commitStr[i].s,
                            MAX_UTF8_SIZE,
                            NULL);
            PyList_SetItem (retval, i, o);
        }

        return retval;
    }

    int
    ChewingContext_keystrokeRtn_get (ChewingContext *self) {
        if (!self->output)
            return 0;
        return self->output->keystrokeRtn;
    }

    PyObject *
    ChewingContext_showMsg_get (ChewingContext *self) {
        ChewingOutput *output = self->output;

        if (output == NULL ||
            output->showMsgLen <= 0 ||
            output->bShowMsg == 0) {
            return PyList_New (0);
        }

        PyObject *retval = PyList_New (output->showMsgLen);
        int i;
        for (i = 0; i < output->showMsgLen; i++) {
            PyObject *o = PyUnicode_DecodeUTF8 (
                            output->showMsg[i].s,
                            MAX_UTF8_SIZE,
                            NULL);
            PyList_SetItem (retval, i, o);
        }

        return retval;
    }
%}
};

