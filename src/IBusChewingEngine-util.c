
/* Apply IBusText attributes */
static IBusText *decorate_preedit(IBusChewingPreEdit * icPreEdit, EngineFlag statusFlags)
{
    gchar *preEdit = ibus_chewing_pre_edit_get_pre_edit(icPreEdit);
    IBusText *iText =
	g_object_ref_sink(ibus_text_new_from_string(preEdit));

    gint chiSymbolCursor = chewing_cursor_Current(icPreEdit->context);
    IBUS_CHEWING_LOG(DEBUG,
		     "decorate_preedit() cursor=%d preEdit=%s",
		     chiSymbolCursor, preEdit);


    gint charLen = (gint) g_utf8_strlen(preEdit, -1);
    gint cursorRight  = chiSymbolCursor + icPreEdit->bpmfLen;

    IBUS_CHEWING_LOG(DEBUG,
		     "decorate_preedit() charLen=%d cursorRight=%d",
		     charLen, cursorRight);

    IntervalType it;
    chewing_interval_Enumerate(icPreEdit->context);
    /* Add double lines on chewing interval that contains cursor */
    /* Add single line on other chewing interval */
    while(chewing_interval_hasNext(icPreEdit->context)){
	chewing_interval_Get(icPreEdit->context, &it);
	if (it.from <= chiSymbolCursor && chiSymbolCursor <= it.to){
	    ibus_text_append_attribute(iText,
		    IBUS_ATTR_TYPE_UNDERLINE,
		    IBUS_ATTR_UNDERLINE_DOUBLE,
		    it.from, it.to+1);

	}else{
	    ibus_text_append_attribute(iText,
		    IBUS_ATTR_TYPE_UNDERLINE,
		    IBUS_ATTR_UNDERLINE_SINGLE,
		    it.from, it.to+1);
	}

    }
   
   if(!mkdg_has_flag(statusFlags, ENGINE_FLAG_CAP_SURROUNDING_TEXT)){
       /* Cannot change color when if the client is not capable
	* of showing surrounding text
	*/
       return iText;
   } 

    /* Show current cursor in red */
    ibus_text_append_attribute(iText, IBUS_ATTR_TYPE_BACKGROUND,
			       0x00ff0000, chiSymbolCursor,
			       chiSymbolCursor + 1);

    return iText;
}

