const gchar *toneKeys[] = {
    "6347",			//Default
    "jfds",			//hsu
    "uiop",			//ibm
    "zaq1",			//gin-yieh
    "4321",			//eten
    "kjfd",			//eten26
    "6347",			//dvorak
    "thdn",			//dvorak_hsu
    "yert",			//dachen_26
    "1234",			//hanyu
    NULL
};

/*============================================
 * Supporting functions
 */
#ifdef IBUS_CHEWING_MAIN

static void ibus_chewing_log_handler(const gchar *log_domain, 
	GLogLevelFlags log_level, const gchar *message, gpointer user_data){
    GString *str=g_string_new(NULL);
    IBusChewingEngine *engine=(IBusChewingEngine *) user_data;
    if (log_level & G_LOG_LEVEL_ERROR){
	g_string_printf(str, "%s [EE] %s\n", log_domain, message);
    }else if (log_level & G_LOG_LEVEL_CRITICAL){
	g_string_printf(str, "%s [CC] %s\n", log_domain, message);
    }else if (log_level & G_LOG_LEVEL_WARNING){
	g_string_printf(str, "%s [WW] %s\n", log_domain, message);
    }else if (log_level & G_LOG_LEVEL_MESSAGE){
	g_string_printf(str, "%s [MM] %s\n", log_domain, message);
    }else if (log_level & G_LOG_LEVEL_INFO){
	g_string_printf(str, "%s [II] %s\n", log_domain, message);
    }else if (log_level & G_LOG_LEVEL_DEBUG){
	g_string_printf(str, "%s [DD] %s\n", log_domain, message);
    }else{
	g_string_printf(str, "%s [%d] %s\n", log_domain, log_level,message);
    }
    if (str->len){
	if (engine->logFile){
	    fputs(str->str, engine->logFile);
	}else{
	    fputs(str->str, stderr);
	}    
    }
    g_string_free(str,TRUE);
    return;
}

static gboolean ibus_chewing_property_get_state(IBusProperty *prop){
#if IBUS_CHECK_VERSION(1, 4, 0)
    return ibus_property_get_state(prop);
#else
    return prop->state;
#endif
}

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
#if 0
static guint keyModifier_get(Display *pDisplay){
    Window    root_retrun, child_retrun;
    int     root_x_return, root_y_return, win_x_return, win_y_return;
    guint     mask_return;
    XQueryPointer(pDisplay, DefaultRootWindow(pDisplay), &root_retrun, &child_retrun,
	    &root_x_return, &root_y_return, &win_x_return, &win_y_return, &mask_return );
    return mask_return;
}
#endif

const char numConst[]="0\0""1\0""2\0""3\0""4\0""5\0""6\0""7\0""8\0""9";
const char alphaConstL[]="a\0b\0c\0d\0e\0f\0g\0h\0i\0j\0k\0l\0m\0n\0o\0p\0q\0r\0s\0t\0u\0v\0w\0x\0y\0z";
const char alphaConstU[]="A\0B\0C\0D\0E\0F\0G\0H\0I\0J\0K\0L\0M\0N\0O\0P\0Q\0R\0S\0T\0U\0V\0W\0X\0Y\0Z";

const char *keyName_get(guint keyval){
    switch (keyval){
	case IBUS_Return:
	    return "Return";
	case IBUS_KP_Enter:
	    return "KP_Enter";
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
	    return "Tab";
	case IBUS_Caps_Lock:
	    return "Caps";
	case IBUS_Shift_L:
	    return "Shift_L";
	case IBUS_Shift_R:
	    return "Shift_R";
	case IBUS_Alt_L:
	    return "Alt_L";
	case IBUS_Alt_R:
	    return "Alt_R";
	case IBUS_Control_L:
	    return "Control_L";
	case IBUS_Control_R:
	    return "Control_R";
	case IBUS_Super_L:
	    return "Super_L";
	case IBUS_Super_R:
	    return "Super_R";
	case IBUS_ISO_Lock:
	    return "ISO_Lock";
	case IBUS_ISO_Level3_Lock:
	    return "ISO_Level3_Lock";
	case IBUS_ISO_Level3_Shift:
	    return "ISO_Level3_Shift";
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

#define CAPS_LOCK_MASK 2
static gboolean is_caps_led_on(Display *pDisplay){
    XKeyboardState retState;
    XGetKeyboardControl(pDisplay,&retState);
    return (retState.led_mask & 1) ? TRUE : FALSE;
}

static void set_caps_led(gboolean on,Display *pDisplay){
    XKeyboardControl control;
    control.led_mode = (on)? LedModeOn: LedModeOff;
    control.led = CAPS_LOCK_MASK;
    guint flags=(on)? CAPS_LOCK_MASK: 0;
    XChangeKeyboardControl(pDisplay, KBLedMode, &control);
    XkbLockModifiers(pDisplay, XkbUseCoreKbd, control.led, flags);
    XFlush(pDisplay);
}

#if 0
/*
 * From send_fake_key_event() event.c gcin
 */
static void key_send_fake_event(KeySym key, Display *pDisplay)
{
    KeyCode keyCode = XKeysymToKeycode(pDisplay, key);
    IBUS_CHEWING_LOG(INFO,"send_fake_key_event(%lx,-), keyCode=%x",key,keyCode);
    XTestFakeKeyEvent(pDisplay, keyCode, True, CurrentTime);
    XTestFakeKeyEvent(pDisplay, keyCode, False, CurrentTime);

}
#endif

#endif

