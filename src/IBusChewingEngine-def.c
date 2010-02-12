const gchar *page_labels[]={
    N_("Editing"),
    N_("Selecting"),
    N_("Keyboard"),
    NULL
};

const gchar *button_labels[]={
    GTK_STOCK_SAVE,
    NULL
};

GtkResponseType button_responses[]={
    GTK_RESPONSE_OK,
};


const gchar *toneKeys[]={
    "6347",  //Default
    "jfds",  //hsu
    "uiop",  //ibm
    "zaq1",  //gin-yieh
    "4321",  //eten
    "kjfd",  //eten26
    "6347",  //dvorak
    "thdn",  //dvorak_hsu
    "yert",  //dachen_26
    "1234",  //hanyu
    NULL
};

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


#define SELKEYS_ARRAY_SIZE 8
const gchar *selKeys_array[SELKEYS_ARRAY_SIZE+1]={
    "1234567890",
    "asdfghjkl;",
    "asdfzxcv89",
    "asdfjkl789",
    "aoeu;qjkix", /* Dvorak */
    "aoeuhtnsid", /* Dvorak */
    "aoeuidhtns", /* Dvorak */
    "1234qweras",
    NULL
};

const gchar *syncCapsLockLocal_strs[]={
    NC_("Sync","disable"),
    NC_("Sync","keyboard"),
    NC_("Sync","input method"),
    NULL
};

const gchar *outputCharsets[]={
    N_("Auto"),
    N_("Big5"),
    N_("UTF8"),
    NULL
};

const gchar *inputStyles[]={
    N_("in application window"),
    N_("in candidate window"),
    NULL
};


#ifdef IBUS_CHEWING_MAIN
static ChewingKbType kbType_id_get_index(const gchar *kbType_id){
    ChewingKbType i=0;
    for(i=0;kbType_ids[i]!=NULL;i++){
	if (strcmp(kbType_id,kbType_ids[i])==0){
	    return i;
	}
    }
    return CHEWING_KBTYPE_INVALID;
}
#endif

/*===== Callback functions =====*/

static void KBType_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    ChewingKbType kbType=kbType_id_get_index(g_value_get_string(value));
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_KBType(engine->context,kbType);
#endif
}

static void selKeys_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    ibus_chewing_engine_set_selKeys_string(engine,g_value_get_string(value));
    if (!engine->table){
	engine->table=g_object_ref_sink(ibus_lookup_table_new(strlen(g_value_get_string(value)),0,FALSE,TRUE));
    }
    ibus_chewing_engine_set_lookup_table_label(engine,g_value_get_string(value));
#endif
}

static void hsuSelKeyType_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_hsuSelKeyType(engine->context,g_value_get_int(value));
#endif
}

static void autoShiftCur_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_autoShiftCur(engine->context,(g_value_get_boolean(value)) ? 1: 0);
#endif
}

static void addPhraseDirection_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_addPhraseDirection(engine->context,(g_value_get_boolean(value)) ? 1: 0);
#endif
}

static void easySymbolInput_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_easySymbolInput(engine->context,(g_value_get_boolean(value)) ? 1: 0);
    if (g_value_get_boolean(value)){
	engine->flags |= CHEWING_FLAG_EASY_SYMBOL_INPUT;
    }else{
	engine->flags &= ~CHEWING_FLAG_EASY_SYMBOL_INPUT;
    }
#endif
}

static void escCleanAllBuf_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_escCleanAllBuf(engine->context,(g_value_get_boolean(value)) ? 1: 0);
#endif
}

/* Additional symbol buffer length */
static void maxChiSymbolLen_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_maxChiSymbolLen(engine->context,g_value_get_int(value));
#endif
}

static void forceLowercaseEnglish_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    if (g_value_get_boolean(value)){
	engine->flags |= CHEWING_FLAG_FORCE_LOWERCASE_ENGLISH;
    }else{
	engine->flags &= ~CHEWING_FLAG_FORCE_LOWERCASE_ENGLISH;
    }
#endif
}

static void syncCapsLockLocal_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    const gchar *str=g_value_get_string(value);
    if (strcmp(str,"keyboard")==0){
	engine->syncCapsLock_local=CHEWING_MODIFIER_SYNC_FROM_KEYBOARD;
    }else if (strcmp(str,"input method")==0){
	engine->syncCapsLock_local=CHEWING_MODIFIER_SYNC_FROM_IM;
    }else{
	engine->syncCapsLock_local=CHEWING_MODIFIER_SYNC_DISABLE;
    }
#endif
}

static void numpadAlwaysNumber_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    if (g_value_get_boolean(value)){
	engine->flags |= CHEWING_FLAG_NUMPAD_ALWAYS_NUMBER;
    }else{
	engine->flags &= ~CHEWING_FLAG_NUMPAD_ALWAYS_NUMBER;
    }
#endif
}

static void inputStyle_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    const gchar *str=g_value_get_string(value);
    if (strcmp(str,"in application window")==0){
	engine->_priv->inputStyle=CHEWING_INPUT_STYLE_IN_APPLICATION;
    }else  if (strcmp(str,"in candidate window")==0){
	engine->_priv->inputStyle=CHEWING_INPUT_STYLE_IN_CANDIDATE;
    }else{
	engine->_priv->inputStyle=CHEWING_INPUT_STYLE_IN_APPLICATION;
    }
    ibus_chewing_engine_force_commit(engine);
#endif
}

static void candPerPage_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_candPerPage(engine->context,g_value_get_int(value));
    if (engine->table){
	ibus_lookup_table_clear(engine->table);
	engine->table->page_size=g_value_get_int(value);
    }else{
	engine->table=g_object_ref_sink(ibus_lookup_table_new(g_value_get_int(value),0,FALSE,TRUE));
    }
#endif
}

static void phraseChoiceRearward_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_phraseChoiceRearward(engine->context,(g_value_get_boolean(value)) ? 1: 0);
#endif
}

static void spaceAsSelection_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    chewing_set_spaceAsSelection(engine->context,(g_value_get_boolean(value)) ? 1: 0);
#endif
}

static void plainZhuyin_set_callback(PropertyContext *ctx, GValue *value){
#ifdef IBUS_CHEWING_MAIN
    IBusChewingEngine *engine=(IBusChewingEngine *) ctx->userData;
    if (g_value_get_boolean(value)){
	engine->flags |= CHEWING_FLAG_PLAIN_ZHUYIN;
    }else{
	engine->flags &= ~CHEWING_FLAG_PLAIN_ZHUYIN;
    }
#endif
}

/*===== End of Callback functions =====*/

PropertySpec propSpecs[]={
    {G_TYPE_STRING, "KBType", "Keyboard",  N_("Keyboard Type"),
	"default", kbType_ids, NULL, 0, 0,
	NULL, KBType_set_callback,
	MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE | MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION, 0, 0,
	N_("Select keyboard layout of Zhuyin symbols."),
    },
    {G_TYPE_STRING, "selKeys", "Keyboard",  N_("Selection keys"),
	"1234567890", selKeys_array, NULL, 0, 0,
	NULL, selKeys_set_callback,
	0, 0, 0,
	N_("Keys used to select candidate. For example \"asdfghjkl;\", press 'a' to select the 1st candidate, 's' for 2nd, and so on."),
    },
    {G_TYPE_INT, "hsuSelKeyType", "Keyboard", N_("Hsu's selection key"),
	"1", NULL,  NULL, 1, 2,
	NULL, hsuSelKeyType_set_callback,
	0, 0, 0,
	N_("Hsu's keyboard selection keys, 1 for asdfjkl789, 2 for asdfzxcv89 ."),
    },

    {G_TYPE_BOOLEAN, "autoShiftCur", "Editing", N_("Auto move cursor"),
	"0", NULL, NULL, 0, 1,
	NULL, autoShiftCur_set_callback,
	0, 0, 0,
	N_("Automatically move cursor to next character."),
    },
    {G_TYPE_BOOLEAN, "addPhraseDirection", "Editing", N_("Add phrases in front"),
	"0", NULL, NULL, 0, 1,
	NULL, addPhraseDirection_set_callback,
	0, 0, 0,
	N_("Add phrases in the front."),
    },
    {G_TYPE_BOOLEAN, "easySymbolInput", "Editing", N_("Easy symbol input"),
	"1", NULL, NULL,  0, 1,
	NULL, easySymbolInput_set_callback,
	0, 0, 0,
	N_("Easy symbol input."),
    },
    {G_TYPE_BOOLEAN, "escCleanAllBuf", "Editing", N_("Esc clean all buffer"),
	"0", NULL, NULL,  0, 1,
	NULL, escCleanAllBuf_set_callback,
	0, 0, 0,
	N_("Escape key cleans the text in pre-edit-buffer."),
    },
    {G_TYPE_INT, "maxChiSymbolLen", "Editing", N_("Maximum Chinese characters"),
	"20", NULL, NULL,  8, 40,
	NULL, maxChiSymbolLen_set_callback,
	0, 0, 0,
	N_("Maximum Chinese characters in pre-edit buffer, including inputing Zhuyin symbols"),
    },
    {G_TYPE_BOOLEAN, "forceLowercaseEnglish", "Editing", N_("Force lowercase in En mode"),
	"0", NULL, NULL,  0, 1,
	NULL, forceLowercaseEnglish_set_callback,
	0, 0, 0,
	N_("Ignore CapsLock status and input lowercase by default.\n\
It is handy if you wish to enter lowercase by default. Uppercase can still be inputted with Shift."),
    },

    /* Sync between CapsLock and IM */
    {G_TYPE_STRING, "syncCapsLockLocal", "Editing", N_("Sync between CapsLock and IM"),
	"keyboard", syncCapsLockLocal_strs,  "Sync", 0, 1,
	NULL, syncCapsLockLocal_set_callback,
	MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE | MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION |
	    MAKER_DIALOG_PROPERTY_FLAG_TRANSLATION_WITH_CONTEXT,
       	0, 0,
	N_("Occasionally, the CapsLock status does not match the IM, \
this option determines how these status be synchronized. Valid values:\n\
\"disable\": Do nothing.\n\
\"keyboard\": IM status follows keyboard status.\n\
\"IM\": Keyboard status follows IM status."),
    },

    {G_TYPE_BOOLEAN, "numpadAlwaysNumber", "Editing", N_("Number pad always input number"),
	"1", NULL, NULL,  0, 1,
	NULL, numpadAlwaysNumber_set_callback,
	0, 0, 0,
	N_("Always input numbers when number keys from key pad is inputted."),
    },

    {G_TYPE_STRING, "inputStyle", "Editing", N_("Input Style"),
	"in candidate window", inputStyles, NULL,  0, 1,
	NULL, inputStyle_set_callback,
	MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE | MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION,
       	0, 0,
	N_("Input style determines where the preedit strings be shown and edited.\n"
		"\"in application window\": Preedit strings are edited on the target application window.\n"
		"\"in candidate window\": Preedit strings are edited on the candidate selection window."),
    },

    {G_TYPE_BOOLEAN, "plainZhuyin", "Selecting", N_("Plain Zhuyin mode"),
	"0", NULL, NULL, 0, 1,
	NULL, plainZhuyin_set_callback,
	0, 0, 0,
	N_("In plain Zhuyin mode, automatic candidate selection and related options are disabled or ignored."),
    },
    {G_TYPE_INT, "candPerPage", "Selecting", N_("Candidate per page"),
	"10", NULL, NULL,  8, 10,
	NULL, candPerPage_set_callback,
	0, 0, 0,
	N_("Number of candidate per page."),
    },
    {G_TYPE_BOOLEAN, "phraseChoiceRearward", "Selecting", N_("Choose phrases from backward"),
	"1", NULL, NULL,  0, 1,
	NULL, phraseChoiceRearward_set_callback,
	0, 0, 0,
	N_("Choose phrases from the back, without moving cursor."),
    },
    {G_TYPE_BOOLEAN, "spaceAsSelection", "Selecting", N_("Space to select"),
	"1", NULL, NULL,  0, 1,
	NULL, spaceAsSelection_set_callback,
	0, 0, 0,
	"Press Space to select the candidate.",
    },

    {G_TYPE_INVALID, "", "", "",
	"", NULL, NULL,  0, 0,
	NULL, NULL,
	0, 0, 0,
	NULL,
    },
};

/*============================================
 * Supporting functions
 */
#ifdef IBUS_CHEWING_MAIN

static guint keysym_KP_to_normal(guint keysym){
    if (keysym < IBUS_KP_0 || keysym > IBUS_KP_9){
	switch(keysym){
	    case IBUS_KP_Decimal:
		return IBUS_period;
	    case IBUS_KP_Add:
		return IBUS_plus;
	    case IBUS_KP_Subtract:
		return IBUS_minus;
	    case IBUS_KP_Multiply:
		return IBUS_asterisk;
	    case IBUS_KP_Divide:
		return IBUS_slash;
	    default:
		return 0;
	}
    }
    return keysym - IBUS_KP_0 + IBUS_0;
}

static int get_tone(ChewingKbType kbType, guint keysym){
    int i=0;
    if (keysym==' ')
	return 1;
    for(i=0;i<4;i++){
	if (toneKeys[kbType][i]==keysym){
	    return i+2;
	}
    }
    return -1;
}

static void add_tone(char *str, gint tone){
    switch(tone){
	case 2:
	    g_strlcat(str,"ˊ",ZHUYIN_BUFFER_SIZE);
	    break;
	case 3:
	    g_strlcat(str,"ˇ",ZHUYIN_BUFFER_SIZE);
	    break;
	case 4:
	    g_strlcat(str,"ˋ",ZHUYIN_BUFFER_SIZE);
	    break;
	case 5:
	    g_strlcat(str,"˙",ZHUYIN_BUFFER_SIZE);
	    break;
	default:
	    break;
    }
}

/*--------------------------------------------
 * Key modifier functions
 */
static guint keyModifier_get(Display *pDisplay){
    Window    root_retrun, child_retrun;
    int     root_x_return, root_y_return, win_x_return, win_y_return;
    guint     mask_return;
    XQueryPointer(pDisplay, DefaultRootWindow(pDisplay), &root_retrun, &child_retrun,
	    &root_x_return, &root_y_return, &win_x_return, &win_y_return, &mask_return );
    return mask_return;
}

const char numConst[]="0\0""1\0""2\0""3\0""4\0""5\0""6\0""7\0""8\0""9";
const char alphaConstL[]="a\0b\0c\0d\0e\0f\0g\0h\0i\0j\0k\0l\0m\0n\0o\0p\0q\0r\0s\0t\0u\0v\0w\0x\0y\0z";
const char alphaConstU[]="A\0B\0C\0D\0E\0F\0G\0H\0I\0J\0K\0L\0M\0N\0O\0P\0Q\0R\0S\0T\0U\0V\0W\0X\0Y\0Z";

const char *keyName_get(guint keyval){
    switch (keyval){
	case IBUS_Return:
	    return "Return";
	case IBUS_KP_Enter:
	    return "KP_Return";
	case IBUS_Escape:
	    return "Escape";
	case IBUS_BackSpace:
	    return "BackSpace";
	case IBUS_Delete:
	    return "Delete";
	case IBUS_KP_Delete:
	    return "KP_Delete";
	case IBUS_space:
	    return "space";
	case IBUS_KP_Space:
	    return "KP_space";
	case IBUS_Page_Up:
	    return "Page_Up";
	case IBUS_KP_Page_Up:
	    return "KP_Page_Up";
	case IBUS_Page_Down:
	    return "Page_Down";
	case IBUS_KP_Page_Down:
	    return "KP_Page_Down";
	case IBUS_Up:
	    return "Up";
	case IBUS_KP_Up:
	    return "KP_Up";
	case IBUS_Down:
	    return "Down";
	case IBUS_KP_Down:
	    return "KP_Down";
	case IBUS_Left:
	    return "Left";
	case IBUS_KP_Left:
	    return "KP_Left";
	case IBUS_Right:
	    return "Right";
	case IBUS_KP_Right:
	    return "KP_Right";
	case IBUS_Home:
	    return "Home";
	case IBUS_KP_Home:
	    return "KP_Home";
	case IBUS_End:
	    return "End";
	case IBUS_KP_End:
	    return "KP_End";
	case IBUS_Tab:
	    return "Table";
	case IBUS_Caps_Lock:
	    return "Caps";
	default:
	    if (keyval>='0' && keyval<='9'){
		return &numConst[(keyval-'0')*2];
	    }else if (keyval>='a' && keyval<='z'){
		return &alphaConstL[(keyval-'a')*2];
	    }else if (keyval>='A' && keyval<='Z'){
		return &alphaConstU[(keyval-'A')*2];
	    }
	    break;
    }
    return "Others";
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

#endif

