const gchar *page_labels[]={
    N_("Editing"),
    N_("Selecting"),
    N_("Keyboard"),
    NULL
};

const gchar *button_labels[]={
    GTK_STOCK_CLOSE,
    NULL
};

GtkResponseType button_responses[]={
    GTK_RESPONSE_CLOSE,
};

//const int selKeys_default[10]={'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
const gchar *kbType_ids[]={
    N_("default"),
    N_("hsu"),
    N_("ibm"),
    N_("gin_yieh"),
    N_("eten"),
    N_("eten26"),
    N_("dvorak"),
    N_("dvorak_hsu"),
    N_("dachen_26"),
    N_("hanyu"),
    NULL
};

#define SELKEYS_ARRAY_SIZE 7
const gchar *selKeys_array[SELKEYS_ARRAY_SIZE+1]={
    "1234567890",
    "asdfghjkl;",
    "asdfzxcv89",   
    "asdfjkl789",
    "aoeuhtn789",
    "1234qweras",
    NULL
};

static ChewingKbType kbType_id_get_index(const gchar *kbType_id){
    ChewingKbType i=0;
    for(i=0;kbType_ids[i]!=NULL;i++){
	if (strcmp(kbType_id,kbType_ids[i])==0){
	    return i;
	}
    }
    return CHEWING_KBTYPE_INVALID;
}

/*===== Callback functions =====*/

static void KBType_set_callback(PropertyContext *ctx, GValue *value){
    ChewingKbType kbType=kbType_id_get_index(g_value_get_string(value));
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_KBType(engine->context,kbType);
}

static void selKeys_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    ibus_chewing_engine_set_selKeys_string(engine,g_value_get_string(value));
}

static void hsuSelKeyType_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_hsuSelKeyType(engine->context,g_value_get_int(value));
}

static void autoShiftCur_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_autoShiftCur(engine->context,(g_value_get_boolean(value)) ? 1: 0);
}

static void addPhraseDirection_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_addPhraseDirection(engine->context,(g_value_get_boolean(value)) ? 1: 0);
}

static void easySymbolInput_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_easySymbolInput(engine->context,(g_value_get_boolean(value)) ? 1: 0);
}

static void escCleanAllBuf_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_escCleanAllBuf(engine->context,(g_value_get_boolean(value)) ? 1: 0);
}

static void maxChiSymbolLen_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_maxChiSymbolLen(engine->context,g_value_get_int(value));
}

static void candPerPage_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_candPerPage(engine->context,g_value_get_int(value));
}

static void phraseChoiceRearward_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_phraseChoiceRearward(engine->context,(g_value_get_boolean(value)) ? 1: 0);
}

static void spaceAsSelection_set_callback(PropertyContext *ctx, GValue *value){
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_spaceAsSelection(engine->context,(g_value_get_boolean(value)) ? 1: 0);
}

/*===== End of Callback functions =====*/

PropertySpec propSpecs[]={
    {G_TYPE_STRING, "KBType", "Keyboard",  N_("Keyboard Type"), 
	"default", kbType_ids,	0, 0, 
	NULL, KBType_set_callback,
	MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE, 0, 0, 
	NULL,
    },
    {G_TYPE_STRING, "selKeys", "Keyboard",  N_("Selection keys"), 
	"1234567890", selKeys_array, 0, 0,
	NULL, selKeys_set_callback,
	0, 0, 0,
	NULL,
    },
    {G_TYPE_INT, "hsuSelKeyType", "Keyboard", N_("Hsu's selection key"),
	"1", NULL, 1, 2, 
	NULL, hsuSelKeyType_set_callback,
	0, 0, 0,
	NULL,
    },

    {G_TYPE_BOOLEAN, "autoShiftCur", "Editing", N_("Auto move cursor"),
	"0", NULL, 0, 1,
	NULL, autoShiftCur_set_callback,
	0, 0, 0,
	NULL,
    },
    {G_TYPE_BOOLEAN, "addPhraseDirection", "Editing", N_("Add phrase in front"),
	"0", NULL, 0, 1, 
	NULL, addPhraseDirection_set_callback,
	0, 0, 0,
	NULL,
    },
    {G_TYPE_BOOLEAN, "easySymbolInput", "Editing", N_("Easy symbol input"),
	"0", NULL, 0, 1,
	NULL, easySymbolInput_set_callback,
	0, 0, 0,
	NULL,
    },
    {G_TYPE_BOOLEAN, "escCleanAllBuf", "Editing", N_("Esc clean all buffer"),
	"0", NULL, 0, 1, 
	NULL, escCleanAllBuf_set_callback,
	0, 0, 0,
	NULL,
    },
    {G_TYPE_INT, "maxChiSymbolLen", "Editing", N_("Maximum Chinese characters"), 
	"16", NULL, 4, 30,
	NULL, maxChiSymbolLen_set_callback,
	0, 0, 0,
	N_("Maximum Chinese characters in pre-edit buffer"),
    },

    {G_TYPE_INT, "candPerPage", "Selecting", N_("Candidate per page"), 
	"10", NULL, 8, 10,
	NULL, candPerPage_set_callback,
	0, 0, 0,
	NULL,
    },
    {G_TYPE_BOOLEAN, "phraseChoiceRearward", "Selecting", N_("Choice phrase from the end"),
	"1", NULL, 0, 1, 
	NULL, phraseChoiceRearward_set_callback,
	0, 0, 0,
	NULL,
    },
    {G_TYPE_BOOLEAN, "spaceAsSelection", "Selecting", N_("Space as selection key"),
	"1", NULL, 0, 1, 
	NULL, spaceAsSelection_set_callback,
	0, 0, 0,
	NULL,
    },
    {G_TYPE_INVALID, "", "", "",
	"", NULL, 0, 0, 
	NULL, NULL,
	0, 0, 0,
	NULL,
    },

};



/*============================================
 * Supporting functions
 */

static gint parameter_get_index(const char *key){
    int i;
    for (i=0;parameters[i].type!=GCONF_VALUE_INVALID;i++){
	if (strcmp(parameters[i].key,key)==0)
	    return i;
    }
    return -1;
}


static gunichar *preedit_string_make(ChewingContext *context, 
	glong *zhuyin_item_written_ptr,  glong *item_written_ptr){
    glong chiSymbol_item_written=0;
    gchar *str_ptr=chewing_buffer_String(context);
    gunichar *chiSymbolBuf=g_utf8_to_ucs4_fast(str_ptr,-1, &chiSymbol_item_written);
    g_free(str_ptr);

    int chiSymbolCursor = chewing_cursor_Current(context);
    str_ptr=chewing_zuin_String(context,NULL);
    gunichar *zhuyinBuf=g_utf8_to_ucs4_fast(str_ptr,-1, zhuyin_item_written_ptr);
    g_free(str_ptr);

    glong i;
    *item_written_ptr=chiSymbol_item_written+ *zhuyin_item_written_ptr;
    gunichar *preeditBuf=g_new(gunichar, *item_written_ptr+1);
    glong j=0;
    for (i=0;i<*item_written_ptr;i++){
	if (i<chiSymbolCursor){
	    preeditBuf[i]=chiSymbolBuf[i];
	}else if (j<*zhuyin_item_written_ptr){
	    /* Inserting zhuyinBuf */
	    preeditBuf[i]=zhuyinBuf[j++];
	}else{
	    /* Append rest zhuyinBuf */
	    preeditBuf[i]=chiSymbolBuf[i- *zhuyin_item_written_ptr];
	}
    }
    preeditBuf[i]='\0';
    g_free(chiSymbolBuf);
    g_free(zhuyinBuf);
    return preeditBuf;
}

/*--------------------------------------------
 * Foreach functions
 */
static guint keyModifier_get(Display *pDisplay){
    Window    root_retrun, child_retrun;
    int     root_x_return, root_y_return, win_x_return, win_y_return;
    guint     mask_return;
    XQueryPointer(pDisplay, DefaultRootWindow(pDisplay), &root_retrun, &child_retrun,
	    &root_x_return, &root_y_return, &win_x_return, &win_y_return, &mask_return );
    return mask_return;
} 


/* 
 * From http://www.thelinuxpimp.com/files/keylockx.c
 */
//static guint key_get_state(KeySym key, Display *pDisplay){
//    guint     keyMask = 0;
//    XModifierKeymap* map = XGetModifierMapping(pDisplay);
//    KeyCode keyCode = XKeysymToKeycode(pDisplay,key);
//    if(keyCode == NoSymbol) return 0;
//    int i = 0;
//    while(i < 8) {
//        if( map->modifiermap[map->max_keypermod * i] == keyCode) {
//            keyMask = 1 << i;
//        }
//        i++;
//    }
//    XFreeModifiermap(map);
//    guint mask_return=keyModifier_get(pDisplay);

//    return (mask_return & keyMask) != 0;
//} 

/* 
 * From send_fake_key_eve() eve.c gcin
 */
static void key_send_fake_event(KeySym key, Display *pDisplay)
{
    KeyCode keyCode = XKeysymToKeycode(pDisplay, key);
    G_DEBUG_MSG(2,"key_sent_fake_event(%lx,-), keyCode=%x",key,keyCode);
    XTestFakeKeyEvent(pDisplay, keyCode, True, CurrentTime);
    XTestFakeKeyEvent(pDisplay, keyCode, False, CurrentTime);
    
}


