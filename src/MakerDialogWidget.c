#define GETTEXT_PACKAGE "gtk20"
#include <glib/gi18n.h>
#include "MakerDialogUtil.h"
#include "MakerDialogWidget.h"

const gchar *mkdgButtonTextArray[MKDG_BUTTON_INDEX_INVALID + 1] = {
    NC_("Configure", "Cancel"),
    NC_("Configure", "Save"),
    NC_("Configure", "Apply"),
    NC_("Configure", "Close"),
    NC_("Configure", "Ok"),
    NULL
};

MkdgButtonResponse mkdg_button_get_response(MkdgButtonIndex index)
{
    switch (index) {
    case MKDG_BUTTON_INDEX_CANCEL:
	return MKDG_BUTTON_RESPONSE_CANCEL;
    case MKDG_BUTTON_INDEX_SAVE:
	return MKDG_BUTTON_RESPONSE_SAVE;
    case MKDG_BUTTON_INDEX_APPLY:
	return MKDG_BUTTON_RESPONSE_APPLY;
    case MKDG_BUTTON_INDEX_CLOSE:
	return MKDG_BUTTON_RESPONSE_CLOSE;
    case MKDG_BUTTON_INDEX_OK:
	return MKDG_BUTTON_RESPONSE_OK;
    default:
	break;
    }
    return MKDG_BUTTON_RESPONSE_INVALID;
}

const gchar *mkdg_button_get_text(MkdgButtonIndex index)
{
    if (index < 0) {
	return NULL;
    }
    if (index > MKDG_BUTTON_INDEX_INVALID) {
	return NULL;
    }
    return g_dpgettext2(NULL,"Configure",mkdgButtonTextArray[index]);
}

/*=====================================
 * MkdgWidget
 *
 */

static void mkdg_list_store_append(GtkListStore * listStore,
				   const gchar * str,
				   const gchar * translationContext,
				   MkdgPropertyFlags propertyFlags)
{
    GtkTreeIter iter;
    gtk_list_store_append(listStore, &iter);

    if (propertyFlags & MKDG_PROPERTY_FLAG_HAS_TRANSLATION) {
	if (STRING_IS_EMPTY(translationContext)) {
	    mkdg_log(DEBUG, "mkdg_list_store_append() str=%s, _(str)=%s",
		     str, _(str));
	    gtk_list_store_set(listStore, &iter, 0, str, 1, _(str), -1);
	} else {
	    mkdg_log(DEBUG, "mkdg_list_store_append() str=%s, _(str)=%s",
		     str, g_dpgettext2(NULL, translationContext, str));
	    gtk_list_store_set(listStore, &iter, 0, str, 1,
			       g_dpgettext2(NULL, translationContext, str),
			       -1);
	}
    } else {
	mkdg_log(DEBUG, "mkdg_list_store_append() str=%s", str);
	gtk_list_store_set(listStore, &iter, 0, str, -1);
    }
}

gint mkdg_list_store_find_string(GtkListStore * listStore,
				 const gchar * str,
				 const gchar * translationContext,
				 MkdgPropertyFlags propertyFlags)
{
    g_assert(str);
    mkdg_log(INFO, "mkdg_list_store_find_string(%s,%u)", str,
	     propertyFlags);
    gint i = 0, index = -1;
    GtkTreeIter iter;
    GValue gValue = { 0 };
    if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listStore), &iter)) {
	do {
	    gtk_tree_model_get_value(GTK_TREE_MODEL(listStore), &iter, 0,
				     &gValue);
	    if (STRING_EQUALS(str, g_value_get_string(&gValue))) {
		index = i;
		break;
	    }
	    i++;
	    g_value_unset(&gValue);
	} while (gtk_tree_model_iter_next
		 (GTK_TREE_MODEL(listStore), &iter));
    }
    if (index < 0 && !(propertyFlags & MKDG_PROPERTY_FLAG_NO_NEW)) {
	/* Add new item */
	mkdg_list_store_append(listStore, str, translationContext,
			       propertyFlags);
	index = i;
    }
    mkdg_log(DEBUG, "mkdg_list_store_find_string(%s,%u) index=%d", str,
	     propertyFlags, index);
    return index;
}

gint mkdg_combo_find_string_index(GtkComboBox * combo,
				  const gchar * str,
				  const gchar * translationContext,
				  MkdgPropertyFlags propertyFlags)
{
    GtkListStore *listStore =
	GTK_LIST_STORE(gtk_combo_box_get_model(combo));
    return mkdg_list_store_find_string(listStore, str, translationContext,
				       propertyFlags);
}

static GValue * mkdg_combo_get_active_text(GtkComboBox * combo)
{
    GtkTreeIter iter;
    if (!gtk_combo_box_get_active_iter(combo, &iter)) {
	return NULL;
    }
    GValue *value = g_new0(GValue, 1);
    GtkListStore *listStore =
	GTK_LIST_STORE(gtk_combo_box_get_model(combo));
    gtk_tree_model_get_value(GTK_TREE_MODEL(listStore), &iter, 0, value);
    return value;
}

/*=====================================
 * Widget Callback function wraps
 */

static void on_value_change(MkdgWidget * mWidget, GValue * value)
{
    if (!(mWidget->flags & MKDG_WIDGET_FLAG_SET_IMMEDIATELY)) {
	if (mWidget->flags & MKDG_WIDGET_FLAG_WRITE_IMMEDIATELY) {
	    mkdg_log(ERROR,
		     "MKDG_WIDGET_FLAG_SET_IMMEDIATELY is needed for MKDG_WIDGET_FLAG_WRITE_IMMEDIATELY");
	    return;
	} else if (mWidget->flags & MKDG_WIDGET_FLAG_APPLY_IMMEDIATELY) {
	    mkdg_log(ERROR,
		     "MKDG_WIDGET_FLAG_SET_IMMEDIATELY is needed for MKDG_WIDGET_FLAG_APPLY_IMMEDIATELY");
	    return;
	}
    }
    if (mWidget->flags & MKDG_WIDGET_FLAG_SET_IMMEDIATELY) {
	property_context_set(mWidget->ctx, value);
    }

    if (mWidget->flags & MKDG_WIDGET_FLAG_WRITE_IMMEDIATELY) {
	property_context_save(mWidget->ctx, value, NULL);
    }

    if (mWidget->flags & MKDG_WIDGET_FLAG_APPLY_IMMEDIATELY) {
	property_context_apply(mWidget->ctx, NULL);
    }
}

static void on_comboBox_changed_wrap(GtkComboBox * comboBox,
				     gpointer userData)
{
    MkdgWidget *mWidget = (MkdgWidget *) userData;
    //GValue gValue = { 0 };
    GValue *value = mkdg_combo_get_active_text(comboBox);
    mkdg_log(INFO, "on_comboBox_changed_wrap(), key=%s value=%s",
	     mWidget->ctx->spec->key, mkdg_g_value_to_string(value));
    on_value_change(mWidget, value);
    g_value_unset(value);
}

static void on_entry_activate_wrap(GtkEntry * entry, gpointer userData)
{
    MkdgWidget *mWidget = (MkdgWidget *) userData;
    GValue gValue = { 0 };
    g_value_init(&gValue, mWidget->ctx->spec->valueType);
    g_value_set_string(&gValue, gtk_entry_get_text(entry));
    mkdg_log(INFO, "on_entry_activate_wrap(), key=%s value=%s",
	     mWidget->ctx->spec->key, g_value_get_string(&gValue));
    on_value_change(mWidget, &gValue);
}

static void on_spinButton_value_changed_wrap(GtkSpinButton * button,
					     gpointer userData)
{
    MkdgWidget *mWidget = (MkdgWidget *) userData;
    GValue gValue = { 0 };
    g_value_init(&gValue, mWidget->ctx->spec->valueType);
    switch (mWidget->ctx->spec->valueType) {
    case G_TYPE_INT:
	g_value_set_int(&gValue, (gint) gtk_spin_button_get_value(button));
	mkdg_log(INFO,
		 "on_spinButton_value_changed_wrap(), key=%s value=%d",
		 mWidget->ctx->spec->key, g_value_get_int(&gValue));
	break;
    case G_TYPE_UINT:
	g_value_set_uint(&gValue,
			 (guint) gtk_spin_button_get_value(button));
	mkdg_log(INFO,
		 "on_spinButton_value_changed_wrap(), key=%s value=%u",
		 mWidget->ctx->spec->key, g_value_get_uint(&gValue));
	break;
    case G_TYPE_DOUBLE:
	g_value_set_double(&gValue, gtk_spin_button_get_value(button));
	mkdg_log(INFO,
		 "on_spinButton_value_changed_wrap(), key=%s value=%g",
		 mWidget->ctx->spec->key, g_value_get_double(&gValue));
	break;
    default:
	break;
    }
    on_value_change(mWidget, &gValue);
}

static void on_toggleButton_toggled_wrap(GtkToggleButton * button,
					 gpointer userData)
{
    MkdgWidget *mWidget = (MkdgWidget *) userData;
    GValue gValue = { 0 };
    g_value_init(&gValue, mWidget->ctx->spec->valueType);
    g_value_set_boolean(&gValue, gtk_toggle_button_get_active(button));
    mkdg_log(INFO, "on_entry_activate_wrap(), key=%s value=%s",
	     mWidget->ctx->spec->key, mkdg_g_value_to_string(&gValue));
    on_value_change(mWidget, &gValue);
}

/*=====================================
 * MkdgWgt subroutines
 */

gint mkdg_wgt_get_width(MkdgWgt * wgt)
{
    GtkRequisition requisition;
    gtk_widget_size_request(wgt, &requisition);
    return requisition.width;
}

void mkdg_wgt_set_width(MkdgWgt * wgt, gint width)
{
    gtk_widget_set_size_request(wgt, width, -1);
}

void mkdg_wgt_set_alignment(MkdgWgt * wgt, gfloat xAlign, gfloat yAlign)
{
    gtk_misc_set_alignment(GTK_MISC(wgt), xAlign, yAlign);
    if (GTK_IS_LABEL(wgt)){
	/* gtk_label_set_justify takes no effect on single line label,
	 * but multi-lined label need this
	 */
	gtk_label_set_justify(GTK_LABEL(wgt),GTK_JUSTIFY_RIGHT);
    }
}

void mkdg_wgt_show(MkdgWgt * wgt)
{
    gtk_widget_show(wgt);
}

void mkdg_wgt_show_all(MkdgWgt * wgt)
{
    gtk_widget_show_all(wgt);
}

void mkdg_wgt_destroy(MkdgWgt * wgt)
{
    gtk_widget_destroy(wgt);
}


/*=====================================
 * MkdgWidgetContainer subroutines
 */

MkdgWidgetContainer *mkdg_widget_container_new(MkdgWgt * wgt)
{
    if (wgt == NULL) {
	return NULL;
    }
    MkdgWidgetContainer *container = g_new0(MkdgWidgetContainer, 1);
    container->wgt = wgt;
    container->children = g_ptr_array_new();
    container->childrenLabelWidth = 0;
    return container;
}

void mkdg_widget_container_add_widget(MkdgWidgetContainer * container,
				      MkdgWidget * mWidget)
{
    if ((container == NULL) || (mWidget == NULL)) {
	return;
    }
    MkdgWgt *vbox = container->wgt;

    /* Add widget to the vbox */
    MkdgWgt *hbox = gtk_hbox_new(FALSE, MKDG_HBOX_SPACING_DEFAULT);
    MkdgWgt *labelWgt = MKDG_WGT(mWidget->label);
    MkdgWgt *wgt = mWidget->wgt;
    gtk_box_pack_start(GTK_BOX(hbox), labelWgt, FALSE, FALSE, 0);
    mkdg_wgt_show(labelWgt);
    gtk_box_pack_start(GTK_BOX(hbox), wgt, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    g_ptr_array_add(container->children, mWidget);
    container->childrenLabelWidth =
	MAX(container->childrenLabelWidth, mkdg_wgt_get_width(labelWgt));
}

void mkdg_widget_container_align_all_children(MkdgWidgetContainer *
					      container,
					      gfloat labelXAlign,
					      gfloat labelYAlign,
					      gfloat wgtXAlign,
					      gfloat wgtYAlign)
{
    if ((container == NULL) || (container->children == NULL)) {
	return;
    }

    gint i;
    for (i = 0; i < mkdg_widget_container_count_children(container); i++) {
	MkdgWidget *mWidget = g_ptr_array_index(container->children, i);
	/* Set Width and Aligning labels  */
	MkdgWgt *labelWgt = MKDG_WGT(mWidget->label);
	mkdg_wgt_set_width(labelWgt, container->childrenLabelWidth);
	mkdg_wgt_set_alignment(labelWgt, labelXAlign, labelYAlign);
	mkdg_wgt_set_alignment(mWidget->wgt, wgtXAlign, wgtYAlign);
    }
}

gint mkdg_widget_container_count_children(MkdgWidgetContainer * container)
{
    if ((container == NULL) || (container->children == NULL)) {
	return -1;
    }
    return container->children->len;
}

void mkdg_widget_cointainer_free(MkdgWidgetContainer * container)
{
    g_ptr_array_free(container->children, TRUE);
    g_free(container);
}

/*=====================================
 * MkdgWidget subroutines
 */

MkdgWidget *mkdg_widget_new(PropertyContext * ctx,
			    MkdgWidgetFlag widgetFlags)
{
    if (ctx == NULL) {
	return NULL;
    }
    mkdg_log(INFO, "mkdg_widget_new(%s,%x)", ctx->spec->key, widgetFlags);
    MkdgWidget *mWidget = g_new0(MkdgWidget, 1);

    MkdgWgt *wgt = NULL;
    GtkAdjustment *gAdjust = NULL;
    gboolean bValue;
    const gchar *strValue;
    gdouble numberValue;
    gdouble stepInc = 1.0;
    gdouble pageInc = 10.0;
    gdouble pageSize = 0.0;

    switch (ctx->spec->valueType) {
    case G_TYPE_BOOLEAN:
	bValue = g_value_get_boolean(property_context_get(ctx));
	wgt = gtk_check_button_new();
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(wgt), bValue);
	g_signal_connect(wgt, "toggled",
			 G_CALLBACK(on_toggleButton_toggled_wrap),
			 mWidget);
	break;
    case G_TYPE_UINT:
    case G_TYPE_INT:
	if (ctx->spec->valueType == G_TYPE_UINT) {
	    numberValue =
		(gdouble) g_value_get_uint(property_context_get(ctx));
	} else {
	    numberValue =
		(gdouble) g_value_get_int(property_context_get(ctx));
	}
	gAdjust = (GtkAdjustment *) gtk_adjustment_new(numberValue,
						       ctx->spec->min,
						       ctx->spec->max,
						       stepInc, pageInc,
						       pageSize);
	wgt = gtk_spin_button_new(gAdjust, 1.0, 0);
	g_signal_connect(wgt, "value-changed",
			 G_CALLBACK(on_spinButton_value_changed_wrap),
			 mWidget);
	break;
    case G_TYPE_STRING:
	strValue = g_value_get_string(property_context_get(ctx));
	if (ctx->spec->validValues) {
	    GtkListStore *listStore = NULL;
	    if (ctx->spec->
		propertyFlags & MKDG_PROPERTY_FLAG_HAS_TRANSLATION) {
		listStore =
		    gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	    } else {
		listStore = gtk_list_store_new(1, G_TYPE_STRING);
	    }
	    int i;
	    for (i = 0; ctx->spec->validValues[i] != NULL; i++) {
		/* Add new item */
		mkdg_list_store_append(listStore,
				       ctx->spec->validValues[i],
				       ctx->spec->translationContext,
				       ctx->spec->propertyFlags);
	    }
	    int index = mkdg_list_store_find_string(listStore, strValue,
						    ctx->spec->
						    translationContext,
						    ctx->spec->
						    propertyFlags);

	    if (ctx->spec->propertyFlags & MKDG_PROPERTY_FLAG_NO_NEW) {
		wgt =
		    gtk_combo_box_new_with_model(GTK_TREE_MODEL
						 (listStore));
		GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
		gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(wgt),
					   renderer, FALSE);
		if (ctx->spec->propertyFlags &
		    MKDG_PROPERTY_FLAG_HAS_TRANSLATION) {
		    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(wgt),
						   renderer, "text", 1,
						   NULL);
		} else {
		    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(wgt),
						   renderer, "text", 0,
						   NULL);
		}
	    } else {
		wgt =
		    gtk_combo_box_entry_new_with_model(GTK_TREE_MODEL
						       (listStore),
						       (ctx->spec->
							propertyFlags &
							MKDG_PROPERTY_FLAG_HAS_TRANSLATION)
						       ? 1 : 0);
	    }
	    gtk_combo_box_set_active(GTK_COMBO_BOX(wgt), index);
	    g_signal_connect(wgt, "changed",
			     G_CALLBACK(on_comboBox_changed_wrap),
			     mWidget);
	} else {
	    wgt = gtk_entry_new();
	    if (ctx->spec->max >= 0) {
		gtk_entry_set_max_length(GTK_ENTRY(wgt), ctx->spec->max);
	    }
	    gtk_entry_set_text(GTK_ENTRY(wgt), strValue);
	    gtk_editable_set_editable(GTK_EDITABLE(wgt),
				      !(ctx->spec->propertyFlags &
					MKDG_PROPERTY_FLAG_NO_NEW));
	    g_signal_connect(wgt, "activate",
			     G_CALLBACK(on_entry_activate_wrap), mWidget);
	}
	break;
    default:
	break;
    }
    if (wgt == NULL) {
	g_free(mWidget);
	return NULL;
    }
    mWidget->ctx = ctx;
    mWidget->wgt = wgt;
    MkdgWgtLabel *label = GTK_LABEL(gtk_label_new(_(ctx->spec->label)));
    mWidget->label = label;
    gtk_widget_set_tooltip_text(GTK_WIDGET(label), _(ctx->spec->tooltip));
    mWidget->flags = widgetFlags;
    mWidget->container = NULL;
    return mWidget;
}

MkdgWidget *mkdg_widget_new_in_container(PropertyContext * ctx,
					 MkdgWidgetFlag widgetFlags,
					 MkdgWidgetContainer * container)
{
    MkdgWidget *mWidget = mkdg_widget_new(ctx, widgetFlags);
    if (mWidget == NULL) {
	return NULL;
    }
    mkdg_widget_container_add_widget(container, mWidget);
    return mWidget;
}

GValue *mkdg_widget_get_widget_value(MkdgWidget * mWidget, GValue * value)
{
    if (mWidget == NULL) {
	return NULL;
    }
    if (mWidget->wgt == NULL) {
	return NULL;
    }
    switch (mWidget->ctx->spec->valueType) {
    case G_TYPE_BOOLEAN:
	g_value_set_boolean(value,
			    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
							 (mWidget->wgt)));
	break;
    case G_TYPE_INT:
	g_value_set_int(value, (gint)
			gtk_spin_button_get_value(GTK_SPIN_BUTTON
						  (mWidget->wgt)));
	break;
    case G_TYPE_UINT:
	g_value_set_uint(value, (guint)
			 gtk_spin_button_get_value(GTK_SPIN_BUTTON
						   (mWidget->wgt)));
	break;
    case G_TYPE_STRING:
	if (mWidget->ctx->spec->validValues) {
	    GValue * strValue =
		mkdg_combo_get_active_text(GTK_COMBO_BOX(mWidget->wgt));
	    mkdg_log(INFO, "mkdg_widget_get_widget_value(%s) %s",
		     mWidget->ctx->spec->key, mkdg_g_value_to_string(strValue));
	    g_value_copy(strValue,value);
	    g_value_unset(strValue);
	} else {
	    g_value_set_string(value,
			       gtk_entry_get_text(GTK_ENTRY
						  (mWidget->wgt)));
	}
	break;
    default:
	return NULL;
    }
    return value;
}

gboolean mkdg_widget_set_widget_value(MkdgWidget * mWidget, GValue * value)
{
    if (mWidget == NULL) {
	return FALSE;
    }
    if (mWidget->ctx == NULL) {
	return FALSE;
    }
    if (mWidget->wgt == NULL) {
	return FALSE;
    }

    switch (mWidget->ctx->spec->valueType) {
    case G_TYPE_BOOLEAN:
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(mWidget->wgt),
				     g_value_get_boolean(value));
	break;
    case G_TYPE_INT:
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(mWidget->wgt),
				  (gdouble) g_value_get_int(value));
	break;
    case G_TYPE_UINT:
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(mWidget->wgt),
				  (gdouble) g_value_get_uint(value));
	break;
    case G_TYPE_STRING:
	if (mWidget->ctx->spec->validValues) {
	    const gchar *str = g_value_get_string(value);
	    gint index =
		mkdg_combo_find_string_index(GTK_COMBO_BOX
					     (mWidget->wgt), str,
					     mWidget->ctx->spec->
					     translationContext,
					     mWidget->ctx->spec->
					     propertyFlags);
	    gtk_combo_box_set_active(GTK_COMBO_BOX(mWidget->wgt), index);
	} else {
	    g_value_set_string(value,
			       gtk_entry_get_text(GTK_ENTRY
						  (mWidget->wgt)));
	}
	break;
    default:
	/* Not supported */
	return FALSE;
    }
    return TRUE;
}
