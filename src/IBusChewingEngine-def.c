
const int selKeys_default[10]={'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
const char *kbType_ids[]={
    "default",
    "hsu",
    "ibm",
    "gin_yieh",
    "eten",
    "eten26",
    "dvorak",
    "dvorak_hsu",
    "dachen_26",
    "hanyu",
    NULL
};

#define SELKEYS_ARRAY_SIZE 7
const char *selKeys_array[SELKEYS_ARRAY_SIZE+1]={
    "1234567890",
    "asdfghjkl;",
    "asdfzxcv89",   
    "asdfjkl789",
    "aoeuhtn789",
    "1234qweras",
    "Custom",
    NULL
};


typedef void (*ChewingSettingFunc)(ChewingContext *ctx, int mode);
typedef int (*ChewingGettingFunc)(ChewingContext *ctx);

typedef struct{
    GConfValueType type;
    gchar key[30];
    gchar defaultValue[30];
    ChewingSettingFunc func;
    ChewingGettingFunc getFunc;
} ChewingParameter;

const ChewingParameter parameters[]={
    {GCONF_VALUE_STRING, "KBType",    "default", NULL,NULL},
    {GCONF_VALUE_STRING, "selKeys","1234567890",NULL,NULL},
    {GCONF_VALUE_BOOL,   N_("autoShiftCur"),"0", chewing_set_autoShiftCur,chewing_get_autoShiftCur},
    {GCONF_VALUE_BOOL, N_("addPhraseDirection"),"0",chewing_set_addPhraseDirection,chewing_get_addPhraseDirection},
    {GCONF_VALUE_INT, N_("candPerPage"),"10",chewing_set_candPerPage,chewing_get_candPerPage},
    {GCONF_VALUE_BOOL, N_("easySymbolInput"),"0",chewing_set_easySymbolInput,chewing_get_easySymbolInput},
    {GCONF_VALUE_BOOL, N_("escCleanAllBuf"),"0",chewing_set_escCleanAllBuf,chewing_get_escCleanAllBuf},
    {GCONF_VALUE_INT, N_("hsuSelKeyType"),"1",chewing_set_hsuSelKeyType,chewing_get_hsuSelKeyType},
    {GCONF_VALUE_INT, N_("maxChiSymbolLen"),"16",chewing_set_maxChiSymbolLen,chewing_get_maxChiSymbolLen},
    {GCONF_VALUE_BOOL, N_("phraseChoiceRearward"),"1",chewing_set_phraseChoiceRearward,chewing_get_phraseChoiceRearward},
    {GCONF_VALUE_BOOL, N_("spaceAsSelection"),"1",chewing_set_spaceAsSelection,chewing_get_spaceAsSelection},
    {GCONF_VALUE_INVALID, "","", NULL}
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

static int property_get_index(const gchar *prop_name){
    int i=0;
    size_t prefix_len=strlen("chewing_");
    const gchar *ptr=prop_name+prefix_len;
    size_t len=strlen(ptr)-5;
    for(i=0;parameters[i].type!=GCONF_VALUE_INVALID;i++){
	if (strncmp(ptr,parameters[i].key,len)==0){
	    return i;
	}
    }
    return -1;
}


static ChewingKbType KBType_property_get_index(const gchar *prop_name){
    ChewingKbType i=0;
    size_t prefix_len=strlen("chewing_KBType_");
    const gchar *ptr=prop_name+prefix_len;
    size_t len=strlen(ptr)-5;
    for(i=0;kbType_ids[i]!=NULL;i++){
	if (strncmp(ptr,kbType_ids[i],len)==0){
	    return i;
	}
    }
    return CHEWING_KBTYPE_INVALID;
}

static ChewingKbType kbType_id_get_index(const gchar *kbType_id){
    ChewingKbType i=0;
    for(i=0;kbType_ids[i]!=NULL;i++){
	if (strcmp(kbType_id,kbType_ids[i])==0){
	    return i;
	}
    }
    return CHEWING_KBTYPE_INVALID;
}


static IBusText** kbTypes_to_iBusTexts(){
    IBusText **kbTypeTexts=g_new(IBusText *,10+1);
    int i=0;
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("Default"));
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("Hsu"));
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("IBM"));
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("Gin-Yieh"));
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("ETen"));
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("ETen 26 keys"));
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("Dvorak"));
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("Dvorak Hsu"));
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("Dachen CP26"));
    kbTypeTexts[i++]=ibus_text_new_from_static_string(_("Hanyu"));
    kbTypeTexts[i]=NULL;
    return kbTypeTexts;
}

static IBusText** selKeys_to_iBusTexts(){
    IBusText **selKeysTexts=g_new(IBusText *,SELKEYS_ARRAY_SIZE+1);
    int i;
    for(i=0;i<SELKEYS_ARRAY_SIZE-1;i++){
	selKeysTexts[i]=ibus_text_new_from_static_string(selKeys_array[i]);
    }
    selKeysTexts[i++]=ibus_text_new_from_static_string(_("Custom"));
    selKeysTexts[i]=NULL;
    return selKeysTexts;
}

static int selKeys_property_get_index(const gchar *prop_name){
    int i=0;
    size_t prefix_len=strlen("chewing_selKeys_");
    const gchar *ptr=prop_name+prefix_len;
    size_t len=strlen(ptr)-5;
    for(i=0;i<SELKEYS_ARRAY_SIZE;i++){
	if (strncmp(ptr,selKeys_array[i],len)==0){
	    return i;
	}
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
static void miscOption_array_show(gpointer data,  gpointer user_data){
    IBusProperty *prop = (IBusProperty *) data;
    IBusChewingEngine *self= (IBusChewingEngine *) user_data;
    IBUS_ENGINE_GET_CLASS(self)->property_show(IBUS_ENGINE(self),prop->key);
}

static void miscOption_array_hide(gpointer data,  gpointer user_data){
    IBusProperty *prop = (IBusProperty *) data;
    IBusChewingEngine *self= (IBusChewingEngine *) user_data;
    IBUS_ENGINE_GET_CLASS(self)->property_hide(IBUS_ENGINE(self),prop->key);
}

//static KeySym modifiers[]={
//    IBUS_Scroll_Lock,
//    IBUS_Num_Lock
//    IBUS_Caps_Lock,
//    IBUS_Shift_L,
//    IBUS_Shift_R,
//    IBUS_Control_L,
//    IBUS_Control_R,
//    IBUS_Shift_Lock,
//    IBUS_Meta_L,
//    IBUS_Meta_R,
//    IBUS_Alt_L, 
//    IBUS_Alt_R,
//    IBUS_Super_L,
//    IBUS_Super_R,
//    IBUS_Hyper_L,
//    IBUS_Hyper_R,
//    IBUS_VoidSymbol
//};

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
    G_DEBUG_MSG("key_sent_fake_event(%lx,-), keyCode=%x",key,keyCode);
    XTestFakeKeyEvent(pDisplay, keyCode, True, CurrentTime);
    XTestFakeKeyEvent(pDisplay, keyCode, False, CurrentTime);
    
}

//static gboolean key_ignore_skip(guint  keyval){
//    switch(keyval){
//	case IBUS_Caps_Lock:
//	    return TRUE;
//	default:
//	    break;
//
//    }
//    return FALSE;
//}
