/**
 * ibus_chewing_engine_keycode_to_keysym:
 *
 * Convert keycode to keysym.
 */
guint ibus_chewing_engine_keycode_to_keysym(IBusChewingEngine *self, guint keysym,  guint keycode, guint modifiers){
    /* Get system layout */
    GValue gValue={0};
    gboolean useSysKeyLayout=TRUE;
    if (ibus_chewing_config_get_value(self->config, "general", "use_system_keyboard_layout", &gValue)){
	useSysKeyLayout=g_value_get_boolean(&gValue);
    }

    guint kSym;
    if (useSysKeyLayout && (!chewing_get_ChiEngMode(self->context))){
	// English mode.
	kSym=keysym;
    }else{
	/* ibus_keymap_lookup_keysym only handles keycode under 256 */
	if ((kSym=ibus_keymap_lookup_keysym(self->keymap_us,keycode,modifiers))==IBUS_VoidSymbol){
	    kSym=keysym;
	}
    }
    return kSym;
}

gboolean ibus_chewing_engine_process_key_event(IBusEngine *engine,
	guint keysym,  guint keycode, guint modifiers){
    gboolean result=TRUE;
    if (modifiers & IBUS_RELEASE_MASK){
	/* Skip release event */
	return TRUE;
    }
    IBusChewingEngine *self=IBUS_CHEWING_ENGINE(engine);
    guint kSym=ibus_chewing_engine_keycode_to_keysym(self,keysym, keycode, modifiers);

    G_DEBUG_MSG(2,"***[I2] process_key_event(-, %x(%s), %x, %x) orig keysym=%x... proceed.",kSym, keyName_get(kSym), keycode, modifiers,keysym);
    guint state= modifiers & (IBUS_SHIFT_MASK | IBUS_CONTROL_MASK | IBUS_MOD1_MASK);
    self->_priv->key_last=kSym;
    if (state==0){
	guint kSym_tmp=keysym_KP_to_normal(kSym);
	if (kSym_tmp){
	    G_DEBUG_MSG(3,"***[I3] process_key_event(): %x is from keypad.", kSym_tmp);
	    /* Is keypad key */
	    if ((self->chewingFlags & CHEWING_FLAG_NUMPAD_ALWAYS_NUMBER) && chewing_get_ChiEngMode(self->context)){
		chewing_set_ChiEngMode(self->context, 0);
		self_handle_Default(self,kSym_tmp,FALSE);
		chewing_set_ChiEngMode(self->context,CHINESE_MODE);
	    }else{
		/* Convert kp numbers to normal */
		self_handle_Default(self,kSym_tmp,FALSE);
	    }
	}else{
	    switch (kSym){
		case IBUS_Return:
		case IBUS_KP_Enter:
		    ibus_chewing_engine_set_status_flag(self,ENGINE_STATUS_NEED_COMMIT);
		    chewing_handle_Enter(self->context);
		    break;
		case IBUS_Escape:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    chewing_handle_Esc(self->context);
		    break;
		case IBUS_BackSpace:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    chewing_handle_Backspace(self->context);
		    break;
		case IBUS_Delete:
		case IBUS_KP_Delete:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    chewing_handle_Del(self->context);
		    break;
		case IBUS_space:
		case IBUS_KP_Space:
		    if (self->chewingFlags & CHEWING_FLAG_PLAIN_ZHUYIN) {
			if (chewing_cand_TotalChoice(self->context) == 0) {
			    chewing_handle_Space(self->context);
			}
		    } else {
			/**
			 * Fix for space in Temporary English mode.
			 */
			chewing_handle_Space(self->context);
		    }
		    if (self->inputMode==CHEWING_INPUT_MODE_SELECTION_DONE ||
			    self->inputMode==CHEWING_INPUT_MODE_BYPASS )
			ibus_chewing_engine_set_status_flag(self,ENGINE_STATUS_NEED_COMMIT);
		    break;
		case IBUS_Page_Up:
		case IBUS_KP_Page_Up:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    IBUS_ENGINE_GET_CLASS(engine)->page_up(engine);
		    break;
		case IBUS_Page_Down:
		case IBUS_KP_Page_Down:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    IBUS_ENGINE_GET_CLASS(engine)->page_down(engine);
		    break;
		case IBUS_Up:
		case IBUS_KP_Up:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    IBUS_ENGINE_GET_CLASS(engine)->cursor_up(engine);
		    break;
		case IBUS_Down:
		case IBUS_KP_Down:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    IBUS_ENGINE_GET_CLASS(engine)->cursor_down(engine);
		    break;
		case IBUS_Left:
		case IBUS_KP_Left:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    chewing_handle_Left(self->context);
		    break;
		case IBUS_Right:
		case IBUS_KP_Right:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    chewing_handle_Right(self->context);
		    break;
		case IBUS_Home:
		case IBUS_KP_Home:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    chewing_handle_Home(self->context);
		    break;
		case IBUS_End:
		case IBUS_KP_End:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    chewing_handle_End(self->context);
		    break;
		case IBUS_Tab:
		    if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
			return FALSE;
		    chewing_handle_Tab(self->context);
		    break;
		case IBUS_Caps_Lock:
		    /* When Chi->Eng with incomplete character */
		    if (chewing_get_ChiEngMode(self->context) && !chewing_zuin_Check(self->context)){
			/* chewing_zuin_Check==0 means incomplete character */
			/* Send a space to finish the character */
			chewing_handle_Space(self->context);
		    }
		    chewing_handle_Capslock(self->context);
		    self_refresh_property(self,"chewing_chieng_prop");
		    break;
		case IBUS_Shift_L:
		case IBUS_Shift_R:
		    /* Some QT application will sneak these through */
		    return FALSE;
		case IBUS_Alt_L:
		case IBUS_Alt_R:
		    /* Some QT application will sneak these through */
		    return FALSE;
		case IBUS_Control_L:
		case IBUS_Control_R:
		    /* Some QT application will sneak these through */
		    return FALSE;
		default:
		    self_handle_Default(self,kSym,FALSE);
		    break;
	    }
	}
    }else if (state==IBUS_SHIFT_MASK){
	switch(kSym){
	    case IBUS_Return:
	    case IBUS_KP_Enter:
		/* Same with Shift Enter and Shift KP_Enter */
		ibus_chewing_engine_set_status_flag(self,ENGINE_STATUS_NEED_COMMIT);
		chewing_handle_Enter(self->context);
		break;
	    case IBUS_Left:
		if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
		    return FALSE;
		chewing_handle_ShiftLeft(self->context);
		break;
	    case IBUS_Right:
		if (self->inputMode==CHEWING_INPUT_MODE_BYPASS)
		    return FALSE;
		chewing_handle_ShiftRight(self->context);
		break;
	    case IBUS_Up:
	    case IBUS_KP_Up:
	    case IBUS_Down:
	    case IBUS_KP_Down:
	    case IBUS_Page_Up:
	    case IBUS_KP_Page_Up:
	    case IBUS_Page_Down:
	    case IBUS_KP_Page_Down:
	    case IBUS_Home:
	    case IBUS_End:
		if (self->_priv->statusFlags & ENGINE_STATUS_NEED_COMMIT)
		    self_force_commit(self);
		return FALSE;
	    case IBUS_space:
	    case IBUS_KP_Space:
		chewing_handle_ShiftSpace(self->context);
		chewing_set_ShapeMode(self->context, !chewing_get_ShapeMode(self->context));
		self_refresh_property(self,"chewing_alnumSize_prop");
		break;
	    default:
		if (kSym>127 || kSym<0){
		    /* Special keys, must let it through */
		    return FALSE;
		}
		self_handle_Default(self,kSym,TRUE);
		break;
	}
    }else if (state==IBUS_CONTROL_MASK){
	if (kSym>=IBUS_0 && kSym<=IBUS_9){
	    chewing_handle_CtrlNum(self->context,kSym);
//	}else if (kSym==IBUS_v || kSym==IBUS_V){
//	    chewing_handle_Right(self->context);
	}else{
	    result=FALSE;
	}

    }else{
	result=FALSE;
    }
    if (!result){
	return FALSE;
    }
    return self_update(self);
}

void ibus_chewing_engine_handle_Default(IBusChewingEngine *self, guint keyval, gboolean shiftPressed){
    G_DEBUG_MSG(2,"[I2] handle_Default(-,%u) plainZhuyin=%s inputMode=%d",
	    keyval,(self->chewingFlags & CHEWING_FLAG_PLAIN_ZHUYIN)? "TRUE": "FALSE",self->inputMode);
    ibus_chewing_engine_set_status_flag(self, ENGINE_STATUS_NEED_COMMIT);
#ifdef EASY_SYMBOL_INPUT_WORK_AROUND
    if (self->chewingFlags & CHEWING_FLAG_EASY_SYMBOL_INPUT){
	/* If shift is pressed, turn on the  easySymbolInput, turn off
	 * otherwise
	 */
	chewing_set_easySymbolInput(self->context,(shiftPressed)? 1:0);
    }
#endif
    if (self->chewingFlags & CHEWING_FLAG_FORCE_LOWERCASE_ENGLISH){
	if (isupper(keyval) && !shiftPressed){
	    keyval=tolower(keyval);
	}else if (islower(keyval) && shiftPressed){
	    keyval=toupper(keyval);
	}
    }
    chewing_handle_Default(self->context,keyval);
    if (self->chewingFlags & CHEWING_FLAG_PLAIN_ZHUYIN){
	if (self_is_selectKey(self,self->_priv->key_last) &&
		self->inputMode==CHEWING_INPUT_MODE_SELECTING){
	    chewing_handle_Enter(self->context);
	    self->inputMode= CHEWING_INPUT_MODE_SELECTION_DONE;
	}
    }
}

/*===================================================
 * Mouse events
 */
void ibus_chewing_engine_candidate_clicked(IBusEngine *engine, guint index, guint button, guint state){
    G_DEBUG_MSG(2,"***[I2] candidate_clicked(-, %u, %u, %u) ... proceed.", index, button, state);
    IBusChewingEngine *self=IBUS_CHEWING_ENGINE(engine);
    if (index >= chewing_get_candPerPage(self->context) || index <0) {
	G_DEBUG_MSG(3,"[I3]  candidate_clicked() index out of ranged");
	return;
    }
    if (self->inputMode==CHEWING_INPUT_MODE_SELECTING){
	self->_priv->key_last=(guint) self->selKeys[index];
	ibus_chewing_engine_handle_Default(self, self->_priv->key_last, FALSE);
	self_update(self);
    } else {
	G_DEBUG_MSG(3,"[I3] candidate_clicked() ... Wrong mode: %u", self->inputMode);
    }
}

void ibus_chewing_engine_property_activate(IBusEngine *engine, const gchar  *prop_name, guint  prop_state){
    G_DEBUG_MSG(3,"[I3] property_activate(-, %s, %u)", prop_name, prop_state);
    Self *self=SELF(engine);
    gboolean needRefresh=TRUE;
    if (strcmp(prop_name,"chewing_chieng_prop")==0){
	/* Toggle Chinese <-> English */
	chewing_set_ChiEngMode(self->context, !chewing_get_ChiEngMode(self->context));
    }else if (strcmp(prop_name,"chewing_alnumSize_prop")==0){
	/* Toggle Full <-> Half */
	chewing_set_ShapeMode(self->context, !chewing_get_ShapeMode(self->context));
    }else if (strcmp(prop_name,"chewing_settings_prop")==0){
#if IBUS_CHECK_VERSION(1, 4, 0)
	if (ibus_property_get_state(self->settings_prop)==PROP_STATE_UNCHECKED)
#else
	if (self->settings_prop->state==PROP_STATE_UNCHECKED)
#endif
	{
	    if (gtk_dialog_run(GTK_DIALOG(self->setting_dialog))==GTK_RESPONSE_OK){
		self_save_config_all(self);
	    }
	    gtk_widget_hide(self->setting_dialog);
#if IBUS_CHECK_VERSION(1, 4, 0)
		ibus_property_set_state(self->settings_prop,PROP_STATE_UNCHECKED);
#else
	    self->settings_prop->state=PROP_STATE_UNCHECKED;
#endif
	}
    }else{
	G_DEBUG_MSG(3,"[I3]  property_activate(-, %s, %u) not recognized",prop_name, prop_state);
	needRefresh=FALSE;
    }
    if (needRefresh)
	self_refresh_property(self,prop_name);
}
