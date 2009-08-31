#ifndef IBUS_1_1
gboolean ibus_chewing_engine_process_key_event_1_2(IBusEngine *engine,
	guint keysym_ignore,  guint  keycode,   guint  modifiers){
    if (modifiers & IBUS_RELEASE_MASK){
	/* Skip release event */
	return TRUE;
    }
    IBusChewingEngine *self=IBUS_CHEWING_ENGINE(engine);
    guint keysym=ibus_keymap_lookup_keysym (self->keymap_us,keycode,modifiers);
    return ibus_chewing_engine_process_key_event(engine, keysym, modifiers);
}
#endif /* IBUS_1_1 */

gboolean ibus_chewing_engine_process_key_event(IBusEngine *engine,
	guint keysym, guint  modifiers){
    gboolean result=TRUE;
    if (modifiers & IBUS_RELEASE_MASK){
	/* Skip release event */
	return TRUE;
    }
    IBusChewingEngine *self=IBUS_CHEWING_ENGINE(engine);
    G_DEBUG_MSG(3,"ibus_chewing_engine_process_key_event(-, %u(%s), %u) ... proceed.",keysym, keyName_get(keysym), modifiers);
    guint state= modifiers & (IBUS_SHIFT_MASK | IBUS_CONTROL_MASK | IBUS_MOD1_MASK);
    self->_priv->key_last=keysym;
    if (state==0){
	guint keysym_tmp=keysym_KP_to_normal(keysym);
	if (keysym_tmp){
	    /* Is keypad key */
	    if (self->numpadAlwaysNumber && chewing_get_ChiEngMode(self->context)){
		chewing_set_ChiEngMode(self->context, 0);
		self_handle_Default(self,keysym_tmp,FALSE);
		chewing_set_ChiEngMode(self->context,CHINESE_MODE);
	    }else{
		/* Convert kp numbers to normal */
		self_handle_Default(self,keysym_tmp,FALSE);
	    }
	}else{
	    switch (keysym){
		case IBUS_Return:
		case IBUS_KP_Enter:
		    chewing_handle_Enter(self->context);
		    break;
		case IBUS_Escape:
		    chewing_handle_Esc(self->context);
		    break;
		case IBUS_BackSpace:
		    chewing_handle_Backspace(self->context);
		    break;
		case IBUS_Delete:
		case IBUS_KP_Delete:
		    chewing_handle_Del(self->context);
		    break;
		case IBUS_space:
		case IBUS_KP_Space:
		    chewing_handle_Space(self->context);
		    break;
		case IBUS_Page_Up:
		case IBUS_KP_Page_Up:
		    IBUS_ENGINE_GET_CLASS(engine)->page_up(engine);
		    break;
		case IBUS_Page_Down:
		case IBUS_KP_Page_Down:
		    IBUS_ENGINE_GET_CLASS(engine)->page_down(engine);
		    break;
		case IBUS_Up:
		case IBUS_KP_Up:
		    IBUS_ENGINE_GET_CLASS(engine)->cursor_up(engine);
		    break;
		case IBUS_Down:
		case IBUS_KP_Down:
		    IBUS_ENGINE_GET_CLASS(engine)->cursor_down(engine);
		    break;
		case IBUS_Left:
		case IBUS_KP_Left:
		    chewing_handle_Left(self->context);
		    break;
		case IBUS_Right:
		case IBUS_KP_Right:
		    chewing_handle_Right(self->context);
		    break;
		case IBUS_Home:
		case IBUS_KP_Home:
		    chewing_handle_Home(self->context);
		    break;
		case IBUS_End:
		case IBUS_KP_End:
		    chewing_handle_End(self->context);
		    break;
		case IBUS_Tab:
		    chewing_handle_Tab(self->context);
		    break;
		case IBUS_Caps_Lock:
		    chewing_handle_Capslock(self->context);
		    self_refresh_property(self,"chewing_chieng_prop");
		    break;
		default:
		    self_handle_Default(self,keysym,FALSE);
		    break;
	    }
	}
    }else if (state==IBUS_SHIFT_MASK){
	switch(keysym){
	    case IBUS_Shift_L:
		chewing_handle_ShiftLeft(self->context);
		break;
	    case IBUS_Shift_R:
		chewing_handle_ShiftRight(self->context);
		break;
	    case IBUS_space:
	    case IBUS_KP_Space:
		chewing_handle_ShiftSpace(self->context);
		chewing_set_ShapeMode(self->context, !chewing_get_ShapeMode(self->context));
		self_refresh_property(self,"chewing_alnumSize_prop");
		break;
	    default:
		self_handle_Default(self,keysym,TRUE);
		break;
	}
    }else if (state==IBUS_CONTROL_MASK){
	if (keysym>=IBUS_0 && keysym<=IBUS_9){
	    chewing_handle_CtrlNum(self->context,keysym);
	}else if (keysym==IBUS_v || keysym==IBUS_V){
	    chewing_handle_Right(self->context);
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

