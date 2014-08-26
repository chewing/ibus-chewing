const gchar *page_labels[] = {
    N_("Editing"),
    N_("Selecting"),
    N_("Keyboard"),
    NULL
};

const gchar *button_labels[] = {
    GTK_STOCK_SAVE,
    NULL
};

GtkResponseType button_responses[] = {
    GTK_RESPONSE_OK,
};

static PropertyContext *propertyContext_new(PropertySpec * spec,
                                            gpointer parent,
					    gpointer userData)
{
    PropertyContext *ctx = g_new(PropertyContext, 1);
    ctx->spec = spec;
    ctx->parent = parent;
    ctx->userData = userData;
    return ctx;
}


static void propertyContext_free(gpointer ctx_ptr)
{
    PropertyContext *ctx = (PropertyContext *) ctx_ptr;
    g_free(ctx);
}

static void listStore_append(GtkListStore * listStore, const gchar * str,
			     const gchar * translationContext,
			     MakerDialogPropertyFlags propertyFlags)
{
    GtkTreeIter iter;
    gtk_list_store_append(listStore, &iter);

    if (propertyFlags & MAKER_DIALOG_PROPERTY_FLAG_HAS_TRANSLATION) {
	if (translationContext
	    || propertyFlags &
	    MAKER_DIALOG_PROPERTY_FLAG_TRANSLATION_WITH_CONTEXT) {
	    IBUS_CHEWING_LOG(DEBUG, "listStore_append() str=%s, _(str)=%s", str,
			     g_dpgettext2(NULL, translationContext, str));
	    gtk_list_store_set(listStore, &iter, 0, str, 1,
			       g_dpgettext2(NULL, translationContext, str),
			       -1);
	} else {
	    IBUS_CHEWING_LOG(DEBUG, "listStore_append() str=%s, _(str)=%s", str, _(str));
	    gtk_list_store_set(listStore, &iter, 0, str, 1, _(str), -1);
	}
    } else {
	IBUS_CHEWING_LOG(DEBUG, "listStore_append() str=%s", str);
	gtk_list_store_set(listStore, &iter, 0, str, -1);
    }

}

static gint listStore_find_string(GtkListStore * listStore,
				  const gchar * str,
				  const gchar * translationContext,
				  MakerDialogPropertyFlags propertyFlags)
{
    g_assert(str);
    IBUS_CHEWING_LOG(INFO, "listStore_find_string(%s,%u)", str,
		     propertyFlags);
    int i = 0, index = -1;
    GtkTreeIter iter;
    GValue gValue = { 0 };
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listStore), &iter)) {
	do {
	    gtk_tree_model_get_value(GTK_TREE_MODEL(listStore), &iter, 0,
				     &gValue);
	    if (strcmp(str, g_value_get_string(&gValue)) == 0) {
		index = i;
		break;
	    }
	    i++;
	    g_value_unset(&gValue);
	} while (gtk_tree_model_iter_next
		 (GTK_TREE_MODEL(listStore), &iter));
    }
    if (index < 0
	&& !(propertyFlags & MAKER_DIALOG_PROPERTY_FLAG_INEDITABLE)) {
	/* Add new item */
	listStore_append(listStore, str, translationContext,
			 propertyFlags);
	index = i;
    }
    IBUS_CHEWING_LOG(DEBUG, "listStore_find_string(%s,%u) index=%d", str,
		     propertyFlags, index);
    return index;
}

static gint combo_find_string_index(GtkComboBox * combo, const gchar * str,
				    const gchar * translationContext,
				    MakerDialogPropertyFlags propertyFlags)
{
    GtkListStore *listStore =
	GTK_LIST_STORE(gtk_combo_box_get_model(combo));
    return listStore_find_string(listStore, str, translationContext,
				 propertyFlags);
}

static const gchar *combo_get_active_text(GtkComboBox * combo,
					  GValue * gValue)
{
    GtkTreeIter iter;
    if (!gtk_combo_box_get_active_iter(combo, &iter)) {
	return NULL;
    }
    GtkListStore *listStore =
	GTK_LIST_STORE(gtk_combo_box_get_model(combo));
    gtk_tree_model_get_value(GTK_TREE_MODEL(listStore), &iter, 0, gValue);
    return g_value_get_string(gValue);
}


static void propList_free_deep_callback(gpointer data, gpointer userData)
{
    propertyContext_free(data);
}

static gchar *widget_get_id(gchar * buffer, gint buffer_size,
			    const gchar * widget_label,
			    const gchar * widget_type)
{
    g_snprintf(buffer, buffer_size, "%s%s%s%s",
	       WIDGET_ID_PREFIX, widget_label,
	       (widget_type) ? "_" : "", (widget_type) ? widget_type : "");
    return buffer;
}

/*===== Widget Callback function wraps =====*/

static void on_comboBox_changed_wrap(GtkComboBox * comboBox,
				     gpointer userData)
{
    PropertyContext *ctx = (PropertyContext *) userData;
    GValue value = { 0 };
    combo_get_active_text(comboBox, &value);
    IBUS_CHEWING_LOG(INFO, "on_comboBox_changed_wrap(), key=%s value=%s",
		     ctx->spec->key, g_value_get_string(&value));
    ctx->spec->setFunc(ctx, &value);
}

static void on_entry_activate_wrap(GtkEntry * entry, gpointer userData)
{
    PropertyContext *ctx = (PropertyContext *) userData;
    GValue value = { 0 };
    g_value_init(&value, ctx->spec->valueType);
    g_value_set_string(&value, gtk_entry_get_text(entry));
    IBUS_CHEWING_LOG(INFO, "on_entry_activate_wrap(), key=%s value=%s",
		     ctx->spec->key, g_value_get_string(&value));
    ctx->spec->setFunc(ctx, &value);
}

static void on_spinButton_value_changed_wrap(GtkSpinButton * button,
					     gpointer userData)
{
    PropertyContext *ctx = (PropertyContext *) userData;
    GValue value = { 0 };
    g_value_init(&value, ctx->spec->valueType);
    switch (ctx->spec->valueType) {
    case G_TYPE_INT:
	g_value_set_int(&value, (gint) gtk_spin_button_get_value(button));
	IBUS_CHEWING_LOG(INFO,
			 "on_spinButton_value_changed_wrap(), key=%s value=%d",
			 ctx->spec->key, g_value_get_int(&value));
	break;
    case G_TYPE_UINT:
	g_value_set_uint(&value,
			 (guint) gtk_spin_button_get_value(button));
	IBUS_CHEWING_LOG(INFO,
			 "on_spinButton_value_changed_wrap(), key=%s value=%u",
			 ctx->spec->key, g_value_get_uint(&value));
	break;
    case G_TYPE_DOUBLE:
	g_value_set_uint(&value, gtk_spin_button_get_value(button));
	IBUS_CHEWING_LOG(INFO,
			 "on_spinButton_value_changed_wrap(), key=%s value=%g",
			 ctx->spec->key, g_value_get_double(&value));
	break;
    default:
	break;
    }
    ctx->spec->setFunc(ctx, &value);
}

static void on_toggleButton_toggled_wrap(GtkToggleButton * button,
					 gpointer userData)
{
    PropertyContext *ctx = (PropertyContext *) userData;
    GValue value = { 0 };
    g_value_init(&value, ctx->spec->valueType);
    g_value_set_boolean(&value, gtk_toggle_button_get_active(button));
    IBUS_CHEWING_LOG(INFO, "on_entry_activate_wrap(), key=%s value=%s",
		     ctx->spec->key, g_value_get_string(&value));
    ctx->spec->setFunc(ctx, &value);
}

/*===== End of Widget Callback function wraps =====*/

typedef struct {
    MakerDialog *self;
    gint currentMaxWidth;
    const gchar *pageName;
    gfloat xalign;
    gfloat yalign;
} WidgetAlignment;

static void calculate_max_label_width_callback(gpointer key, gpointer value,
					      gpointer widgetAlignment)
{
    gchar * wKey = (gchar *) key;
    gchar * pageName = (gchar *) value;
    WidgetAlignment *wAlignment = (WidgetAlignment *) widgetAlignment;
    if (!STRING_EQUALS(wAlignment->pageName,pageName)){
	/* Different Page */
	return;
    }

    GtkWidget *widget =
	maker_dialog_get_widget(wAlignment->self, wKey, "label");
    GtkRequisition requisition;
    gtk_widget_size_request(widget, &requisition);
    wAlignment->currentMaxWidth =
	MAX(wAlignment->currentMaxWidth, requisition.width);
}

static void set_label_width_callback(gpointer key, gpointer value,
				     gpointer userData)
{
    WidgetAlignment *wAlignment = (WidgetAlignment *) userData;
    gchar * wKey = (gchar *) key;
    gchar * pageName = (gchar *) value;
    if (!STRING_EQUALS(wAlignment->pageName,pageName)){
	/* Different Page */
	return;
    }
    GtkWidget *widget =
	maker_dialog_get_widget(wAlignment->self, wKey, "label");
    gtk_widget_set_size_request(widget, wAlignment->currentMaxWidth, -1);
    gtk_misc_set_alignment(GTK_MISC(widget), wAlignment->xalign,
			   wAlignment->yalign);
    gtk_widget_show(widget);
}


/*============================================
 * Supporting functions
 */

/**
 * atob:
 * @string: A string.
 * @returns: Boolean value represented by @string.
 *
 * String to boolean.
 * It returns FALSE if:
 *    1. @string is NULL or have 0 length.
 *    2. @string starts with 'F', 'f', 'N' or 'n'.
 *    3. @string can be converted to a numeric 0.
 *
 * Everything else is TRUE.
 */
gboolean atob(const gchar * string)
{
    if (!string)
	return FALSE;
    if (strlen(string) <= 0)
	return FALSE;
    if (string[0] == 'F' || string[0] == 'f' || string[0] == 'N'
	    || string[0] == 'n')
	return FALSE;
    char *endPtr = NULL;
    long int longValue = strtol(string, &endPtr, 10);

    if (longValue == 0 && *endPtr == '\0') {
	// 0
	return FALSE;
    }
    return TRUE;
}

gchar * GValue_to_string(GValue * gValue){
    static gchar result[MAKER_DIALOG_VALUE_LENGTH];
    result[0]='\0';
    GType gType = g_value_get_gtype(gValue);
    guint uintValue;
    int intValue;
    switch(gType){
    case G_TYPE_BOOLEAN:
	if (g_value_get_boolean(gValue)){
	    g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "1");
	} else {
	    g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "0");
	}
	break;
    case G_TYPE_UINT:
	uintValue=g_value_get_uint(gValue);
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%u", uintValue);
	break;
    case G_TYPE_INT:
	intValue=g_value_get_int(gValue);
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, "%d", intValue);
	break;
    case G_TYPE_STRING:
	g_snprintf(result, MAKER_DIALOG_VALUE_LENGTH, g_value_get_string(gValue));
	break;
    default:
	break;
    }
    return result;
}

gboolean maker_dialog_add_PropertySpec_callback(PropertyContext * ctx, gpointer userData)
{
    PropertySpec * pSpec=ctx->spec;
    MakerDialog * self=(MakerDialog *) ctx->userData;
    IBusChewingConfig * iConfig=(IBusChewingConfig *) userData;
    gchar * strValue;
    if (iConfig){
	GValue gValue= {0};
	IBusChewingConfig_get_value(iConfig, pSpec->key, &gValue);
	strValue = GValue_to_string(&gValue);
    } else {
	strValue = pSpec->defaultValue;
    }
    return maker_dialog_add_property(self, pSpec, strValue, NULL);
}

